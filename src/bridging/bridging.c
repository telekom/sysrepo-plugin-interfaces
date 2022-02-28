#include <bridging/bridging.h>
#include <bridging/common.h>
#include <bridging/context.h>
#include <utils/memory.h>

// stdlib
#include <stdbool.h>

// sysrepo
#include <sysrepo.h>

// libyang
#include <libyang/tree_data.h>

// bridging
#include "startup.h"

static bool bridging_running_datastore_is_empty(sr_session_ctx_t *session);

int bridging_sr_plugin_init_cb(sr_session_ctx_t *session, void **private_data)
{
	int error = 0;

	// sysrepo
	sr_session_ctx_t *startup_session = NULL;
	sr_conn_ctx_t *connection = NULL;
	sr_subscription_ctx_t *subscription = NULL;

	// plugin
	bridging_ctx_t *ctx = NULL;

	// init context
	ctx = xmalloc(sizeof(*ctx));
	*ctx = (bridging_ctx_t){0};

	*private_data = ctx;

	connection = sr_session_get_connection(session);
	error = sr_session_start(connection, SR_DS_STARTUP, &startup_session);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_session_start() error (%d): %s", error, sr_strerror(error));
	}

	ctx->startup_session = startup_session;

	if (bridging_running_datastore_is_empty(session)) {
		SRPLG_LOG_INF(PLUGIN_NAME, "Running datasore is empty");
		SRPLG_LOG_INF(PLUGIN_NAME, "Loading initial data");
		error = bridging_startup_load_data(ctx, session);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "Error loading initial data into the datastore... exiting");
			goto error_out;
		}
		error = sr_copy_config(startup_session, BASE_YANG_MODEL, SR_DS_RUNNING, 0);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "sr_copy_config() error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}
	}

	goto out;

error_out:
	error = -1;
	SRPLG_LOG_ERR(PLUGIN_NAME, "Error occured while initializing the plugin (%d)", error);

out:
	return error ? SR_ERR_CALLBACK_FAILED : SR_ERR_OK;
}

void bridging_sr_plugin_cleanup_cb(sr_session_ctx_t *session, void *private_data)
{
	bridging_ctx_t *ctx = (bridging_ctx_t *) private_data;

	FREE_SAFE(ctx);
}

static bool bridging_running_datastore_is_empty(sr_session_ctx_t *session)
{
	int error = SR_ERR_OK;
	bool is_empty = true;
	sr_data_t *test_data = NULL;

	error = sr_get_subtree(session, BRIDGING_BRIDGES_CONTAINER_YANG_PATH, 0, &test_data);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_get_subtree() error (%d): %s", error, sr_strerror(error));
		goto out;
	}

	if (test_data->tree != NULL && lyd_child(test_data->tree) != NULL) {
		// main container found: datastore is not empty
		is_empty = false;
	}

out:
	sr_release_data(test_data);

	return is_empty;
}