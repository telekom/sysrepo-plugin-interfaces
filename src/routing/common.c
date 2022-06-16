/*
 * telekom / sysrepo-plugin-interfaces
 *
 * This program is made available under the terms of the
 * BSD 3-Clause license which is available at
 * https://opensource.org/licenses/BSD-3-Clause
 *
 * SPDX-FileCopyrightText: 2022 Deutsche Telekom AG
 * SPDX-FileContributor: Sartura Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "common.h"
#include <utils/memory.h>
#include <routing/route/next_hop.h>

#include <sysrepo.h>

#include <netlink/route/route.h>
#include <netlink/route/link.h>

#include <utlist.h>

void foreach_nexthop(struct rtnl_nexthop *nh, void *arg)
{
	struct route_next_hop *nexthop = arg;
	struct nl_cache *link_cache = NULL;
	struct nl_sock *socket = NULL;
	struct rtnl_link *iface = NULL;
	int ifindex = 0;
	char *if_name = NULL;
	int nl_err = 0;

	socket = nl_socket_alloc();
	if (socket == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "unable to init nl_sock struct...");
		return;
	}

	nl_err = nl_connect(socket, NETLINK_ROUTE);
	if (nl_err != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nl_connect failed (%d): %s", nl_err, nl_geterror(nl_err));
		return;
	}

	nl_err = rtnl_link_alloc_cache(socket, AF_UNSPEC, &link_cache);
	if (nl_err != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_route_alloc_cache failed (%d): %s", nl_err, nl_geterror(nl_err));
		return;
	}

	ifindex = rtnl_route_nh_get_ifindex(nh);
	iface = rtnl_link_get(link_cache, ifindex);
	if_name = xstrdup(rtnl_link_get_name(iface));

	route_next_hop_add_list(nexthop, ifindex, if_name, rtnl_route_nh_get_gateway(nh));

	rtnl_link_put(iface);
	nl_cache_free(link_cache);
	nl_socket_free(socket);
}

int routing_collect_ribs(struct nl_cache *routes_cache, struct rib_list_element **ribs_head)
{
	int error = 0;
	struct rtnl_route *route = NULL;
	char table_buffer[32] = {0};

	route = (struct rtnl_route *) nl_cache_get_first(routes_cache);

	while (route != NULL) {
		// fetch table name
		const int table_id = (int) rtnl_route_get_table(route);
		const uint8_t af = rtnl_route_get_family(route);
		rtnl_route_table2str(table_id, table_buffer, sizeof(table_buffer));

		// add the table to the list and set properties
		rib_list_add(ribs_head, table_buffer, af);

		// default table is main (same as iproute2) - for both ipv4 and ipv6 addresses
		if (strncmp(table_buffer, "main", sizeof("main") - 1) == 0) {
			rib_list_set_default(ribs_head, table_buffer, af, 1);
		}

		route = (struct rtnl_route *) nl_cache_get_next((struct nl_object *) route);
	}

	// after the list is finished -> build descriptions for all ribs
	error = routing_build_rib_descriptions(ribs_head);
	if (error != 0) {
		SRPLG_LOG_DBG(PLUGIN_NAME, "routing_build_rib_descriptions() failed");
		goto error_out;
	}

	goto out;

error_out:
	SRPLG_LOG_ERR(PLUGIN_NAME, "error collecting RIBs: %d", error);

out:
	return error;
}

int routing_collect_routes(struct nl_cache *routes_cache, struct nl_cache *link_cache, struct rib_list_element **ribs_head)
{
	int error = 0;
	struct rtnl_route *route = NULL;
	struct route tmp_route = {0};
	char table_buffer[32] = {0};
	struct rib *tmp_rib = NULL;
	struct rtnl_link *iface = NULL;
	int ifindex = 0;
	char *if_name = NULL;
	struct rib_list_element *ribs_iter = NULL;
	struct route_list_hash_element *routes_hash_iter = NULL;
	struct route_list_element *routes_iter = NULL;

	error = routing_collect_ribs(routes_cache, ribs_head);
	if (error != 0) {
		goto error_out;
	}

	// gather all routes for each table
	route = (struct rtnl_route *) nl_cache_get_first(routes_cache);
	while (route != NULL) {
		// fetch table name
		const int table_id = (int) rtnl_route_get_table(route);
		const int route_type = (int) rtnl_route_get_type(route);
		const uint8_t af = rtnl_route_get_family(route);
		rtnl_route_table2str(table_id, table_buffer, sizeof(table_buffer));

		// get the current RIB of the route
		tmp_rib = rib_list_get(ribs_head, table_buffer, af);
		if (tmp_rib == NULL) {
			error = -1;
			goto error_out;
		}

		// fill the route with info and add to the hash of the current RIB
		route_init(&tmp_route);
		route_set_preference(&tmp_route, rtnl_route_get_priority(route));

		// next-hop container
		switch (route_type) {
			case RTN_BLACKHOLE:
				route_next_hop_set_special(&tmp_route.next_hop, "blackhole");
				break;
			case RTN_UNREACHABLE:
				route_next_hop_set_special(&tmp_route.next_hop, "unreachable");
				break;
			case RTN_PROHIBIT:
				route_next_hop_set_special(&tmp_route.next_hop, "prohibit");
				break;
			case RTN_LOCAL:
				route_next_hop_set_special(&tmp_route.next_hop, "local");
				break;
			default: {
				const int NEXTHOP_COUNT = rtnl_route_get_nnexthops(route);
				if (NEXTHOP_COUNT == 1) {
					struct rtnl_nexthop *nh = rtnl_route_nexthop_n(route, 0);
					ifindex = rtnl_route_nh_get_ifindex(nh);
					iface = rtnl_link_get(link_cache, ifindex);
					if_name = rtnl_link_get_name(iface);
					route_next_hop_set_simple(&tmp_route.next_hop, ifindex, if_name, rtnl_route_nh_get_gateway(nh));

					// free recieved link
					rtnl_link_put(iface);
				} else {
					rtnl_route_foreach_nexthop(route, foreach_nexthop, &tmp_route.next_hop);
				}
			}
		}

		// route-metadata/source-protocol
		if (rtnl_route_get_protocol(route) == RTPROT_STATIC) {
			route_set_source_protocol(&tmp_route, "ietf-routing:static");
		} else {
			route_set_source_protocol(&tmp_route, "ietf-routing:direct");
		}

		// add the created route to the hash by a destination address
		route_list_hash_add(&tmp_rib->routes_head, rtnl_route_get_dst(route), &tmp_route);

		// last-updated -> TODO: implement later
		route_free(&tmp_route);
		route = (struct rtnl_route *) nl_cache_get_next((struct nl_object *) route);
	}

	LL_FOREACH(*ribs_head, ribs_iter)
	{
		struct route_list_hash_element **routes_hash_head = &ribs_iter->rib.routes_head;

		LL_FOREACH(*routes_hash_head, routes_hash_iter)
		{
			struct route_list_element **routes_list_head = &routes_hash_iter->routes_head;
			if (*routes_list_head != NULL) {
				struct route *pref = &(*routes_list_head)->route;

				LL_FOREACH(*routes_list_head, routes_iter)
				{
					// struct route *ptr = &ls->list[k];
					if (routes_iter->route.preference < pref->preference) {
						pref = &routes_iter->route;
					}
				}

				pref->metadata.active = 1;
			}
		}
	}

error_out:
	return error;
}

int routing_build_rib_descriptions(struct rib_list_element **ribs_head)
{
	int error = 0;

	char name_buffer[32 + 5] = {0};
	struct rib_list_element *ribs_iter = NULL;

	struct rib_description_pair default_descriptions[] = {
		{"ipv4-main", "main routing table - normal routing table containing all non-policy routes (ipv4 only)"},
		{"ipv6-main", "main routing table - normal routing table containing all non-policy routes (ipv6 only)"},
		{"ipv4-default", "default routing table - empty and reserved for post-processing if previous default rules did not select the packet (ipv4 only)"},
		{"ipv6-default", "default routing table - empty and reserved for post-processing if previous default rules did not select the packet (ipv6 only)"},
		{"ipv4-local", "local routing table - maintained by the kernel, containing high priority control routes for local and broadcast addresses (ipv4 only)"},
		{"ipv6-local", "local routing table - maintained by the kernel, containing high priority control routes for local and broadcast addresses (ipv6 only)"},
	};

	// now iterate over each rib and set its description
	LL_FOREACH(*ribs_head, ribs_iter)
	{
		const struct rib *RIB = &ribs_iter->rib;
		const int TABLE_ID = rtnl_route_str2table(RIB->name);

		// for default, local and main add prefixes, for other use given names
		if (routing_is_rib_known(TABLE_ID)) {
			snprintf(name_buffer, sizeof(name_buffer), "%s-%s", RIB->address_family == AF_INET ? "ipv4" : "ipv6", RIB->name);
		} else {
			snprintf(name_buffer, sizeof(name_buffer), "%s", RIB->name);
		}

		const size_t NAME_LEN = strlen(name_buffer);

		// set the description for each AF
		for (size_t j = 0; j < sizeof(default_descriptions) / sizeof(struct rib_description_pair); j++) {
			const struct rib_description_pair *PAIR = &default_descriptions[j];
			if (strncmp(name_buffer, PAIR->name, NAME_LEN) == 0) {
				memcpy(((struct rib *) RIB)->description, PAIR->description, sizeof(PAIR->description));
				break;
			}
		}
	}

	return error;
}

int routing_is_rib_known(int table)
{
	return table == RT_TABLE_DEFAULT || table == RT_TABLE_LOCAL || table == RT_TABLE_MAIN;
}

int routing_apply_new_routes(struct nl_sock *socket, struct route_list_hash_element *routes_hash)
{
	int error = 0;
	int nl_err = 0;

	// libnl
	struct rtnl_route *route = NULL;
	struct rtnl_nexthop *next_hop = NULL;
	struct nl_addr *dst_addr = NULL;

	// plugin
	struct route_list_hash_element *routes_iter = NULL;
	struct route_list_element *route_iter = NULL;

	LL_FOREACH(routes_hash, routes_iter)
	{
		LL_FOREACH(routes_iter->routes_head, route_iter)
		{
			route = rtnl_route_alloc();
			if (route == NULL) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "unable to alloc rtnl_route struct");
				goto error_out;
			}

			dst_addr = nl_addr_clone(routes_iter->prefix);

			rtnl_route_set_table(route, RT_TABLE_MAIN);
			rtnl_route_set_protocol(route, RTPROT_STATIC);
			rtnl_route_set_dst(route, dst_addr);
			rtnl_route_set_priority(route, route_iter->route.preference);

			if (route_iter->route.next_hop.kind == route_next_hop_kind_simple) {
				next_hop = rtnl_route_nh_alloc();
				if (next_hop == NULL) {
					error = -1;
					SRPLG_LOG_ERR(PLUGIN_NAME, "unable to alloc rtnl_nexthop struct");
					goto error_out;
				}

				if (route_iter->route.next_hop.value.simple.if_name == NULL && route_iter->route.next_hop.value.simple.addr == NULL) {
					error = -1;
					SRPLG_LOG_ERR(PLUGIN_NAME, "outgoing-interface and next-hop-address can't both be NULL");
					goto error_out;
				}

				if (route_iter->route.next_hop.value.simple.if_name != NULL) {
					rtnl_route_nh_set_ifindex(next_hop, route_iter->route.next_hop.value.simple.ifindex);
				}

				if (route_iter->route.next_hop.value.simple.addr != NULL) {
					rtnl_route_nh_set_gateway(next_hop, route_iter->route.next_hop.value.simple.addr);
				}
				rtnl_route_add_nexthop(route, next_hop);
			} else if (route_iter->route.next_hop.kind == route_next_hop_kind_list) {
				struct route_next_hop_list_element *nexthop_iter = NULL;

				LL_FOREACH(route_iter->route.next_hop.value.list_head, nexthop_iter)
				{
					next_hop = rtnl_route_nh_alloc();
					if (next_hop == NULL) {
						error = -1;
						SRPLG_LOG_ERR(PLUGIN_NAME, "unable to alloc rtnl_nexthop struct");
						goto error_out;
					}

					rtnl_route_nh_set_ifindex(next_hop, nexthop_iter->simple.ifindex);
					rtnl_route_nh_set_gateway(next_hop, nexthop_iter->simple.addr);
					rtnl_route_add_nexthop(route, next_hop);
				}
			}

			rtnl_route_set_scope(route, (uint8_t) rtnl_route_guess_scope(route));

			// create new route
			nl_err = rtnl_route_add(socket, route, NLM_F_CREATE);
			if (nl_err != 0) {
				error = -1;
				SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_route_add() failed (%d): %s", nl_err, nl_geterror(nl_err));
				goto error_out;
			}

			nl_addr_put(dst_addr);
			rtnl_route_put(route);
			route = NULL;
			dst_addr = NULL;
		}
	}

	goto out;

error_out:
	error = -1;

out:
	if (dst_addr) {
		nl_addr_put(dst_addr);
	}

	if (route) {
		rtnl_route_put(route);
	}

	return error;
}

int routing_apply_modify_routes(struct nl_sock *socket, struct route_list_hash_element *routes_hash)
{
	int error = 0;

	return error;
}

int routing_apply_delete_routes(struct nl_sock *socket, struct route_list_hash_element *routes_hash)
{
	int error = 0;
	int nl_err = 0;

	// libnl
	struct rtnl_route *route = NULL;
	struct nl_addr *dst_addr = NULL;

	// plugin
	struct route_list_hash_element *routes_iter = NULL;
	struct route_list_element *route_iter = NULL;

	LL_FOREACH(routes_hash, routes_iter)
	{
		LL_FOREACH(routes_iter->routes_head, route_iter)
		{
			route = rtnl_route_alloc();
			if (route == NULL) {
				error = -1;
				SRPLG_LOG_ERR(PLUGIN_NAME, "unable to alloc rtnl_route struct");
				goto error_out;
			}

			// alloc destination prefix for route
			dst_addr = nl_addr_clone(routes_iter->prefix);

			// setup route for deletion
			rtnl_route_set_table(route, RT_TABLE_MAIN);
			rtnl_route_set_protocol(route, RTPROT_STATIC);
			rtnl_route_set_dst(route, dst_addr);
			rtnl_route_set_priority(route, route_iter->route.preference);
			rtnl_route_set_scope(route, RT_SCOPE_NOWHERE);

			// delete route
			nl_err = rtnl_route_delete(socket, route, 0);
			if (nl_err != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_route_delete() failed (%d): %s", nl_err, nl_geterror(nl_err));
				error = -1;
				goto error_out;
			}

			nl_addr_put(dst_addr);
			rtnl_route_put(route);
			route = NULL;
			dst_addr = NULL;
		}
	}

error_out:
	if (dst_addr) {
		nl_addr_put(dst_addr);
	}

	if (route) {
		rtnl_route_put(route);
	}

	return error;
}