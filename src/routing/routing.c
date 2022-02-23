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

// startup datastore -> initial load
#include "startup.h"

static bool routing_running_datastore_is_empty(sr_session_ctx_t *session);

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

	if (routing_running_datastore_is_empty(session)) {
		SRPLG_LOG_INF(PLUGIN_NAME, "running datasore is empty -> loading data");
		error = routing_startup_load_data(ctx, session);
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
