#include "change.h"
#include "libyang/tree_data.h"
#include "netlink/errno.h"
#include "netlink/socket.h"
#include "route.h"
#include "route/list_hash.h"
#include "route/next_hop.h"
#include "sysrepo_types.h"
#include <linux/rtnetlink.h>
#include <routing/context.h>
#include <routing/common.h>
#include <utils/memory.h>

// stdlib
#include <net/if.h>

// sysrepo
#include <sysrepo.h>
#include <sysrepo/xpath.h>

// libnl
#include <netlink/route/route.h>

// uthash
#include <utlist.h>

static int set_control_plane_protocol_value(struct route_list_hash_element **ipv4_routes_head, struct route_list_hash_element **ipv6_routes_head, char *xpath, char *value);
static int delete_control_plane_protocol_value(struct route_list_hash_element **ipv4_routes_head, struct route_list_hash_element **ipv6_routes_head, char *xpath);
static int delete_static_route_value(struct route_list_hash_element **routes_hash_head, char *xpath, char *node_name, int family);
static int set_static_route_value(char *xpath, char *node_name, char *node_value, struct route_list_hash_element **routes_hash_head, int family);
static void set_static_route_description(struct route_list_element **routes_head, char *node_value);
static int set_static_route_simple_next_hop(struct route_list_element **routes_head, char *node_value, int family);
static int set_static_route_simple_outgoing_if(struct route_list_element **routes_head, char *node_value);
static int update_new_static_rotues(struct route_list_hash_element **routes_hash_head, uint8_t family);
static int update_delete_static_rotues(struct route_list_hash_element **routes_hash_head, uint8_t family);

static int apply_static_routes_changes(struct routing_ctx *ctx, sr_session_ctx_t *session, const char *base_xpath);
static int apply_new_routes(struct nl_sock *socket, struct route_list_hash_element *routes_hash);
static int apply_modify_routes(struct nl_sock *socket, struct route_list_hash_element *routes_hash);
static int apply_delete_routes(struct nl_sock *socket, struct route_list_hash_element *routes_hash);

int routing_control_plane_protocol_list_change_cb(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data)
{
	int error = 0;

	// context
	struct routing_ctx *ctx = (struct routing_ctx *) private_data;

	// xpath buffer
	char xpath_buffer[PATH_MAX] = {0};

	SRPLG_LOG_INF(PLUGIN_NAME, "Module Name: %s; XPath: %s; Event: %d, Request ID: %u", module_name, xpath, event, request_id);

	if (event == SR_EV_ABORT) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "aborting changes for: %s", xpath);
		error = -1;
		goto error_out;
	} else if (event == SR_EV_DONE) {
		error = sr_copy_config(ctx->startup_session, BASE_YANG_MODEL, SR_DS_RUNNING, 0);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "sr_copy_config error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}
	} else if (event == SR_EV_CHANGE) {
		// ipv4 static routes changes
		error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/static-routes/ietf-ipv4-unicast-routing:ipv4//.", xpath);
		if (error < 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() failed (%d)", error);
			goto error_out;
		}
		error = apply_static_routes_changes(ctx, session, xpath_buffer);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "apply_static_routes_changes() failed (%d) - ipv4", error);
			goto error_out;
		}

		// ipv6 static routes changes
		error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/static-routes/ietf-ipv6-unicast-routing:ipv6//.", xpath);
		if (error < 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() failed (%d)", error);
			goto error_out;
		}
		error = apply_static_routes_changes(ctx, session, xpath_buffer);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "apply_static_routes_changes() failed (%d) - ipv6", error);
			goto error_out;
		}
	}
	goto out;

error_out:
	SRPLG_LOG_ERR(PLUGIN_NAME, "error applying control plane protocols module changes");
	error = -1;

out:
	return error != 0 ? SR_ERR_CALLBACK_FAILED : SR_ERR_OK;
}

static int apply_static_routes_changes(struct routing_ctx *ctx, sr_session_ctx_t *session, const char *xpath)
{
	int error = 0;

	// buffers
	char change_path[PATH_MAX] = {0};

	// sysrepo
	sr_change_iter_t *changes_iterator = NULL;
	sr_change_oper_t operation = SR_OP_CREATED;
	const char *prev_value = NULL;
	const char *prev_list = NULL;
	int prev_default = 0;
	const char *node_value = NULL;
	const char *node_name = NULL;

	// libnl
	struct nl_addr *prefix = NULL, *gateway = NULL;
	struct nl_sock *socket = NULL;

	// libyang
	const struct lyd_node *node = NULL;

	// new routes to add
	struct route_list_hash_element *new_routes = NULL;

	// routes to modify
	struct route_list_hash_element *modify_routes = NULL;

	// existing routes to delete
	struct route_list_hash_element *delete_routes = NULL;

	// helper value
	struct route_list_element **routes_head = NULL;

	error = sr_get_changes_iter(session, xpath, &changes_iterator);
	if (error != SR_ERR_OK) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_get_changes_iter() failed (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	while (sr_get_change_tree_next(session, changes_iterator, &operation, &node, &prev_value, &prev_list, &prev_default) == SR_ERR_OK) {
		error = (lyd_path(node, LYD_PATH_STD, change_path, sizeof(change_path)) == NULL);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "lyd_path() error");
			goto error_out;
		}

		node_name = sr_xpath_node_name(change_path);
		node_value = lyd_get_value(node);

		SRPLG_LOG_DBG(PLUGIN_NAME, "Node Path: %s", change_path);
		SRPLG_LOG_DBG(PLUGIN_NAME, "Node Name: %s", node_name);
		SRPLG_LOG_DBG(PLUGIN_NAME, "Previous Value: %s; Value: %s; Operation: %d", prev_value, node_value, operation);

		switch (operation) {
			case SR_OP_CREATED:
				// data needed: destination-prefix and next-hop address
				if (!strncmp(node_name, "destination-prefix", sizeof("destination-prefix") - 1)) {
					// create new route
					error = nl_addr_parse(node_value, AF_UNSPEC, &prefix);
					if (error != 0) {
						SRPLG_LOG_ERR(PLUGIN_NAME, "nl_addr_parse() error (%d): %s", error, nl_geterror(error));
						goto error_out;
					}

					// add empty route to the start of the list for 'prefix'
					route_list_hash_add(&new_routes, prefix, &(struct route){0});
					routes_head = route_list_hash_get(&new_routes, prefix);

					// free created prefix
					nl_addr_put(prefix);
				} else if (!strncmp(node_name, "description", sizeof("description") - 1)) {
					if (!routes_head) {
						// error
						SRPLG_LOG_ERR(PLUGIN_NAME, "invalid routes_head value");
						goto error_out;
					} else {
						route_set_description(&(*routes_head)->route, node_value);
					}
				} else if (!strncmp(node_name, "next-hop-address", sizeof("next-hop-address") - 1)) {
					// simple next hop
					if (!routes_head) {
						SRPLG_LOG_ERR(PLUGIN_NAME, "invalid routes_head value");
						goto error_out;
					} else {
						error = nl_addr_parse(node_value, AF_UNSPEC, &gateway);
						if (error != 0) {
							SRPLG_LOG_ERR(PLUGIN_NAME, "nl_addr_parse() error (%d): %s", error, nl_geterror(error));
							goto error_out;
						}
						route_next_hop_set_simple_gateway(&(*routes_head)->route.next_hop, gateway);

						// free created gateway
						nl_addr_put(gateway);
					}
				}
				break;
			case SR_OP_MODIFIED:
				// data needed: next-hop container value
				break;
			case SR_OP_DELETED:
				// data needed: destination-prefix (optional next-hop value)
				break;
			default:
				break;
		}
	}

	// allocate libnl socket for all modifications (new, modify and delete)
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

	SRPLG_LOG_INF(PLUGIN_NAME, "applying recieved changes for static routes");

	error = apply_new_routes(socket, new_routes);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "apply_new_routes() error (%d)", error);
		goto error_out;
	}

	error = apply_modify_routes(socket, modify_routes);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "apply_modify_routes() error (%d)", error);
		goto error_out;
	}

	error = apply_delete_routes(socket, delete_routes);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "apply_delete_routes() error (%d)", error);
		goto error_out;
	}

	goto out;

error_out:
	error = -1;

out:
	// libnl
	if (socket) {
		nl_socket_free(socket);
	}

	// list hashes
	route_list_hash_free(&new_routes);
	route_list_hash_free(&modify_routes);
	route_list_hash_free(&delete_routes);

	// iterator
	sr_free_change_iter(changes_iterator);

	return error;
}

static int apply_new_routes(struct nl_sock *socket, struct route_list_hash_element *routes_hash)
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

static int apply_modify_routes(struct nl_sock *socket, struct route_list_hash_element *routes_hash)
{
	int error = 0;

	return error;
}

static int apply_delete_routes(struct nl_sock *socket, struct route_list_hash_element *routes_hash)
{
	int error = 0;

	return error;
}

static int set_control_plane_protocol_value(struct route_list_hash_element **ipv4_routes_head, struct route_list_hash_element **ipv6_routes_head, char *node_xpath, char *node_value)
{
	char *node_name = NULL;
	char *orig_xpath = NULL;

	int error = SR_ERR_OK;

	orig_xpath = xstrdup(node_xpath);

	node_name = sr_xpath_node_name(node_xpath);

	if (strstr(orig_xpath, "ietf-ipv4-unicast-routing:ipv4")) {
		error = set_static_route_value(orig_xpath, node_name, node_value, ipv4_routes_head, AF_INET);
		if (error != 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "error setting IPv4 static route value");
			goto out;
		}
	} else if (strstr(orig_xpath, "ietf-ipv6-unicast-routing:ipv6")) {
		error = set_static_route_value(orig_xpath, node_name, node_value, ipv6_routes_head, AF_INET6);
		if (error != 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "error setting IPv6 static route value");
			goto out;
		}
	}

out:
	if (orig_xpath) {
		FREE_SAFE(orig_xpath);
	}

	return error ? SR_ERR_CALLBACK_FAILED : SR_ERR_OK;
}

static int set_static_route_value(char *xpath, char *node_name, char *node_value, struct route_list_hash_element **routes_hash_head, int family)
{
	sr_xpath_ctx_t xpath_ctx = {0};

	struct nl_addr *destination_prefix_addr = NULL;

	// struct route_list *route_list = NULL;
	struct route_list_element **routes_head = NULL;
	char *next_hop_list = NULL;
	char *destination_prefix = NULL;
	int error = 0;

	next_hop_list = sr_xpath_key_value(xpath, "next-hop", "index", &xpath_ctx);
	destination_prefix = sr_xpath_key_value(xpath, "route", "destination-prefix", &xpath_ctx);

	if (destination_prefix == NULL) {
		error = -1;
		SRPLG_LOG_ERR(PLUGIN_NAME, "destination-prefix couldn't be retrieved");
		goto out;
	}

	error = nl_addr_parse(destination_prefix, family, &destination_prefix_addr);
	if (error != 0) {
		error = -1;
		SRPLG_LOG_ERR(PLUGIN_NAME, "failed to parse destination-prefix into nl_addr");
		goto out;
	}

	// there should exist a list for the current prefix
	if (strcmp(node_name, "destination-prefix") != 0) {
		routes_head = route_list_hash_get(routes_hash_head, destination_prefix_addr);
		if (routes_head == NULL) {
			error = -1;
			SRPLG_LOG_ERR(PLUGIN_NAME, "matching route_list destination-prefix %s not found", destination_prefix);
			goto out;
		}
	}

	if (next_hop_list != NULL) {
		(*routes_head)->route.next_hop.kind = route_next_hop_kind_list;

		if (!strcmp(node_name, "next-hop-address")) {
		} else if (!strcmp(node_name, "outgoing-interface")) {
		}
	} else if (!strcmp(node_name, "destination-prefix")) {
		route_list_hash_add(routes_hash_head, destination_prefix_addr, &(struct route){0});
		// route_list_hash_add_empty(routes_hash_head, destination_prefix_addr);
	} else if (!strcmp(node_name, "description")) {
		set_static_route_description(routes_head, node_value);
	} else if (!strcmp(node_name, "next-hop-address")) {
		error = set_static_route_simple_next_hop(routes_head, node_value, family);
		if (error) {
			error = -1;
			SRPLG_LOG_ERR(PLUGIN_NAME, "failed to set static route next-hop-address");
			goto out;
		}
	} else if (!strcmp(node_name, "outgoing-interface")) {
		error = set_static_route_simple_outgoing_if(routes_head, node_value);
		if (error) {
			error = -1;
			SRPLG_LOG_ERR(PLUGIN_NAME, "failed to set static route next-hop-address");
			goto out;
		}
	}

out:
	if (destination_prefix_addr) {
		nl_addr_put(destination_prefix_addr);
	}

	return error ? SR_ERR_CALLBACK_FAILED : SR_ERR_OK;
}

static void set_static_route_description(struct route_list_element **routes_head, char *node_value)
{
	(*routes_head)->route.metadata.description = node_value ? xstrdup(node_value) : NULL;
}

static int set_static_route_simple_next_hop(struct route_list_element **routes_head, char *node_value, int family)
{
	int error = 0;

	(*routes_head)->route.next_hop.kind = route_next_hop_kind_simple;

	error = nl_addr_parse(node_value, family, &(*routes_head)->route.next_hop.value.simple.addr);
	if (error != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "failed to parse next-hop-address into nl_addr");
		return -1;
	}

	return 0;
}

static int set_static_route_simple_outgoing_if(struct route_list_element **routes_head, char *node_value)
{
	int ifindex = 0;

	(*routes_head)->route.next_hop.kind = route_next_hop_kind_simple;
	(*routes_head)->route.next_hop.value.simple.if_name = xstrdup(node_value);
	ifindex = (int) if_nametoindex(node_value);
	if (ifindex == 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "failed to get ifindex for %s", node_value);
		return -1;
	}

	(*routes_head)->route.next_hop.value.simple.ifindex = ifindex;
	return 0;
}

static int delete_control_plane_protocol_value(struct route_list_hash_element **ipv4_routes_head, struct route_list_hash_element **ipv6_routes_head, char *node_xpath)
{
	char *node_name = NULL;
	char *orig_xpath = NULL;

	int error = SR_ERR_OK;

	orig_xpath = xstrdup(node_xpath);

	node_name = sr_xpath_node_name(node_xpath);

	if (strstr(orig_xpath, "ietf-ipv4-unicast-routing:ipv4")) {
		error = delete_static_route_value(ipv4_routes_head, orig_xpath, node_name, AF_INET);
		if (error != 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "error setting IPv4 static route value");
			goto out;
		}
	} else if (strstr(orig_xpath, "ietf-ipv6-unicast-routing:ipv6")) {
		error = delete_static_route_value(ipv6_routes_head, orig_xpath, node_name, AF_INET6);
		if (error != 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "error setting IPv6 static route value");
			goto out;
		}
	}

out:
	if (orig_xpath) {
		FREE_SAFE(orig_xpath);
	}

	return error ? SR_ERR_CALLBACK_FAILED : SR_ERR_OK;
}

static int delete_static_route_value(struct route_list_hash_element **routes_hash_head, char *xpath, char *node_name, int family)
{
	sr_xpath_ctx_t xpath_ctx = {0};

	struct nl_addr *destination_prefix_addr = NULL;
	struct route_list_element **routes_head = NULL;
	char *next_hop_list = NULL;
	char *destination_prefix = NULL;
	int error = 0;

	next_hop_list = sr_xpath_key_value(xpath, "next-hop", "index", &xpath_ctx);
	destination_prefix = sr_xpath_key_value(xpath, "route", "destination-prefix", &xpath_ctx);

	if (destination_prefix == NULL) {
		error = -1;
		SRPLG_LOG_ERR(PLUGIN_NAME, "destination-prefix couldn't be retrieved");
		goto out;
	}

	error = nl_addr_parse(destination_prefix, family, &destination_prefix_addr);
	if (error != 0) {
		error = -1;
		SRPLG_LOG_ERR(PLUGIN_NAME, "failed to parse destination-prefix into nl_addr");
		goto out;
	}

	routes_head = route_list_hash_get(routes_hash_head, destination_prefix_addr);

	if (routes_head == NULL) {
		error = -1;
		SRPLG_LOG_ERR(PLUGIN_NAME, "matching route_list destination-prefix %s not found", destination_prefix);
		goto out;
	}

	if (next_hop_list != NULL) {
		(*routes_head)->route.next_hop.kind = route_next_hop_kind_list;

		if (!strcmp(node_name, "next-hop-address")) {
		} else if (!strcmp(node_name, "outgoing-interface")) {
		}
	} else if (!strcmp(node_name, "destination-prefix")) {
		// delete item
		LL_DELETE(*routes_head, *routes_head);
	} else if (!strcmp(node_name, "description")) {
		set_static_route_description(routes_head, NULL);
	} else if (!strcmp(node_name, "next-hop-address")) {
		(*routes_head)->route.next_hop.value.simple.addr = NULL;
	} else if (!strcmp(node_name, "outgoing-interface")) {
		(*routes_head)->route.next_hop.value.simple.if_name = NULL;
	}

out:
	if (destination_prefix_addr) {
		nl_addr_put(destination_prefix_addr);
	}

	return error ? SR_ERR_CALLBACK_FAILED : SR_ERR_OK;
}

static int update_static_routes(struct route_list_hash_element **routes_head, uint8_t family)
{
	struct nl_sock *socket = NULL;
	struct rtnl_route *route = NULL;
	struct rtnl_nexthop *next_hop = NULL;
	struct nl_addr *dst_addr = NULL;
	int error = 0;
	int nl_err = 0;
	struct route_list_hash_element *routes_iter = NULL;

	socket = nl_socket_alloc();
	if (socket == NULL) {
		error = -1;
		SRPLG_LOG_ERR(PLUGIN_NAME, "unable to init nl_sock struct...");
		goto error_out;
	}

	nl_err = nl_connect(socket, NETLINK_ROUTE);
	if (nl_err != 0) {
		error = -1;
		SRPLG_LOG_ERR(PLUGIN_NAME, "nl_connect failed (%d): %s", nl_err, nl_geterror(nl_err));
		goto error_out;
	}

	LL_FOREACH(*routes_head, routes_iter)
	{
		if (route_list_is_empty(&routes_iter->routes_head)) {
			continue;
		}

		route = rtnl_route_alloc();
		if (route == NULL) {
			error = -1;
			SRPLG_LOG_ERR(PLUGIN_NAME, "unable to alloc rtnl_route struct");
			goto error_out;
		}

		rtnl_route_set_table(route, RT_TABLE_MAIN);
		rtnl_route_set_protocol(route, RTPROT_STATIC);
		rtnl_route_set_family(route, family);
		dst_addr = nl_addr_clone(routes_iter->prefix);
		rtnl_route_set_dst(route, dst_addr);
		rtnl_route_set_priority(route, routes_iter->routes_head->route.preference);

		if (routes_iter->routes_head->route.next_hop.kind == route_next_hop_kind_simple) {
			next_hop = rtnl_route_nh_alloc();
			if (next_hop == NULL) {
				error = -1;
				SRPLG_LOG_ERR(PLUGIN_NAME, "unable to alloc rtnl_nexthop struct");
				goto error_out;
			}

			if (routes_iter->routes_head->route.next_hop.value.simple.if_name == NULL && routes_iter->routes_head->route.next_hop.value.simple.addr == NULL) {
				error = -1;
				SRPLG_LOG_ERR(PLUGIN_NAME, "outgoing-interface and next-hop-address can't both be NULL");
				goto error_out;
			}

			if (routes_iter->routes_head->route.next_hop.value.simple.if_name != NULL) {
				rtnl_route_nh_set_ifindex(next_hop, routes_iter->routes_head->route.next_hop.value.simple.ifindex);
			}

			if (routes_iter->routes_head->route.next_hop.value.simple.addr != NULL) {
				rtnl_route_nh_set_gateway(next_hop, routes_iter->routes_head->route.next_hop.value.simple.addr);
			}
			rtnl_route_add_nexthop(route, next_hop);
		} else if (routes_iter->routes_head->route.next_hop.kind == route_next_hop_kind_list) {
			struct route_next_hop_list_element *nexthop_iter = NULL;

			LL_FOREACH(routes_iter->routes_head->route.next_hop.value.list_head, nexthop_iter)
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

		nl_err = rtnl_route_add(socket, route, NLM_F_REPLACE);
		if (nl_err != 0) {
			error = -1;
			SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_route_add failed (%d): %s", nl_err, nl_geterror(nl_err));
			goto error_out;
		}

		nl_addr_put(dst_addr);
		dst_addr = NULL;
		rtnl_route_put(route);
		route = NULL;
	}

error_out:
	if (dst_addr) {
		nl_addr_put(dst_addr);
	}

	if (route) {
		rtnl_route_put(route);
	}

	if (socket) {
		nl_socket_free(socket);
	}

	return error;
}

static int update_new_static_rotues(struct route_list_hash_element **routes_hash_head, uint8_t family)
{
	int error = 0;
	int nl_err = 0;

	// libnl
	struct nl_sock *socket = NULL;
	struct rtnl_route *route = NULL;
	struct rtnl_nexthop *next_hop = NULL;
	struct nl_addr *dst_addr = NULL;

	// plugin
	struct route_list_hash_element *routes_iter = NULL;
	struct route_list_element *route_iter = NULL;

	socket = nl_socket_alloc();
	if (socket == NULL) {
		error = -1;
		SRPLG_LOG_ERR(PLUGIN_NAME, "unable to init nl_sock struct...");
		goto error_out;
	}

	nl_err = nl_connect(socket, NETLINK_ROUTE);
	if (nl_err != 0) {
		error = -1;
		SRPLG_LOG_ERR(PLUGIN_NAME, "nl_connect failed (%d): %s", nl_err, nl_geterror(nl_err));
		goto error_out;
	}

	LL_FOREACH(*routes_hash_head, routes_iter)
	{
		LL_FOREACH(routes_iter->routes_head, route_iter)
		{
			route = rtnl_route_alloc();
			if (route == NULL) {
				error = -1;
				SRPLG_LOG_ERR(PLUGIN_NAME, "unable to alloc rtnl_route struct");
				goto error_out;
			}

			dst_addr = nl_addr_clone(routes_iter->prefix);

			rtnl_route_set_table(route, RT_TABLE_MAIN);
			rtnl_route_set_protocol(route, RTPROT_STATIC);
			rtnl_route_set_family(route, family);
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

			nl_err = rtnl_route_add(socket, route, NLM_F_REPLACE);
			if (nl_err != 0) {
				error = -1;
				SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_route_add() failed (%d): %s", nl_err, nl_geterror(nl_err));
				goto error_out;
			}

			nl_addr_put(dst_addr);
			rtnl_route_put(route);
		}
	}

error_out:
	if (dst_addr) {
		nl_addr_put(dst_addr);
	}

	if (route) {
		rtnl_route_put(route);
	}

	if (socket) {
		nl_socket_free(socket);
	}

	return error;
}

static int update_delete_static_rotues(struct route_list_hash_element **routes_hash_head, uint8_t family)
{
	int error = 0;
	int nl_err = 0;

	// libnl
	struct nl_sock *socket = NULL;
	struct rtnl_route *route = NULL;
	struct nl_addr *dst_addr = NULL;

	// plugin
	struct route_list_hash_element *routes_iter = NULL;
	struct route_list_element *route_iter = NULL;

	socket = nl_socket_alloc();
	if (socket == NULL) {
		error = -1;
		SRPLG_LOG_ERR(PLUGIN_NAME, "unable to init nl_sock struct...");
		goto error_out;
	}

	nl_err = nl_connect(socket, NETLINK_ROUTE);
	if (nl_err != 0) {
		error = -1;
		SRPLG_LOG_ERR(PLUGIN_NAME, "nl_connect failed (%d): %s", nl_err, nl_geterror(nl_err));
		goto error_out;
	}

	LL_FOREACH(*routes_hash_head, routes_iter)
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
			rtnl_route_set_family(route, family);
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
		}
	}

error_out:
	if (dst_addr) {
		nl_addr_put(dst_addr);
	}

	if (route) {
		rtnl_route_put(route);
	}

	if (socket) {
		nl_socket_free(socket);
	}

	return error;
}
