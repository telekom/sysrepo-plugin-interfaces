#include "change.h"
#include "sysrepo_types.h"
#include <bridging/common.h>
#include <bridging/context.h>

#include <sysrepo.h>
#include <sysrepo/xpath.h>

// change callback type
typedef int (*bridging_change_cb)(bridging_ctx_t *ctx, sr_session_ctx_t *session, const struct lyd_node *node, sr_change_oper_t operation);

// change callbacks - respond to node changes with the given operation
int bridge_name_change_cb(bridging_ctx_t *ctx, sr_session_ctx_t *session, const struct lyd_node *node, sr_change_oper_t operation);
int bridge_address_change_cb(bridging_ctx_t *ctx, sr_session_ctx_t *session, const struct lyd_node *node, sr_change_oper_t operation);

// common functionality for iterating changes specified by the given xpath - calls callback function for each iterated node
int apply_change(bridging_ctx_t *ctx, sr_session_ctx_t *session, const char *xpath, bridging_change_cb cb);

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
		if (error < 0) {
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
		if (error < 0) {
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

	// libyang
	const struct lyd_node *node = NULL;

	SRPLG_LOG_DBG(PLUGIN_NAME, "Getting changes for xpath %s", xpath);

	error = sr_get_changes_iter(session, xpath, &changes_iterator);
	if (error != SR_ERR_OK) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_get_changes_iter() failed (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	while (sr_get_change_tree_next(session, changes_iterator, &operation, &node, &prev_value, &prev_list, &prev_default) == SR_ERR_OK) {
		cb(ctx, session, node, operation);
	}

	goto out;

error_out:
	error = -1;

out:

	// iterator
	sr_free_change_iter(changes_iterator);

	return error;
}

int bridge_name_change_cb(bridging_ctx_t *ctx, sr_session_ctx_t *session, const struct lyd_node *node, sr_change_oper_t operation)
{
	int error = 0;

	char change_path[PATH_MAX] = {0};
	const char *node_name = NULL;
	const char *node_value = NULL;

	error = (lyd_path(node, LYD_PATH_STD, change_path, sizeof(change_path)) == NULL);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "lyd_path() error: %d", error);
		goto error_out;
	}

	node_name = sr_xpath_node_name(change_path);
	node_value = lyd_get_value(node);

	SRPLG_LOG_DBG(PLUGIN_NAME, "Node Path: %s", change_path);
	SRPLG_LOG_DBG(PLUGIN_NAME, "Node Name: %s", node_name);
	SRPLG_LOG_DBG(PLUGIN_NAME, "Value: %s; Operation: %d", node_value, operation);

	goto out;

error_out:
	error = -1;

out:
	return error;
}

int bridge_address_change_cb(bridging_ctx_t *ctx, sr_session_ctx_t *session, const struct lyd_node *node, sr_change_oper_t operation)
{
	int error = 0;

	char change_path[PATH_MAX] = {0};
	const char *node_name = NULL;
	const char *node_value = NULL;

	error = (lyd_path(node, LYD_PATH_STD, change_path, sizeof(change_path)) == NULL);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "lyd_path() error: %d", error);
		goto error_out;
	}

	node_name = sr_xpath_node_name(change_path);
	node_value = lyd_get_value(node);

	SRPLG_LOG_DBG(PLUGIN_NAME, "Node Path: %s", change_path);
	SRPLG_LOG_DBG(PLUGIN_NAME, "Node Name: %s", node_name);
	SRPLG_LOG_DBG(PLUGIN_NAME, "Value: %s; Operation: %d", node_value, operation);

	goto out;

error_out:
	error = -1;

out:
	return error;
}