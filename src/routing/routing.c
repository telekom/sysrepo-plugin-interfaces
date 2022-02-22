#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <linux/genetlink.h>
#include <linux/limits.h>
#include <linux/neighbour.h>
#include <linux/rtnetlink.h>
#include <linux/netlink.h>
#include <sys/socket.h>
#include <netlink/addr.h>
#include <netlink/cache.h>
#include <netlink/socket.h>
#include <netlink/route/link.h>
#include <netlink/route/route.h>
#include <net/if.h>

#include <sysrepo.h>
#include <sysrepo/xpath.h>

#include <utlist.h>

#include "route/next_hop.h"
#include "routing.h"
#include "context.h"
#include "common.h"
#include "rib.h"
#include "rib/list.h"
#include "rib/description_pair.h"
#include "route/list.h"
#include "route/list_hash.h"
#include "control_plane_protocol.h"
#include "sysrepo_types.h"
#include "utils/memory.h"

// subs
#include "subscription/change.h"
#include "subscription/rpc.h"
#include "subscription/operational.h"

// initial loading into the datastore
static int routing_load_data(struct routing_ctx *ctx, sr_session_ctx_t *session);
static int routing_load_ribs(struct routing_ctx *ctx, sr_session_ctx_t *session, struct lyd_node *routing_container_node);
static int routing_load_control_plane_protocols(struct routing_ctx *ctx, sr_session_ctx_t *session, struct lyd_node *routing_container_node);

// operational callbacks helpers
static int routing_build_protos_map(struct control_plane_protocol map[ROUTING_PROTOS_COUNT]);
static inline int routing_is_proto_type_known(int type);

// init
static bool routing_running_datastore_is_empty(sr_session_ctx_t *session);
static int static_routes_init(struct route_list_hash_element **ipv4_head, struct route_list_hash_element **ipv6_head);

int routing_sr_plugin_init_cb(sr_session_ctx_t *session, void **private_data)
{
	int error = 0;

	// sysrepo
	sr_session_ctx_t *startup_session = NULL;
	sr_conn_ctx_t *connection = NULL;
	sr_subscription_ctx_t *subscription = NULL;
	struct routing_ctx *ctx = NULL;

	*private_data = NULL;

	// allocate routing plugin context
	ctx = xmalloc(sizeof(*ctx));
	if (!ctx) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "unable to allocate memory for routing context struct");
		goto error_out;
	}

	// memset context to 0
	*ctx = (struct routing_ctx){0};

	// set to private data
	*private_data = ctx;

	connection = sr_session_get_connection(session);
	error = sr_session_start(connection, SR_DS_STARTUP, &startup_session);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_session_start error: %d -> %s", error, sr_strerror(error));
	}

	ctx->startup_session = startup_session;

	error = static_routes_init(&ctx->ipv4_static_routes_head, &ctx->ipv6_static_routes_head);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "static_routes_init error");
		goto error_out;
	}

	if (routing_running_datastore_is_empty(session)) {
		SRPLG_LOG_INF(PLUGIN_NAME, "running datasore is empty -> loading data");
		error = routing_load_data(ctx, session);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "error loading initial data into the datastore... exiting");
			goto error_out;
		}
		error = sr_copy_config(startup_session, BASE_YANG_MODEL, SR_DS_RUNNING, 0);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "sr_copy_config error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}
	}

	SRPLG_LOG_INF(PLUGIN_NAME, "subscribing to module change");

	// control-plane-protocol list module changes
	error = sr_module_change_subscribe(session, BASE_YANG_MODEL, "/" BASE_YANG_MODEL ":*", routing_module_change_cb, *private_data, 0, SR_SUBSCR_DEFAULT, &subscription);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_module_change_subscribe error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	SRPLG_LOG_INF(PLUGIN_NAME, "subscribing to interfaces operational data");

	// interface leaf-list oper data
	error = sr_oper_get_subscribe(session, BASE_YANG_MODEL, ROUTING_INTERFACES_CONTAINER_YANG_PATH, routing_oper_get_interfaces_cb, NULL, 0, &subscription);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_oper_get_items_subscribe error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	// RIB oper data
	error = sr_oper_get_subscribe(session, BASE_YANG_MODEL, ROUTING_RIB_LIST_YANG_PATH, routing_oper_get_rib_routes_cb, NULL, 1, &subscription);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_oper_get_items_subscribe error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	SRPLG_LOG_INF(PLUGIN_NAME, "subscribing to plugin RPCs/actions");

	// active-route RPC/action
	error = sr_rpc_subscribe(session, ROUTING_RIB_LIST_ACTIVE_ROUTE_RPC_PATH, routing_rpc_active_route_cb, NULL, 1, 1, &subscription);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_rpc_subscribe error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	goto out;
error_out:
	SRPLG_LOG_ERR(PLUGIN_NAME, "error occured while initializing the plugin -> %d", error);
out:
	return error;
}

static int static_routes_init(struct route_list_hash_element **ipv4_routes_head, struct route_list_hash_element **ipv6_routes_head)
{
	int nl_err = 0;
	int error = 0;

	struct nl_sock *socket = NULL;
	struct rtnl_route *route = NULL;
	struct nl_cache *cache = NULL;
	struct nl_cache *link_cache = NULL;
	struct route tmp_route = {0};
	struct rtnl_link *iface = NULL;
	char *if_name = NULL;
	int ifindex = 0;

	// *ipv4_routes = xmalloc(sizeof(struct route_list_hash));
	// *ipv6_routes = xmalloc(sizeof(struct route_list_hash));

	route_list_hash_init(ipv4_routes_head);
	route_list_hash_init(ipv6_routes_head);

	socket = nl_socket_alloc();
	if (socket == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "unable to init nl_sock struct...");
		goto error_out;
	}

	nl_err = nl_connect(socket, NETLINK_ROUTE);
	if (nl_err != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nl_connect failed (%d): %s", nl_err, nl_geterror(nl_err));
		goto error_out;
	}

	nl_err = rtnl_route_alloc_cache(socket, AF_UNSPEC, 0, &cache);
	if (nl_err != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_route_alloc_cache failed (%d): %s", nl_err, nl_geterror(nl_err));
		goto error_out;
	}

	nl_err = rtnl_link_alloc_cache(socket, AF_UNSPEC, &link_cache);
	if (nl_err != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_route_alloc_cache failed (%d): %s", nl_err, nl_geterror(nl_err));
		goto error_out;
	}

	route = (struct rtnl_route *) nl_cache_get_first(cache);
	while (route != NULL) {
		const int PROTO = rtnl_route_get_protocol(route);

		if (PROTO == RTPROT_STATIC) {
			const int AF = rtnl_route_get_family(route);
			// fill the route with info and add to the hash of the current RIB
			route_init(&tmp_route);
			route_set_preference(&tmp_route, rtnl_route_get_priority(route));

			// next-hop container -> TODO: see what about special type
			const int NEXTHOP_COUNT = rtnl_route_get_nnexthops(route);
			if (NEXTHOP_COUNT == 1) {
				struct rtnl_nexthop *nh = rtnl_route_nexthop_n(route, 0);
				ifindex = rtnl_route_nh_get_ifindex(nh);
				iface = rtnl_link_get(link_cache, ifindex);
				if_name = xstrdup(rtnl_link_get_name(iface));
				route_next_hop_set_simple(&tmp_route.next_hop, ifindex, if_name, rtnl_route_nh_get_gateway(nh));
				rtnl_link_put(iface);
			} else {
				rtnl_route_foreach_nexthop(route, foreach_nexthop, &tmp_route.next_hop);
			}

			// route-metadata/source-protocol
			route_set_source_protocol(&tmp_route, "ietf-routing:static");

			// add the route to the protocol's container
			if (AF == AF_INET) {
				// v4
				route_list_hash_add(ipv4_routes_head, rtnl_route_get_dst(route), &tmp_route);
			} else if (AF == AF_INET6) {
				// v6
				route_list_hash_add(ipv6_routes_head, rtnl_route_get_dst(route), &tmp_route);
			}

			route_free(&tmp_route);
		}
		route = (struct rtnl_route *) nl_cache_get_next((struct nl_object *) route);
	}

	goto out;

error_out:
	SRPLG_LOG_ERR(PLUGIN_NAME, "error initializing static routes");
	error = -1;
out:
	nl_cache_free(cache);
	nl_cache_free(link_cache);
	nl_socket_free(socket);
	return error;
}

void routing_sr_plugin_cleanup_cb(sr_session_ctx_t *session, void *private_data)
{
	struct routing_ctx *ctx = (struct routing_ctx *) private_data;

	sr_session_ctx_t *startup_session = ctx->startup_session;

	if (startup_session) {
		sr_session_stop(startup_session);
	}

	route_list_hash_free(&ctx->ipv4_static_routes_head);
	route_list_hash_free(&ctx->ipv6_static_routes_head);

	// release context memory
	FREE_SAFE(ctx);
}

static int routing_load_data(struct routing_ctx *ctx, sr_session_ctx_t *session)
{
	int error = 0;
	LY_ERR ly_err = LY_SUCCESS;
	const struct ly_ctx *ly_ctx = NULL;
	struct lyd_node *routing_container_node = NULL;

	ly_ctx = sr_acquire_context(sr_session_get_connection(session));
	if (ly_ctx == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "unable to get ly_ctx variable... exiting immediately");
		goto error_out;
	}

	// create the routing container node and pass it to other functions which will add to the given node
	ly_err = lyd_new_path(NULL, ly_ctx, ROUTING_CONTAINER_YANG_PATH, NULL, 0, &routing_container_node);
	if (ly_err != LY_SUCCESS) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "unable to create the routing container ly node");
		goto error_out;
	}

	error = routing_load_ribs(ctx, session, routing_container_node);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "routing_load_ribs failed : %d", error);
		goto error_out;
	}

	error = routing_load_control_plane_protocols(ctx, session, routing_container_node);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "routing_load_control_plane_protocols failed : %d", error);
		goto error_out;
	}

	// after all nodes have been added with no error -> edit the values and apply changes
	error = sr_edit_batch(session, routing_container_node, "merge");
	if (error != SR_ERR_OK) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_edit_batch error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}
	error = sr_apply_changes(session, 0);
	if (error != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_apply_changes error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	goto out;
error_out:
	SRPLG_LOG_ERR(PLUGIN_NAME, "unable to load initial data");
out:
	if (routing_container_node) {
		lyd_free_tree(routing_container_node);
	}
	return error;
}

static int routing_load_ribs(struct routing_ctx *ctx, sr_session_ctx_t *session, struct lyd_node *routing_container_node)
{
	// error handling
	int error = 0;
	int nl_err = 0;
	LY_ERR ly_err = LY_SUCCESS;

	// libyang
	struct lyd_node *ribs_node = NULL, *rib_node = NULL, *added_node = NULL;
	const struct ly_ctx *ly_ctx = NULL;

	// libnl
	struct nl_sock *socket = NULL;
	struct nl_cache *cache = NULL;
	struct rib_list_element *ribs_head = NULL, *ribs_iter = NULL;

	// temp buffers
	char list_buffer[PATH_MAX] = {0};

	ly_ctx = sr_acquire_context(sr_session_get_connection(session));
	if (!ly_ctx) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "unable to load external modules... exiting");
		error = -1;
		goto error_out;
	}

	// rib_list_init(&ribs_head);

	socket = nl_socket_alloc();
	if (socket == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "unable to init nl_sock struct...");
		goto error_out;
	}

	nl_err = nl_connect(socket, NETLINK_ROUTE);
	if (nl_err != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nl_connect failed (%d): %s", nl_err, nl_geterror(nl_err));
		goto error_out;
	}

	nl_err = rtnl_route_alloc_cache(socket, AF_UNSPEC, 0, &cache);
	if (nl_err != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_route_alloc_cache failed (%d): %s", nl_err, nl_geterror(nl_err));
		goto error_out;
	}

	nl_err = rtnl_route_read_table_names("/etc/iproute2/rt_tables");
	if (nl_err != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_route_read_table_names failed (%d): %s", nl_err, nl_geterror(nl_err));
		goto error_out;
	}

	nl_err = rtnl_route_read_protocol_names("/etc/iproute2/rt_protos");
	if (nl_err != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_route_read_table_names failed (%d): %s", nl_err, nl_geterror(nl_err));
		goto error_out;
	}

	error = routing_collect_ribs(cache, &ribs_head);
	if (error != 0) {
		goto error_out;
	}

	ly_err = lyd_new_path(routing_container_node, ly_ctx, ROUTING_RIBS_CONTAINER_YANG_PATH, NULL, 0, &ribs_node);
	if (ly_err != LY_SUCCESS) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "unable to create rib list container...");
		goto error_out;
	}

	// all RIBs loaded - add them to the initial config
	struct rib *iter = NULL;
	LL_FOREACH(ribs_head, ribs_iter)
	{
		iter = &ribs_iter->rib;
		SRPLG_LOG_DBG(PLUGIN_NAME, "adding table %s to the list", iter->name);

		// write the current adding table into the buffer
		snprintf(list_buffer, sizeof(list_buffer), "%s[name='%s-%s']", ROUTING_RIB_LIST_YANG_PATH, iter->address_family == AF_INET ? "ipv4" : "ipv6", iter->name);
		ly_err = lyd_new_path(ribs_node, ly_ctx, list_buffer, NULL, 0, &rib_node);
		if (ly_err != LY_SUCCESS) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "unable to add new list...");
			goto error_out;
		}

		// address family
		ly_err = lyd_new_path(rib_node, ly_ctx, "address-family", iter->address_family == AF_INET ? "ipv4" : "ipv6", 0, &added_node);
		if (ly_err != LY_SUCCESS) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "unable to add 'address-family' node to the tree");
			goto error_out;
		}

		// description
		if (iter->description[0] != 0) {
			ly_err = lyd_new_path(rib_node, ly_ctx, "description", iter->description, 0, &added_node);
			if (ly_err != LY_SUCCESS) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "unable to add 'description' node to the rib");
				goto error_out;
			}
		}
	}

	goto out;
error_out:
	SRPLG_LOG_ERR(PLUGIN_NAME, "error loading initial data");
	error = -1;

out:
	rib_list_free(&ribs_head);
	nl_socket_free(socket);
	nl_cache_free(cache);

	return error;
}

static int routing_load_control_plane_protocols(struct routing_ctx *ctx, sr_session_ctx_t *session, struct lyd_node *routing_container_node)
{
	// error handling
	int error = 0;
	int nl_err = 0;
	LY_ERR ly_err = LY_SUCCESS;

	// libyang
	const struct lys_module *ly_uv4mod = NULL, *ly_uv6mod = NULL;
	const struct ly_ctx *ly_ctx = NULL;
	struct lyd_node *cpp_container_node = NULL;
	struct lyd_node *cpp_list_node = NULL;
	struct lyd_node *static_routes_container_node = NULL;
	struct lyd_node *tmp_node = NULL;
	struct lyd_node *ipv4_container_node = NULL, *ipv6_container_node = NULL;
	struct lyd_node *route_node = NULL;
	struct lyd_node *nh_node = NULL, *nh_list_node = NULL;

	// temp buffers
	char list_buffer[PATH_MAX] = {0};
	char ip_buffer[INET6_ADDRSTRLEN] = {0};
	char prefix_buffer[INET6_ADDRSTRLEN + 1 + 3] = {0};
	char route_path_buffer[PATH_MAX] = {0};
	char xpath_buffer[256] = {0};

	// iterators
	struct route_list_hash_element *routes_hash_iter = NULL;

	// control-plane-protocol structs
	struct control_plane_protocol cpp_map[ROUTING_PROTOS_COUNT] = {0};

	ly_ctx = sr_acquire_context(sr_session_get_connection(session));
	if (ly_ctx) {
		ly_uv4mod = ly_ctx_get_module(ly_ctx, "ietf-ipv4-unicast-routing", "2018-03-13");
		ly_uv6mod = ly_ctx_get_module(ly_ctx, "ietf-ipv6-unicast-routing", "2018-03-13");
	} else {
		SRPLG_LOG_ERR(PLUGIN_NAME, "unable to load external modules... exiting");
		error = -1;
		goto error_out;
	}

	nl_err = rtnl_route_read_table_names("/etc/iproute2/rt_tables");
	if (nl_err != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_route_read_table_names failed (%d): %s", nl_err, nl_geterror(nl_err));
		goto error_out;
	}

	nl_err = rtnl_route_read_protocol_names("/etc/iproute2/rt_protos");
	if (nl_err != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_route_read_table_names failed (%d): %s", nl_err, nl_geterror(nl_err));
		goto error_out;
	}

	// build protocols map array
	error = routing_build_protos_map(cpp_map);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "unable to build protocols mapping array: %d", error);
		goto error_out;
	}

	ly_err = lyd_new_path(routing_container_node, ly_ctx, ROUTING_CONTROL_PLANE_PROTOCOLS_CONTAINER_YANG_PATH, NULL, 0, &cpp_container_node);
	if (ly_err != LY_SUCCESS) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "unable to create control plane protocols container...");
		goto error_out;
	}

	// control plane protocols container created - add all protocols to the inner list

	// add the data to the datastore
	for (size_t i = 0; i < ROUTING_PROTOS_COUNT; i++) {
		const struct control_plane_protocol *proto = &cpp_map[i];
		if (proto->initialized) {
			// write proto path - type + name are added automatically when creating the list node
			snprintf(list_buffer, sizeof(list_buffer), "%s[type=\"%s\"][name=\"%s\"]", ROUTING_CONTROL_PLANE_PROTOCOL_LIST_YANG_PATH, proto->type, proto->name);
			ly_err = lyd_new_path(cpp_container_node, ly_ctx, list_buffer, NULL, 0, &cpp_list_node);
			if (ly_err != LY_SUCCESS) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "unable to add new control plane protocol %s", proto->name);
				goto error_out;
			}

			// description
			ly_err = lyd_new_path(cpp_list_node, ly_ctx, "description", proto->description, 0, &tmp_node);
			if (ly_err != LY_SUCCESS) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "unable to add 'description' node for the control plane protocol %s...", proto->name);
				goto error_out;
			}

			// static protocol -> static-routes
			if (strncmp(proto->name, "static", sizeof("static") - 1) == 0) {
				ly_err = lyd_new_path(cpp_list_node, ly_ctx, "static-routes", NULL, 0, &static_routes_container_node);
				if (ly_err != LY_SUCCESS) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "unable to add static-routes container node... exiting");
					goto error_out;
				}

				// for the static protocol add ipv4 and ipv6 static routes - each destination prefix => one route => one nexthop + description
				ly_err = lyd_new_inner(static_routes_container_node, ly_uv4mod, "ipv4", false, &ipv4_container_node);
				if (ly_err != LY_SUCCESS) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "unable to add ipv4 container node... exiting");
					goto error_out;
				}
				ly_err = lyd_new_inner(static_routes_container_node, ly_uv6mod, "ipv6", false, &ipv6_container_node);
				if (ly_err != LY_SUCCESS) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "unable to add ipv6 container node... exiting");
					goto error_out;
				}

				LL_FOREACH(ctx->ipv4_static_routes_head, routes_hash_iter)
				{
					const struct nl_addr *DST_PREFIX = routes_hash_iter->prefix;
					const struct route *ROUTE = &routes_hash_iter->routes_head->route;
					const union route_next_hop_value *NEXTHOP = &ROUTE->next_hop.value;

					// configure prefix
					nl_addr2str(DST_PREFIX, ip_buffer, sizeof(ip_buffer));
					if (strchr(ip_buffer, '/') == NULL) {
						snprintf(prefix_buffer, sizeof(prefix_buffer), "%s/%d", ip_buffer, nl_addr_get_prefixlen(DST_PREFIX));
					} else {
						snprintf(prefix_buffer, sizeof(prefix_buffer), "%s", ip_buffer);
					}

					// create new list node
					snprintf(route_path_buffer, sizeof(route_path_buffer), "route[destination-prefix=\"%s\"]", prefix_buffer);
					ly_err = lyd_new_path(ipv4_container_node, ly_ctx, route_path_buffer, NULL, 0, &route_node);
					if (ly_err != LY_SUCCESS) {
						SRPLG_LOG_ERR(PLUGIN_NAME, "unable to create a new route node for the list %s", route_path_buffer);
						goto error_out;
					}

					// description
					if (ROUTE->metadata.description != NULL) {
						ly_err = lyd_new_term(route_node, ly_uv4mod, "description", ROUTE->metadata.description, false, &tmp_node);
						if (ly_err != LY_SUCCESS) {
							SRPLG_LOG_ERR(PLUGIN_NAME, "unable to add description leaf to the %s node", route_path_buffer);
							goto error_out;
						}
					}

					// next-hop container
					ly_err = lyd_new_inner(route_node, ly_uv4mod, "next-hop", false, &nh_node);
					if (ly_err != LY_SUCCESS) {
						SRPLG_LOG_ERR(PLUGIN_NAME, "unable to create new next-hop container for the node %s", route_path_buffer);
						goto error_out;
					}
					switch (ROUTE->next_hop.kind) {
						case route_next_hop_kind_none:
							break;
						case route_next_hop_kind_simple: {
							// next-hop-address
							if (NEXTHOP->simple.addr) {
								nl_addr2str(NEXTHOP->simple.addr, ip_buffer, sizeof(ip_buffer));
								ly_err = lyd_new_term(nh_node, ly_uv4mod, "next-hop-address", ip_buffer, false, &tmp_node);
								if (ly_err != LY_SUCCESS) {
									SRPLG_LOG_ERR(PLUGIN_NAME, "unable to create next-hop-address leaf for the node %s", route_path_buffer);
									goto error_out;
								}

								ly_err = lyd_new_term(nh_node, ly_uv4mod, "outgoing-interface", NEXTHOP->simple.if_name, false, &tmp_node);
								if (ly_err != LY_SUCCESS) {
									SRPLG_LOG_ERR(PLUGIN_NAME, "unable to create outgoing-interface leaf for the node %s", route_path_buffer);
									goto error_out;
								}
							}
							break;
						}
						case route_next_hop_kind_special:
							break;
						case route_next_hop_kind_list: {
							struct route_next_hop_list_element *nexthop_iter = NULL;

							LL_FOREACH(ROUTE->next_hop.value.list_head, nexthop_iter)
							{
								error = snprintf(xpath_buffer, sizeof(xpath_buffer), "next-hop/next-hop-list]/next-hop[index=%d]", nexthop_iter->simple.ifindex);
								if (error < 0) {
									SRPLG_LOG_ERR(PLUGIN_NAME, "unable to create new next-hop-list/next-hop node, couldn't retrieve interface index");
									goto error_out;
								}

								ly_err = lyd_new_any(nh_node, ly_uv4mod, xpath_buffer, NULL, true, LYD_ANYDATA_STRING, false, &nh_list_node);
								if (ly_err != LY_SUCCESS) {
									SRPLG_LOG_ERR(PLUGIN_NAME, "unable to create new next-hop-list/next-hop list for the node %s", route_path_buffer);
									SRPLG_LOG_ERR(PLUGIN_NAME, "ly err = %d", ly_err);
									goto error_out;
								}
								// next-hop-address
								if (nexthop_iter->simple.addr) {
									nl_addr2str(nexthop_iter->simple.addr, ip_buffer, sizeof(ip_buffer));
									ly_err = lyd_new_term(nh_list_node, ly_uv4mod, "next-hop-address", ip_buffer, false, &tmp_node);
									if (ly_err != LY_SUCCESS) {
										SRPLG_LOG_ERR(PLUGIN_NAME, "unable to create next-hop-address leaf in the list for route %s", route_path_buffer);
										goto error_out;
									}

									ly_err = lyd_new_term(nh_list_node, ly_uv4mod, "outgoing-interface", nexthop_iter->simple.if_name, false, &tmp_node);
									if (ly_err != LY_SUCCESS) {
										SRPLG_LOG_ERR(PLUGIN_NAME, "unable to create outgoing-interface leaf in the list for route %s", route_path_buffer);
										goto error_out;
									}
								}
							}
							break;
						}
					}
				}

				LL_FOREACH(ctx->ipv6_static_routes_head, routes_hash_iter)
				{
					const struct nl_addr *DST_PREFIX = routes_hash_iter->prefix;
					const struct route *ROUTE = &routes_hash_iter->routes_head->route;
					const union route_next_hop_value *NEXTHOP = &ROUTE->next_hop.value;

					// configure prefix
					nl_addr2str(DST_PREFIX, ip_buffer, sizeof(ip_buffer));
					if (strchr(ip_buffer, '/') == NULL) {
						snprintf(prefix_buffer, sizeof(prefix_buffer), "%s/%d", ip_buffer, nl_addr_get_prefixlen(DST_PREFIX));
					} else {
						snprintf(prefix_buffer, sizeof(prefix_buffer), "%s", ip_buffer);
					}

					// create new list node
					snprintf(route_path_buffer, sizeof(route_path_buffer), "route[destination-prefix=\"%s\"]", prefix_buffer);
					ly_err = lyd_new_path(ipv6_container_node, ly_ctx, route_path_buffer, NULL, 0, &route_node);
					if (ly_err != LY_SUCCESS) {
						SRPLG_LOG_ERR(PLUGIN_NAME, "unable to create a new route node for the list %s", route_path_buffer);
						goto error_out;
					}

					// description
					if (ROUTE->metadata.description != NULL) {
						ly_err = lyd_new_term(route_node, ly_uv6mod, "description", ROUTE->metadata.description, false, &tmp_node);
						if (ly_err != LY_SUCCESS) {
							SRPLG_LOG_ERR(PLUGIN_NAME, "unable to add description leaf to the %s node", route_path_buffer);
							goto error_out;
						}
					}

					// next-hop container
					ly_err = lyd_new_inner(route_node, ly_uv6mod, "next-hop", false, &nh_node);
					if (ly_err != LY_SUCCESS) {
						SRPLG_LOG_ERR(PLUGIN_NAME, "unable to create new next-hop container for the node %s", route_path_buffer);
						goto error_out;
					}
					switch (ROUTE->next_hop.kind) {
						case route_next_hop_kind_none:
							break;
						case route_next_hop_kind_simple: {
							// next-hop-address
							if (NEXTHOP->simple.addr) {
								nl_addr2str(NEXTHOP->simple.addr, ip_buffer, sizeof(ip_buffer));
								ly_err = lyd_new_term(nh_node, ly_uv6mod, "next-hop-address", ip_buffer, false, &tmp_node);
								if (ly_err != LY_SUCCESS) {
									SRPLG_LOG_ERR(PLUGIN_NAME, "unable to create next-hop-address leaf for the node %s", route_path_buffer);
									goto error_out;
								}

								ly_err = lyd_new_term(nh_node, ly_uv6mod, "outgoing-interface", NEXTHOP->simple.if_name, false, &tmp_node);
								if (ly_err != LY_SUCCESS) {
									SRPLG_LOG_ERR(PLUGIN_NAME, "unable to create outgoing-interface leaf for the node %s", route_path_buffer);
									goto error_out;
								}
							}
							break;
						}
						case route_next_hop_kind_special:
							break;
						case route_next_hop_kind_list: {
							struct route_next_hop_list_element *nexthop_iter = NULL;
							LL_FOREACH(ROUTE->next_hop.value.list_head, nexthop_iter)
							{
								error = snprintf(xpath_buffer, sizeof(xpath_buffer), "next-hop/next-hop-list/next-hop[index=%d]", nexthop_iter->simple.ifindex);
								if (error < 0) {
									SRPLG_LOG_ERR(PLUGIN_NAME, "unable to create new next-hop-list/next-hop node, couldn't retrieve interface index");
									goto error_out;
								}

								ly_err = lyd_new_any(nh_node, ly_uv6mod, xpath_buffer, NULL, true, LYD_ANYDATA_STRING, false, &nh_list_node);
								if (ly_err != LY_SUCCESS) {
									SRPLG_LOG_ERR(PLUGIN_NAME, "unable to create new next-hop-list/next-hop list for the node %s", route_path_buffer);
									goto error_out;
								}
								// next-hop-address
								if (nexthop_iter->simple.addr) {
									nl_addr2str(nexthop_iter->simple.addr, ip_buffer, sizeof(ip_buffer));
									ly_err = lyd_new_term(nh_list_node, ly_uv6mod, "next-hop-address", ip_buffer, false, &tmp_node);
									if (ly_err != LY_SUCCESS) {
										SRPLG_LOG_ERR(PLUGIN_NAME, "unable to create next-hop-address leaf in the list for route %s", route_path_buffer);
										goto error_out;
									}

									ly_err = lyd_new_term(nh_list_node, ly_uv6mod, "outgoing-interface", nexthop_iter->simple.if_name, false, &tmp_node);
									if (ly_err != LY_SUCCESS) {
										SRPLG_LOG_ERR(PLUGIN_NAME, "unable to create outgoing-interface leaf in the list for route %s", route_path_buffer);
										goto error_out;
									}
								}
							}
							break;
						}
					}
				}
			}
		}
	}

	goto out;

error_out:
	SRPLG_LOG_ERR(PLUGIN_NAME, "error loading initial data for control-plane-procotols container");
	error = 1;
out:
	// free all control plane protocol structs
	for (size_t i = 0; i < ROUTING_PROTOS_COUNT; i++) {
		control_plane_protocol_free(&cpp_map[i]);
	}

	return error;
}

static int routing_build_protos_map(struct control_plane_protocol map[ROUTING_PROTOS_COUNT])
{
	int error = 0;

	const char *known_types_map[ROUTING_PROTOS_COUNT] = {
		[RTPROT_UNSPEC] = "ietf-routing:direct",
		[RTPROT_REDIRECT] = "ietf-routing:direct",
		[RTPROT_KERNEL] = "ietf-routing:direct",
		[RTPROT_BOOT] = "ietf-routing:direct",
		[RTPROT_STATIC] = "ietf-routing:static",
		[RTPROT_GATED] = "ietf-routing:direct",
		[RTPROT_RA] = "ietf-routing:direct",
		[RTPROT_MRT] = "ietf-routing:direct",
		[RTPROT_ZEBRA] = "ietf-routing:direct",
		[RTPROT_BIRD] = "ietf-routing:direct",
		[RTPROT_DNROUTED] = "ietf-routing:direct",
		[RTPROT_XORP] = "ietf-routing:direct",
		[RTPROT_NTK] = "ietf-routing:direct",
		[RTPROT_DHCP] = "ietf-routing:direct",
		[RTPROT_MROUTED] = "ietf-routing:direct",
		[RTPROT_BABEL] = "ietf-routing:direct",
		[RTPROT_BGP] = "ietf-routing:direct",
		[RTPROT_ISIS] = "ietf-routing:direct",
		[RTPROT_OSPF] = "ietf-routing:direct",
		[RTPROT_RIP] = "ietf-routing:direct",
		[RTPROT_EIGRP] = "ietf-routing:direct",
	};

	const char *known_descr_map[ROUTING_PROTOS_COUNT] = {
		[RTPROT_UNSPEC] = "unspecified protocol",
		[RTPROT_REDIRECT] = "redirect protocol",
		[RTPROT_KERNEL] = "kernel protocol",
		[RTPROT_BOOT] = "boot protocol",
		[RTPROT_STATIC] = "static protocol",
		[RTPROT_GATED] = "gated protocol",
		[RTPROT_RA] = "ra protocol",
		[RTPROT_MRT] = "mrt protocol",
		[RTPROT_ZEBRA] = "zebra protocol",
		[RTPROT_BIRD] = "bird protocol",
		[RTPROT_DNROUTED] = "dnrouted protocol",
		[RTPROT_XORP] = "xorp protocol",
		[RTPROT_NTK] = "ntk protocol",
		[RTPROT_DHCP] = "dhcp protocol",
		[RTPROT_MROUTED] = "mrouted protocol",
		[RTPROT_BABEL] = "babel protocol",
		[RTPROT_BGP] = "bgp protocol",
		[RTPROT_ISIS] = "isis protocol",
		[RTPROT_OSPF] = "ospf protocol",
		[RTPROT_RIP] = "rip protocol",
		[RTPROT_EIGRP] = "eigrp protocol",
	};

	for (int i = 0; i < ROUTING_PROTOS_COUNT; i++) {
		if (routing_is_proto_type_known(i)) {
			rtnl_route_proto2str(i, map[i].name, sizeof(map[i].name));
			map[i].type = xstrdup(known_types_map[i]);
			map[i].description = xstrdup(known_descr_map[i]);
			map[i].initialized = 1;
		}
	}

	return error;
}

static inline int routing_is_proto_type_known(int type)
{
	return type == RTPROT_UNSPEC ||
		   type == RTPROT_REDIRECT ||
		   type == RTPROT_KERNEL ||
		   type == RTPROT_BOOT ||
		   type == RTPROT_STATIC ||
		   type == RTPROT_GATED ||
		   type == RTPROT_RA ||
		   type == RTPROT_MRT ||
		   type == RTPROT_ZEBRA ||
		   type == RTPROT_BIRD ||
		   type == RTPROT_DNROUTED ||
		   type == RTPROT_XORP ||
		   type == RTPROT_NTK ||
		   type == RTPROT_DHCP ||
		   type == RTPROT_MROUTED ||
		   type == RTPROT_BABEL ||
		   type == RTPROT_BGP ||
		   type == RTPROT_ISIS ||
		   type == RTPROT_OSPF ||
		   type == RTPROT_RIP ||
		   type == RTPROT_EIGRP;
}

static bool routing_running_datastore_is_empty(sr_session_ctx_t *session)
{
	int error = SR_ERR_OK;
	bool is_empty = true;
	sr_val_t *values = NULL;
	size_t value_cnt = 0;

	error = sr_get_items(session, ROUTING_CONTROL_PLANE_PROTOCOL_LIST_YANG_PATH, 0, SR_OPER_DEFAULT, &values, &value_cnt);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_get_items error (%d): %s", error, sr_strerror(error));
		goto out;
	}

	// check if cpp list is empty
	if (value_cnt > 0) {
		sr_free_values(values, value_cnt);
		is_empty = false;
	}

out:
	return is_empty;
}
