#include "operational.h"
#include "libyang/log.h"
#include "ly_tree.h"
#include "netlink/cache.h"
#include "netlink/route/link/bridge.h"
#include <bridging/common.h>

#include <limits.h>

#include <sysrepo.h>

#include <libyang/libyang.h>

#include <netlink/addr.h>
#include <netlink/route/link.h>

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
	if (netlink_error != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nl_connect() failed (%d): %s", netlink_error, nl_geterror(netlink_error));
		goto error_out;
	}

	netlink_error = rtnl_link_alloc_cache(socket, AF_UNSPEC, &link_cache);
	if (netlink_error != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_alloc_cache() failed (%d): %s", netlink_error, nl_geterror(netlink_error));
		goto error_out;
	}

	link_iter = (struct rtnl_link *) nl_cache_get_first(link_cache);
	while (link_iter != 0) {
		if (rtnl_link_is_bridge(link_iter)) {
			error = bridging_ly_tree_add_bridge(ly_ctx, *parent, link_iter, &bridge_node);
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "bridging_ly_tree_add_bridge() failed (%d)", error);
				goto error_out;
			}

			error = bridging_ly_tree_add_bridge_ports(ly_ctx, bridge_node, link_iter, link_cache);
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "bridging_ly_tree_add_bridge_ports() failed (%d)", error);
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