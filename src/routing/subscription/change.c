#include "change.h"
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

static int set_control_plane_protocol_value(struct routing_ctx *ctx, char *xpath, char *value);
static int set_static_route_value(char *xpath, char *node_name, char *node_value, struct route_list_hash_element **routes_hash_head, int family);
static void set_static_route_description(struct route_list_element **routes_head, char *node_value);
static int set_static_route_simple_next_hop(struct route_list_element **routes_head, char *node_value, int family);
static int set_static_route_simple_outgoing_if(struct route_list_element **routes_head, char *node_value);
static int delete_control_plane_protocol_value(struct routing_ctx *ctx, char *xpath);
static int delete_static_route_value(struct routing_ctx *ctx, char *xpath, char *node_name, int family);
static int update_static_routes(struct route_list_hash_element **routes_head, uint8_t family);

int routing_module_change_cb(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data)
{
	int error = 0;

	// context
	struct routing_ctx *ctx = (struct routing_ctx *) private_data;

	// sysrepo
	sr_session_ctx_t *startup_session = ctx->startup_session;
	sr_change_iter_t *routing_change_iter = NULL;
	sr_change_oper_t operation = SR_OP_CREATED;

	// libyang
	const struct lyd_node *node = NULL;

	// temp helper vars
	char path[512] = {0};
	const char *prev_value = NULL;
	const char *prev_list = NULL;
	int prev_default = false;
	char *node_xpath = NULL;
	const char *node_value = NULL;

	bool ipv4_update = false;
	bool ipv6_update = false;

	SRPLG_LOG_INF(PLUGIN_NAME, "module_name: %s, xpath: %s, event: %d, request_id: %u", module_name, xpath, event, request_id);

	if (event == SR_EV_ABORT) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "aborting changes for: %s", xpath);
		error = -1;
		goto error_out;
	} else if (event == SR_EV_DONE) {
		error = sr_copy_config(startup_session, BASE_YANG_MODEL, SR_DS_RUNNING, 0);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "sr_copy_config error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}
	} else if (event == SR_EV_CHANGE) {
		sprintf(path, "%s//.", xpath);

		error = sr_get_changes_iter(session, path, &routing_change_iter);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "sr_get_changes_iter error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}

		while (sr_get_change_tree_next(session, routing_change_iter, &operation, &node, &prev_value, &prev_list, &prev_default) == SR_ERR_OK) {
			node_xpath = lyd_path(node, LYD_PATH_STD, NULL, 0);

			if (node->schema->nodetype == LYS_LEAF || node->schema->nodetype == LYS_LEAFLIST) {
				node_value = xstrdup(lyd_get_value(node));
			}

			SRPLG_LOG_DBG(PLUGIN_NAME, "node_xpath: %s; prev_val: %s; node_val: %s; operation: %d", node_xpath, prev_value, node_value, operation);

			if (node->schema->nodetype == LYS_LEAF || node->schema->nodetype == LYS_LEAFLIST) {
				if (strstr(node_xpath, "ietf-ipv4-unicast-routing:ipv4")) {
					ipv4_update = true;
				} else if (strstr(node_xpath, "ietf-ipv6-unicast-routing:ipv6")) {
					ipv6_update = true;
				}

				if (operation == SR_OP_CREATED || operation == SR_OP_MODIFIED) {
					if (strstr(node_xpath, "ietf-routing:routing/control-plane-protocols")) {
						error = set_control_plane_protocol_value(ctx, node_xpath, (char *) node_value);
						if (error) {
							SRPLG_LOG_ERR(PLUGIN_NAME, "set_control_plane_protocol_value error (%d)", error);
							goto error_out;
						}
					}
				} else if (operation == SR_OP_DELETED) {
					if (strstr(node_xpath, "ietf-routing:routing/control-plane-protocols")) {
						error = delete_control_plane_protocol_value(ctx, node_xpath);
						if (error) {
							SRPLG_LOG_ERR(PLUGIN_NAME, "set_control_plane_protocol_value error (%d)", error);
							goto error_out;
						}
					}
				}
			}
			FREE_SAFE(node_xpath);
			node_value = NULL;
		}

		if (ipv4_update) {
			error = update_static_routes(&ctx->ipv4_static_routes_head, AF_INET);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "failed to update IPv4 static routes on system");
				goto error_out;
			}
			// route_list_hash_prune(ipv4_static_routes);
		}

		if (ipv6_update) {
			error = update_static_routes(&ctx->ipv6_static_routes_head, AF_INET6);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "failed to update IPv6 static routes on system");
				goto error_out;
			}
		}
	}
	goto out;

error_out:
	SRPLG_LOG_ERR(PLUGIN_NAME, "error applying control plane protocols module changes");
	error = -1;
	FREE_SAFE(node_xpath);
	node_value = NULL;

out:
	sr_free_change_iter(routing_change_iter);

	return error != 0 ? SR_ERR_CALLBACK_FAILED : SR_ERR_OK;
}

static int set_control_plane_protocol_value(struct routing_ctx *ctx, char *node_xpath, char *node_value)
{
	char *node_name = NULL;
	char *orig_xpath = NULL;

	int error = SR_ERR_OK;

	orig_xpath = xstrdup(node_xpath);

	node_name = sr_xpath_node_name(node_xpath);

	if (strstr(orig_xpath, "ietf-ipv4-unicast-routing:ipv4")) {
		error = set_static_route_value(orig_xpath, node_name, node_value, &ctx->ipv4_static_routes_head, AF_INET);
		if (error != 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "error setting IPv4 static route value");
			goto out;
		}
	} else if (strstr(orig_xpath, "ietf-ipv6-unicast-routing:ipv6")) {
		error = set_static_route_value(orig_xpath, node_name, node_value, &ctx->ipv6_static_routes_head, AF_INET6);
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

	if (strcmp(node_name, "destination-prefix")) {
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

static int delete_control_plane_protocol_value(struct routing_ctx *ctx, char *node_xpath)
{
	char *node_name = NULL;
	char *orig_xpath = NULL;

	int error = SR_ERR_OK;

	orig_xpath = xstrdup(node_xpath);

	node_name = sr_xpath_node_name(node_xpath);

	if (strstr(orig_xpath, "ietf-ipv4-unicast-routing:ipv4")) {
		error = delete_static_route_value(ctx, orig_xpath, node_name, AF_INET);
		if (error != 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "error setting IPv4 static route value");
			goto out;
		}
	} else if (strstr(orig_xpath, "ietf-ipv6-unicast-routing:ipv6")) {
		error = delete_static_route_value(ctx, orig_xpath, node_name, AF_INET6);
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

static int delete_static_route_value(struct routing_ctx *ctx, char *xpath, char *node_name, int family)
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

	if (family == AF_INET) {
		routes_head = route_list_hash_get(&ctx->ipv4_static_routes_head, destination_prefix_addr);
	} else if (family == AF_INET6) {
		routes_head = route_list_hash_get(&ctx->ipv6_static_routes_head, destination_prefix_addr);
	}

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