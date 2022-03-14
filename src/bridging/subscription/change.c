#include "change.h"
#include "netlink/addr.h"
#include "netlink/cache.h"
#include "netlink/errno.h"
#include "netlink/route/link/bridge.h"
#include "sysrepo_types.h"
#include <bridging/common.h>
#include <bridging/context.h>

#include <string.h>
#include <sysrepo.h>
#include <sysrepo/xpath.h>

#include <assert.h>

#include <netlink/socket.h>
#include <netlink/route/link.h>

// change callback type
typedef int (*bridging_change_cb)(bridging_ctx_t *ctx, sr_session_ctx_t *session, struct nl_sock *socket, const struct lyd_node *node, sr_change_oper_t operation);

// change callbacks - respond to node changes with the given operation
int bridge_name_change_cb(bridging_ctx_t *ctx, sr_session_ctx_t *session, struct nl_sock *socket, const struct lyd_node *node, sr_change_oper_t operation);
int bridge_address_change_cb(bridging_ctx_t *ctx, sr_session_ctx_t *session, struct nl_sock *socket, const struct lyd_node *node, sr_change_oper_t operation);

// common functionality for iterating changes specified by the given xpath - calls callback function for each iterated node
int apply_change(bridging_ctx_t *ctx, sr_session_ctx_t *session, const char *xpath, bridging_change_cb cb);

// helpers
static void mac_address_ly_to_nl(char *addr);

int bridging_bridge_list_change_cb(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data)
{
	int error = 0;

	// context
	bridging_ctx_t *ctx = (bridging_ctx_t *) private_data;

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
		// name change
		error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s//name", xpath);
		if (error < 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() error: %d", error);
			goto error_out;
		}
		error = apply_change(ctx, session, xpath_buffer, bridge_name_change_cb);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "apply_change() for bridge name failed: %d", error);
			goto error_out;
		}

		// address change
		error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s//address", xpath);
		if (error < 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() error: %d", error);
			goto error_out;
		}
		error = apply_change(ctx, session, xpath_buffer, bridge_address_change_cb);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "apply_change() for bridge address failed: %d", error);
			goto error_out;
		}
	}
	goto error_out;

error_out:
	SRPLG_LOG_ERR(PLUGIN_NAME, "error applying bridge list module changes");
	error = -1;

out:
	return error != 0 ? SR_ERR_CALLBACK_FAILED : SR_ERR_OK;
}

int apply_change(bridging_ctx_t *ctx, sr_session_ctx_t *session, const char *xpath, bridging_change_cb cb)
{
	int error = 0;

	// sysrepo
	sr_change_iter_t *changes_iterator = NULL;
	sr_change_oper_t operation = SR_OP_CREATED;
	const char *prev_value = NULL, *prev_list = NULL;
	int prev_default;

	struct nl_sock *socket = NULL;

	// libyang
	const struct lyd_node *node = NULL;

	SRPLG_LOG_DBG(PLUGIN_NAME, "Getting changes for xpath %s", xpath);

	// connect to libnl

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

	error = sr_get_changes_iter(session, xpath, &changes_iterator);
	if (error != SR_ERR_OK) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_get_changes_iter() failed (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	while (sr_get_change_tree_next(session, changes_iterator, &operation, &node, &prev_value, &prev_list, &prev_default) == SR_ERR_OK) {
		error = cb(ctx, session, socket, node, operation);
		if (error != 0) {
			goto error_out;
		}
	}

	goto out;

error_out:
	error = -1;

out:
	// libnl
	if (socket) {
		nl_socket_free(socket);
	}

	// iterator
	sr_free_change_iter(changes_iterator);

	return error;
}

int bridge_name_change_cb(bridging_ctx_t *ctx, sr_session_ctx_t *session, struct nl_sock *socket, const struct lyd_node *node, sr_change_oper_t operation)
{
	int error = 0;

	char change_path[PATH_MAX] = {0};
	const char *node_name = NULL;
	const char *node_value = NULL;

	// libnl
	struct rtnl_link *tmp_bridge = NULL;

	error = (lyd_path(node, LYD_PATH_STD, change_path, sizeof(change_path)) == NULL);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "lyd_path() error: %d", error);
		goto error_out;
	}

	node_name = sr_xpath_node_name(change_path);
	node_value = lyd_get_value(node);

	assert(strcmp(node_name, "name") == 0);

	SRPLG_LOG_DBG(PLUGIN_NAME, "Node Path: %s", change_path);
	SRPLG_LOG_DBG(PLUGIN_NAME, "Node Name: %s", node_name);
	SRPLG_LOG_DBG(PLUGIN_NAME, "Value: %s; Operation: %d", node_value, operation);

	switch (operation) {
		case SR_OP_CREATED:
			// create new bridge interface
			tmp_bridge = rtnl_link_bridge_alloc();
			rtnl_link_set_name(tmp_bridge, node_value);

			error = rtnl_link_add(socket, tmp_bridge, 0);
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_add() failed (%d): %s", error, nl_geterror(error));
				goto error_out;
			}

			break;
		case SR_OP_MODIFIED:
			// name cannot be changed
			break;
		case SR_OP_DELETED:
			// delete bridge whose name matches the given node value
			tmp_bridge = rtnl_link_bridge_alloc();
			rtnl_link_set_name(tmp_bridge, node_value);

			// delete bridge
			error = rtnl_link_delete(socket, tmp_bridge);
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_delete() failed (%d): %s", error, nl_geterror(error));
				goto error_out;
			}
			break;
		case SR_OP_MOVED:
			break;
	}

	goto out;

error_out:
	error = -1;

out:

	// free bridge
	if (tmp_bridge) {
		rtnl_link_put(tmp_bridge);
	}
	return error;
}

int bridge_address_change_cb(bridging_ctx_t *ctx, sr_session_ctx_t *session, struct nl_sock *socket, const struct lyd_node *node, sr_change_oper_t operation)
{
	int error = 0;

	// helper variables
	char change_path[PATH_MAX] = {0};
	char tmp_xpath[PATH_MAX] = {0};
	const char *node_name = NULL;
	const char *node_value = NULL;
	const char *bridge_name = NULL;

	// sysrepo
	sr_xpath_ctx_t xpath_ctx = {0};

	// libnl
	struct rtnl_link *orig_link = NULL;
	struct rtnl_link *change_link = NULL;
	struct nl_addr *address = NULL;
	struct nl_cache *link_cache = NULL;

	error = (lyd_path(node, LYD_PATH_STD, change_path, sizeof(change_path)) == NULL);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "lyd_path() error: %d", error);
		goto error_out;
	}

	// use temp buffer for xpath operations
	memcpy(tmp_xpath, change_path, sizeof(change_path));

	node_name = sr_xpath_node_name(tmp_xpath);
	node_value = lyd_get_value(node);
	bridge_name = sr_xpath_key_value(tmp_xpath, "bridge", "name", &xpath_ctx);

	assert(strcmp(node_name, "address") == 0);

	// get cache of interfaces
	error = rtnl_link_alloc_cache(socket, AF_UNSPEC, &link_cache);
	if (error < 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_alloc_cache() failed (%d): %s", error, nl_geterror(error));
		goto error_out;
	}

	SRPLG_LOG_DBG(PLUGIN_NAME, "Node Path: %s", change_path);
	SRPLG_LOG_DBG(PLUGIN_NAME, "Node Name: %s", node_name);
	SRPLG_LOG_DBG(PLUGIN_NAME, "Bridge name: %s", bridge_name);
	SRPLG_LOG_DBG(PLUGIN_NAME, "Value: %s; Operation: %d", node_value, operation);

	switch (operation) {
		case SR_OP_CREATED:
			// change address of the interface - same as modified
		case SR_OP_MODIFIED:
			// get bridge interface and set its address
			orig_link = rtnl_link_get_by_name(link_cache, bridge_name);
			if (orig_link == NULL) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_get_by_name() failed (%d): %s", error, nl_geterror(error));
				goto error_out;
			}

			// convert to libnl mac address format
			mac_address_ly_to_nl((char *) node_value);
			error = nl_addr_parse(node_value, AF_LLC, &address);
			if (error < 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "nl_addr_parse() failed (%d): %s", error, nl_geterror(error));
				goto error_out;
			}

			// configure link for changes
			change_link = rtnl_link_bridge_alloc();
			rtnl_link_set_name(change_link, bridge_name);
			rtnl_link_set_addr(change_link, address);

			error = rtnl_link_change(socket, orig_link, change_link, 0);
			if (error < 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_change() failed (%d): %s", error, nl_geterror(error));
				goto error_out;
			}
			break;
		case SR_OP_DELETED:
			// address cannot be deleted
		case SR_OP_MOVED:
			break;
	}

	goto out;

error_out:
	error = -1;

out:

	// libnl
	if (link_cache) {
		nl_cache_free(link_cache);
	}
	if (change_link) {
		rtnl_link_put(change_link);
	}
	return error;
}

static void mac_address_ly_to_nl(char *addr)
{
	char *tmp_ptr = addr;
	while ((tmp_ptr = strchr(tmp_ptr, '-'))) {
		*tmp_ptr = ':';
	}
}