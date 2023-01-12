#include "startup.h"
#include "netlink/route/link/vlan.h"
#include <bridging/common.h>
#include <bridging/ly_tree.h>
#include <bridge_netlink.h>

// libyang
#include <libyang/log.h>
#include <libyang/out.h>
#include <libyang/printer_data.h>
#include <libyang/tree_data.h>

// sysrepo
#include <sysrepo.h>

// libnl
#include <netlink/cache.h>
#include <netlink/socket.h>
#include <netlink/route/link.h>
#include <netlink/route/link/bridge.h>
#include <netlink/addr.h>
#include <netlink/object.h>
#include <sysrepo_types.h>

// helpers - split loading into parts
static int bridging_startup_load_bridges(sr_session_ctx_t *session, struct lyd_node *bridges_container);
static int bridging_startup_load_bridge_component_list(sr_session_ctx_t *session, struct lyd_node *bridge_node, struct rtnl_link *bridge_link, struct nl_cache *link_cache);

int bridging_startup_load_data(bridging_ctx_t *ctx, sr_session_ctx_t *session)
{
	int error = 0;
	const struct ly_ctx *ly_ctx = NULL;
	struct lyd_node *bridges_container_node = NULL;
	sr_conn_ctx_t *conn_ctx = NULL;

	conn_ctx = sr_session_get_connection(session);
	ly_ctx = sr_acquire_context(conn_ctx);
	if (ly_ctx == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "Unable to get ly_ctx variable");
		goto error_out;
	}

	error = bridging_ly_tree_add_bridges_container(ly_ctx, NULL, &bridges_container_node);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "Unable to create bridges container node");
		goto error_out;
	}

	error = bridging_startup_load_bridges(session, bridges_container_node);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "bridging_startup_load_bridges() failed (%d)", error);
		goto error_out;
	}

#ifdef APPLY_DATASTORE_CHANGES

	error = sr_edit_batch(session, bridges_container_node, "merge");
	if (error != SR_ERR_OK) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_edit_batch() error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	error = sr_apply_changes(session, 0);
	if (error != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_apply_changes() error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

#endif

	goto out;

error_out:
	error = -1;
	SRPLG_LOG_ERR(PLUGIN_NAME, "Unable to load initial data");

out:
	if (bridges_container_node) {
		lyd_free_tree(bridges_container_node);
	}
	sr_release_context(conn_ctx);
	return error;
}

static int bridging_startup_load_bridges(sr_session_ctx_t *session, struct lyd_node *bridges_container)
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
	struct lyd_node *bridge_node = NULL;
	struct ly_out *ly_output = NULL;
	const struct ly_ctx *ly_ctx = NULL;
	LY_ERR ly_error = LY_SUCCESS;

	conn_ctx = sr_session_get_connection(session);
	ly_ctx = sr_acquire_context(conn_ctx);

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
			// create new link
			error = bridging_ly_tree_add_bridge(ly_ctx, bridges_container, link_iter, &bridge_node);
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "bridging_ly_tree_add_bridge() failed (%d)", error);
				goto error_out;
			}

			// mac address
			error = bridging_ly_tree_add_bridge_address(ly_ctx, bridge_node, link_iter);
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "bridging_ly_tree_add_bridge_address() failed (%d)", error);
				goto error_out;
			}

			bridge_vlan_info_t vlan_info = {0};
			error = bridge_get_vlan_info(socket, link_iter, &vlan_info);
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "bridge_get_vlan_info() failed (%d)", error);
				goto error_out;
			}

			// type
			error = bridging_ly_tree_add_bridge_type(ly_ctx, bridge_node, link_iter);
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "bridging_ly_tree_add_bridge_type() failed (%d)", error);
				goto error_out;
			}

			// component
			error = bridging_startup_load_bridge_component_list(session, bridge_node, link_iter, link_cache);
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "bridging_startup_load_bridge_component_list() failed (%d)", error);
				goto error_out;
			}
		}

		link_iter = (struct rtnl_link *) nl_cache_get_next((struct nl_object *) link_iter);
	}

	ly_error = ly_out_new_file(stdout, &ly_output);
	if (ly_error != LY_SUCCESS) {
		SRPLG_LOG_DBG(PLUGIN_NAME, "ly_out_new_file() failed: %d");
		goto error_out;
	}

	ly_error = lyd_print_tree(ly_output, bridges_container, LYD_XML, 0);
	if (ly_error != LY_SUCCESS) {
		SRPLG_LOG_DBG(PLUGIN_NAME, "lyd_print_tree() failed: %d");
		goto error_out;
	}

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

static int bridging_startup_load_bridge_component_list(sr_session_ctx_t *session, struct lyd_node *bridge_node, struct rtnl_link *bridge_link, struct nl_cache *link_cache)
{
	int error = 0;

	sr_conn_ctx_t *conn_ctx = NULL;

	// libyang
	struct ly_out *ly_output = NULL;
	struct lyd_node *component_node = NULL;
	const struct ly_ctx *ly_ctx = NULL;
	// LY_ERR ly_error = LY_SUCCESS;

	// libnl
	struct rtnl_link *link_iter = NULL;

	conn_ctx = sr_session_get_connection(session);
	ly_ctx = sr_acquire_context(conn_ctx);

	link_iter = (struct rtnl_link *) nl_cache_get_first(link_cache);

	while (link_iter != NULL) {
		if (rtnl_link_get_master(link_iter) == rtnl_link_get_ifindex(bridge_link) && rtnl_link_is_vlan(link_iter)) {
			struct rtnl_link *component_link = link_iter;

			// found VLAN interface associated with the bridge -> add a new component
			error = bridging_ly_tree_add_bridge_component(ly_ctx, bridge_node, component_link, &component_node);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "bridging_ly_tree_add_bridge_component() failed (%d)", error);
				goto error_out;
			}

			// id
			error = bridging_ly_tree_add_bridge_component_id(ly_ctx, component_node, component_link);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "bridging_ly_tree_add_bridge_component_id() failed (%d)", error);
				goto error_out;
			}

			// type
			error = bridging_ly_tree_add_bridge_component_type(ly_ctx, component_node, component_link);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "bridging_ly_tree_add_bridge_component_type() failed (%d)", error);
				goto error_out;
			}

			// address
			error = bridging_ly_tree_add_bridge_component_address(ly_ctx, component_node, component_link);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "bridging_ly_tree_add_bridge_component_address() failed (%d)", error);
				goto error_out;
			}
		}
		link_iter = (struct rtnl_link *) nl_cache_get_next((struct nl_object *) link_iter);
	}

	goto out;

error_out:
	error = -1;

out:
	if (ly_output) {
		ly_out_free(ly_output, NULL, 1);
	}
	sr_release_context(conn_ctx);

	return error;
}