#include "startup.h"
#include "common.h"
#include "libyang/log.h"
#include "libyang/out.h"
#include "libyang/printer_data.h"
#include "libyang/tree_data.h"
#include "netlink/addr.h"
#include "netlink/object.h"
#include "sysrepo_types.h"

// sysrepo
#include <sysrepo.h>

#include <netlink/cache.h>
#include <netlink/socket.h>
#include <netlink/route/link.h>
#include <netlink/route/link/bridge.h>

// helpers - split loading into parts
static int bridging_startup_load_bridges(sr_session_ctx_t *session, struct lyd_node *bridges_container);

static int bridging_startup_add_bridge(const struct ly_ctx *ly_ctx, struct lyd_node *bridges_container, struct rtnl_link *bridge_link, struct lyd_node **bridge_node);
static int bridging_startup_add_bridge_address(const struct ly_ctx *ly_ctx, struct lyd_node *bridge_node, struct rtnl_link *bridge_link);
static int bridging_startup_add_bridge_type(const struct ly_ctx *ly_ctx, struct lyd_node *bridge_node, struct rtnl_link *bridge_link);

int bridging_startup_load_data(bridging_ctx_t *ctx, sr_session_ctx_t *session)
{
	int error = 0;
	LY_ERR libyang_error = LY_SUCCESS;
	const struct ly_ctx *ly_context = NULL;
	struct lyd_node *bridges_container_node = NULL;
	sr_conn_ctx_t *conn_ctx = NULL;

	conn_ctx = sr_session_get_connection(session);
	ly_context = sr_acquire_context(conn_ctx);
	if (ly_context == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "Unable to get ly_ctx variable");
		goto error_out;
	}

	libyang_error = lyd_new_path(NULL, ly_context, BRIDGING_BRIDGES_CONTAINER_YANG_PATH, NULL, 0, &bridges_container_node);
	if (libyang_error != LY_SUCCESS) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "Unable to create the bridges container libyang node");
		goto error_out;
	}

	error = bridging_startup_load_bridges(session, bridges_container_node);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "bridging_startup_load_bridges() failed (%d)", error);
		goto error_out;
	}

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

	netlink_error = rtnl_link_alloc_cache(socket, AF_BRIDGE, &link_cache);
	if (netlink_error != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_alloc_cache() failed (%d): %s", netlink_error, nl_geterror(netlink_error));
		goto error_out;
	}

	link_iter = (struct rtnl_link *) nl_cache_get_first(link_cache);
	while (link_iter != 0) {
		// create new link
		error = bridging_startup_add_bridge(ly_ctx, bridges_container, link_iter, &bridge_node);
		if (error != 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "bridging_startup_add_bridge() failed (%d)", error);
			goto error_out;
		}

		// mac address
		error = bridging_startup_add_bridge_address(ly_ctx, bridge_node, link_iter);
		if (error != 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "bridging_startup_add_bridge_address() failed (%d)", error);
			goto error_out;
		}

		// type
		error = bridging_startup_add_bridge_type(ly_ctx, bridge_node, link_iter);
		if (error != 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "bridging_startup_add_bridge_type() failed (%d)", error);
			goto error_out;
		}

		link_iter = (struct rtnl_link *) nl_cache_get_next((struct nl_object *) link_iter);
	}

	ly_error = ly_out_new_file(stdout, &ly_output);
	if (ly_error != LY_SUCCESS) {
		SRPLG_LOG_DBG(PLUGIN_NAME, "ly_out_new_file() failed: %d");
		goto error_out;
	}

	ly_error = lyd_print_tree(ly_output, bridges_container, LYD_JSON, 0);
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

static int bridging_startup_add_bridge(const struct ly_ctx *ly_ctx, struct lyd_node *bridges_container, struct rtnl_link *bridge_link, struct lyd_node **bridge_node)
{
	LY_ERR ly_error = LY_SUCCESS;

	char path_buffer[PATH_MAX] = {0};

	SRPLG_LOG_DBG(PLUGIN_NAME, "Bridge name = %s", rtnl_link_get_name(bridge_link));
	snprintf(path_buffer, sizeof(path_buffer), "bridge[name=\"%s\"]", rtnl_link_get_name(bridge_link));
	ly_error = lyd_new_path(bridges_container, ly_ctx, path_buffer, rtnl_link_get_name(bridge_link), 0, bridge_node);
	if (ly_error != LY_SUCCESS) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "lyd_new_path() for \"bridge\" failed (%d): %s", ly_error, ly_errmsg(ly_ctx));
		return -1;
	}

	return 0;
}

static int bridging_startup_add_bridge_address(const struct ly_ctx *ly_ctx, struct lyd_node *bridge_node, struct rtnl_link *bridge_link)
{
	LY_ERR ly_error = LY_SUCCESS;

	char mac_addr_buffer[100] = {0};
	char *tmp_ptr = mac_addr_buffer;

	// get mac address and convert libnl format to libyang format
	nl_addr2str(rtnl_link_get_addr(bridge_link), mac_addr_buffer, sizeof(mac_addr_buffer));
	tmp_ptr = mac_addr_buffer;
	while ((tmp_ptr = strchr(tmp_ptr, ':'))) {
		*tmp_ptr = '-';
	}

	// add node
	ly_error = lyd_new_path(bridge_node, ly_ctx, "address", mac_addr_buffer, 0, NULL);
	if (ly_error != LY_SUCCESS) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "lyd_new_path() for \"address\" failed (%d): %s", ly_error, ly_errmsg(ly_ctx));
		return -1;
	}

	return 0;
}

static int bridging_startup_add_bridge_type(const struct ly_ctx *ly_ctx, struct lyd_node *bridge_node, struct rtnl_link *bridge_link)
{
	int error = 0;
	LY_ERR ly_error = LY_SUCCESS;

	ly_error = lyd_new_path(bridge_node, ly_ctx, "bridge-type", "customer-vlan-bridge", 0, NULL);
	if (ly_error != LY_SUCCESS) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "lyd_new_path() for \"type\" failed (%d): %s", ly_error, ly_errmsg(ly_ctx));
		return -1;
	}

	return error;
}