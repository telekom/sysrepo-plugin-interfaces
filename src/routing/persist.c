/*
 * telekom / sysrepo-plugin-interfaces
 *
 * This program is made available under the terms of the
 * BSD 3-Clause license which is available at
 * https://opensource.org/licenses/BSD-3-Clause
 *
 * SPDX-FileCopyrightText: 2021 Deutsche Telekom AG
 * SPDX-FileContributor: Sartura Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "persist.h"
#include "libyang/log.h"
#include "libyang/out.h"
#include "libyang/printer_data.h"
#include "libyang/tree_data.h"
#include "netlink/socket.h"
#include "route/list_hash.h"
#include "route/next_hop.h"

#include <net/if.h>
#include <routing/common.h>
#include <utils/memory.h>

#include <sysrepo.h>
#include <sysrepo/xpath.h>

#include <netlink/addr.h>

// gather routes from the running datastore which will be applied to the system
static int gather_static_routes(struct route_list_hash_element **routes_head, sr_session_ctx_t *session, const char *augment_path);

int routing_persist_static_routes(sr_session_ctx_t *session)
{
	int error = 0;
	struct route_list_hash_element *routes_head = NULL;
	struct nl_sock *socket = NULL;

	// netlink
	socket = nl_socket_alloc();
	if (socket == NULL) {
		error = -1;
		SRPLG_LOG_ERR(PLUGIN_NAME, "unable to init nl_sock struct...");
		goto error_out;
	}

	error = nl_connect(socket, NETLINK_ROUTE);
	if (error != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nl_connect() failed (%d): %s", error, nl_geterror(error));
		goto error_out;
	}

	error = gather_static_routes(&routes_head, session, ROUTING_STATIC_ROUTES_CONTAINER_YANG_PATH "/ietf-ipv4-unicast-routing:ipv4");
	if (error != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "Unable to gather static IPv4 routes from the running datastore");
		goto error_out;
	}

	error = gather_static_routes(&routes_head, session, ROUTING_STATIC_ROUTES_CONTAINER_YANG_PATH "/ietf-ipv6-unicast-routing:ipv6");
	if (error != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "Unable to gather static IPv6 routes from the running datastore");
		goto error_out;
	}

	error = routing_apply_new_routes(socket, routes_head);
	if (error != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "routing_apply_new_routes() failed (%d)", error);
		goto error_out;
	}

	goto out;

error_out:
	error = -1;

out:
	nl_socket_free(socket);
	route_list_hash_free(&routes_head);
	return error;
}

static int gather_static_routes(struct route_list_hash_element **routes_head, sr_session_ctx_t *session, const char *augment_path)
{
	int error = 0;
	LY_ERR ly_error = 0;

	// sysrepo
	sr_data_t *static_routes_container = NULL;

	// libnl
	struct nl_addr *prefix = NULL, *gateway = NULL;

	// libyang
	const struct ly_ctx *ly_context = sr_session_acquire_context(session);
	struct lyd_node *route_node_iterator = NULL;

	error = sr_get_subtree(session, augment_path, 0, &static_routes_container);
	if (error != SR_ERR_OK) {
		SRPLG_LOG_INF(PLUGIN_NAME, "sr_get_subtree() failed (%d): %s", error, sr_strerror(error));
		SRPLG_LOG_INF(PLUGIN_NAME, "Static routes path (%s) doesn't exist in the running datastore", augment_path);
		goto out;
	}

	route_node_iterator = lyd_child(static_routes_container->tree);

	while (route_node_iterator) {
		struct lyd_node *prefix_node = NULL, *next_hop_address = NULL, *outgoing_interface = NULL;

		ly_error = lyd_find_path(route_node_iterator, "destination-prefix", 0, &prefix_node);
		if (ly_error != LY_SUCCESS) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "lyd_find_path() failed (%d): %s", ly_error, ly_errmsg(ly_context));
			goto error_out;
		}

		ly_error = lyd_find_path(route_node_iterator, "next-hop/next-hop-address", 0, &next_hop_address);
		if (ly_error != LY_SUCCESS) {
			next_hop_address = NULL;
			SRPLG_LOG_INF(PLUGIN_NAME, "lyd_find_path() failed (%d): %s", ly_error, ly_errmsg(ly_context));
			SRPLG_LOG_INF(PLUGIN_NAME, "next-hop-address node not found, trying outgoing-interface");
		}

		ly_error = lyd_find_path(route_node_iterator, "next-hop/outgoing-interface", 0, &outgoing_interface);
		if (ly_error != LY_SUCCESS) {
			outgoing_interface = NULL;
			SRPLG_LOG_ERR(PLUGIN_NAME, "lyd_find_path() failed (%d): %s", ly_error, ly_errmsg(ly_context));
			if (!next_hop_address) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "unable to get next-hop-address or outgoing-interface node");
				goto error_out;
			}
		}

		SRPLG_LOG_DBG(PLUGIN_NAME, "destination-prefix: %s", lyd_get_value(prefix_node));

		error = nl_addr_parse(lyd_get_value(prefix_node), AF_UNSPEC, &prefix);
		if (error != 0) {
			SRPLG_LOG_INF(PLUGIN_NAME, "nl_addr_parse() failed (%d): %s", ly_error, ly_errmsg(ly_context));
		}

		// create new route
		route_list_hash_add(routes_head, prefix, &(struct route){0});
		struct route_list_element **route_list_head = route_list_hash_get(routes_head, prefix);

		nl_addr_put(prefix);

		if (next_hop_address != NULL) {
			SRPLG_LOG_DBG(PLUGIN_NAME, "next-hop: %s", lyd_get_value(next_hop_address));
			error = nl_addr_parse(lyd_get_value(next_hop_address), AF_UNSPEC, &gateway);
			if (error != 0) {
				SRPLG_LOG_INF(PLUGIN_NAME, "nl_addr_parse() failed (%d): %s", ly_error, ly_errmsg(ly_context));
			}
			route_next_hop_set_simple_gateway(&(*route_list_head)->route.next_hop, gateway);

			// cleanup
			nl_addr_put(gateway);
		}
		if (outgoing_interface != NULL) {
			SRPLG_LOG_DBG(PLUGIN_NAME, "outgoing-interface: %s", lyd_get_value(outgoing_interface));
			const int ifindex = (int) if_nametoindex(lyd_get_value(outgoing_interface));
			route_next_hop_set_simple_interface(&(*route_list_head)->route.next_hop, ifindex, lyd_get_value(outgoing_interface));
		}

		route_node_iterator = route_node_iterator->next;
	}

	goto out;

error_out:
	error = -1;

out:

	lyd_free_tree(static_routes_container->tree);
	FREE_SAFE(static_routes_container);

	return error;
}