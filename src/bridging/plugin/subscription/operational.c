#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <linux/if_bridge.h>

#include <sysrepo.h>
#include <sysrepo/xpath.h>

#include <libyang/libyang.h>

#include <netlink/addr.h>
#include <netlink/route/link.h>
#include <netlink/cache.h>
#include <netlink/route/link/bridge.h>

#include "operational.h"
#include "plugin/api/bridge_netlink.h"
#include "plugin/ly_tree.h"
#include "plugin/common.h"

#include "memory.h"

int bridging_oper_get_bridges(sr_session_ctx_t *session, uint32_t sub_id, const char *module_name, const char *path, const char *request_xpath, uint32_t request_id, struct lyd_node **parent, void *private_data)
{
	int error = 0;

	// sysrepo
	sr_conn_ctx_t *conn_ctx = NULL;

	// libnl
	int netlink_error = 0;
	struct nl_cache *link_cache = NULL;
	struct nl_sock *socket = NULL;
	struct rtnl_link *link_iter = NULL;

	// libyang
	// LY_ERR ly_error = LY_SUCCESS;
	struct lyd_node *bridge_node = NULL;
	struct ly_out *ly_output = NULL;
	const struct ly_ctx *ly_ctx = NULL;

	conn_ctx = sr_session_get_connection(session);
	ly_ctx = sr_acquire_context(conn_ctx);

	// connect to libnl and cache bridges
	socket = nl_socket_alloc();
	if (socket == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "Unable to init nl_sock struct...");
		goto error_out;
	}

	netlink_error = nl_connect(socket, NETLINK_ROUTE);
	if (netlink_error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nl_connect() failed (%d): %s", netlink_error, nl_geterror(netlink_error));
		goto error_out;
	}

	netlink_error = rtnl_link_alloc_cache(socket, AF_UNSPEC, &link_cache);
	if (netlink_error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_alloc_cache() failed (%d): %s", netlink_error, nl_geterror(netlink_error));
		goto error_out;
	}

	link_iter = (struct rtnl_link *) nl_cache_get_first(link_cache);
	while (link_iter != 0) {
		if (rtnl_link_is_bridge(link_iter)) {
			error = bridging_ly_tree_add_bridge(ly_ctx, *parent, link_iter, &bridge_node);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "bridging_ly_tree_add_bridge() failed (%d)", error);
				goto error_out;
			}

			error = bridging_ly_tree_add_bridge_ports(ly_ctx, bridge_node, link_iter, link_cache);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "bridging_ly_tree_add_bridge_ports() failed (%d)", error);
				goto error_out;
			}

			error = bridging_ly_tree_add_bridge_components(ly_ctx, bridge_node, link_iter, link_cache);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "bridging_ly_tree_add_bridge_components() failed (%d)", error);
				goto error_out;
			}
		}
		link_iter = (struct rtnl_link *) nl_cache_get_next((struct nl_object *) link_iter);
	}

	// ly_error = ly_out_new_file(stdout, &ly_output);
	// if (ly_error != LY_SUCCESS) {
	// 	SRPLG_LOG_DBG(PLUGIN_NAME, "ly_out_new_file() failed: %d");
	// 	goto error_out;
	// }

	// ly_error = lyd_print_tree(ly_output, *parent, LYD_XML, 0);
	// if (ly_error != LY_SUCCESS) {
	// 	SRPLG_LOG_DBG(PLUGIN_NAME, "lyd_print_tree() failed: %d");
	// 	goto error_out;
	// }

	goto out;

error_out:
	error = -1;

out:
	nl_cache_free(link_cache);
	nl_socket_free(socket);
	if (ly_output) {
		ly_out_free(ly_output, NULL, 1);
	}
	sr_release_context(conn_ctx);

	return error;
}


int bridging_oper_get_bridge_vlan(sr_session_ctx_t *session, uint32_t sub_id, const char *module_name, const char *path, const char *request_xpath, uint32_t request_id, struct lyd_node **parent, void *private_data)
{
	SRPLG_LOG_DBG(PLUGIN_NAME, "bridging_oper_get_bridge_vlan (operational callback) called for path: %s", request_xpath);

	int error = 0;
	LY_ERR ly_err = 0;

	struct bridge_vlan_info *port_vlan_list = NULL;
	const char *port_type = NULL;
	size_t port_vlan_count = 0;
	bool untagged = false;
	char parent_path[PATH_MAX] = {0};
	char vlan_port_path[PATH_MAX] = {0};
	const char *br_component_name = NULL;
	// sysrepo
	sr_conn_ctx_t *conn_ctx = NULL;
	sr_xpath_ctx_t xpath_ctx = {0};
	// libnl
	struct nl_cache *link_cache = NULL;
	struct nl_sock *socket = NULL;
	struct rtnl_link *bridge = NULL;
	struct rtnl_link *link_iter = NULL;
	// libyang
	struct ly_out *ly_output = NULL;
	const struct ly_ctx *ly_ctx = NULL;

	error = (lyd_path(*parent, LYD_PATH_STD, parent_path, PATH_MAX) == NULL);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "lyd_path() error: %d", error);
		goto error_out;
	}
	SRPLG_LOG_DBG(PLUGIN_NAME, "parent node path for 'bridge-vlan' container: %s\n", parent_path);

	br_component_name = sr_xpath_key_value(parent_path, "component", "name", &xpath_ctx);
	if (br_component_name == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_xpath_key_value() error: %d", error);
		goto error_out;

	}

	conn_ctx = sr_session_get_connection(session);
	ly_ctx = sr_acquire_context(conn_ctx);

	// connect to libnl and cache bridges
	socket = nl_socket_alloc();
	if (socket == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "Unable to init nl_sock struct...");
		goto error_out;
	}
	error = nl_connect(socket, NETLINK_ROUTE);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nl_connect() failed (%d): %s", error, nl_geterror(error));
		goto error_out;
	}
	error = rtnl_link_alloc_cache(socket, AF_UNSPEC, &link_cache);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_alloc_cache() failed (%d): %s", error, nl_geterror(error));
		goto error_out;
	}

	bridge = rtnl_link_get_by_name(link_cache, br_component_name);
	if (bridge == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_get_by_name() failed (bridge not found)");
		goto error_out;
	}

	// set value for vlan version, max-vids & override-default-pvid nodes
	error = bridging_ly_tree_add_general_bridge_vlan_info(ly_ctx, *parent);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "bridging_ly_tree_add_general_bridge_vlan_info() failed");
		goto error_out;
	}

	// find and set untagged and egress VLAN ports on this bridge component
	link_iter = (struct rtnl_link *) nl_cache_get_first(link_cache);
	while (link_iter != 0) {
		if (rtnl_link_get_master(link_iter) == rtnl_link_get_ifindex(bridge)) {
			// find vlans on this bridge port
			error = bridge_get_vlan_list(socket, link_iter, &port_vlan_list, &port_vlan_count);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "bridge_get_vlan_list() failed (%d)", error);
				goto loop_cleanup;
			}
			for (size_t i = 0; i < port_vlan_count; i++) {
				untagged = port_vlan_list[i].flags & BRIDGE_VLAN_INFO_UNTAGGED;
				port_type = untagged ? "untagged" : "egress";
				SRPC_SAFE_CALL_ERR_COND(error, error < 0, snprintf(vlan_port_path, PATH_MAX, "bridge-vlan/vlan[vid='%d']/%s-ports", port_vlan_list[i].vid, port_type), error_out);
				error = 0;

				ly_err = lyd_new_path(*parent, ly_ctx, vlan_port_path, rtnl_link_get_name(link_iter), 0, NULL);
				if (ly_err != LY_SUCCESS) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "lyd_new_path() failed (%d): %s", ly_err, ly_errmsg(ly_ctx));
					goto loop_cleanup;
				}
			}
		}
		link_iter = (struct rtnl_link *) nl_cache_get_next((struct nl_object *) link_iter);
loop_cleanup:
		if (port_vlan_list != NULL) {
			FREE_SAFE(port_vlan_list);
		}
		if (error) {
			goto error_out;
		}
	}

	goto out;

error_out:
	error = SR_ERR_CALLBACK_FAILED;
out:
	nl_cache_free(link_cache);
	if (bridge) {
		rtnl_link_put(bridge);
	}
	nl_socket_free(socket);
	if (ly_output) {
		ly_out_free(ly_output, NULL, 1);
	}
	sr_release_context(conn_ctx);

	return error;
}
