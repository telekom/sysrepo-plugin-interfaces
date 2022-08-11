#include "plugin.h"
#include "netlink/cache.h"
#include "netlink/socket.h"
#include "plugin/common.h"
#include "plugin/context.h"

// startup
#include "plugin/startup/load.h"
#include "plugin/startup/store.h"

// subscription
#include "plugin/subscription/change.h"
#include "plugin/subscription/operational.h"
#include "plugin/subscription/rpc.h"

#include <libyang/libyang.h>
#include <srpc.h>
#include <sysrepo.h>

int sr_plugin_init_cb(sr_session_ctx_t* running_session, void** private_data)
{
    int error = 0;

    bool empty_startup = false;

    // sysrepo
    sr_session_ctx_t* startup_session = NULL;
    sr_conn_ctx_t* connection = NULL;
    sr_subscription_ctx_t* subscription = NULL;

    // plugin
    interfaces_ctx_t* ctx = NULL;

    // init context
    ctx = malloc(sizeof(*ctx));
    *ctx = (interfaces_ctx_t) { 0 };

    *private_data = ctx;

    // module changes
    srpc_module_change_t module_changes[] = {
        {
            INTERFACES_INTERFACES_INTERFACE_YANG_PATH,
            interfaces_subscription_change_interfaces_interface,
        },
    };

    // operational getters
    srpc_operational_t oper[] = {
        {
            INTERFACES_INTERFACES_INTERFACE_ADMIN_STATUS_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_admin_status,
        },
        {
            INTERFACES_INTERFACES_INTERFACE_OPER_STATUS_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_oper_status,
        },
        {
            INTERFACES_INTERFACES_INTERFACE_LAST_CHANGE_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_last_change,
        },
        {
            INTERFACES_INTERFACES_INTERFACE_IF_INDEX_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_if_index,
        },
        {
            INTERFACES_INTERFACES_INTERFACE_PHYS_ADDRESS_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_phys_address,
        },
        {
            INTERFACES_INTERFACES_INTERFACE_HIGHER_LAYER_IF_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_higher_layer_if,
        },
        {
            INTERFACES_INTERFACES_INTERFACE_LOWER_LAYER_IF_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_lower_layer_if,
        },
        {
            INTERFACES_INTERFACES_INTERFACE_SPEED_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_speed,
        },
        {
            INTERFACES_INTERFACES_INTERFACE_STATISTICS_DISCONTINUITY_TIME_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_statistics_discontinuity_time,
        },
        {
            INTERFACES_INTERFACES_INTERFACE_STATISTICS_IN_OCTETS_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_statistics_in_octets,
        },
        {
            INTERFACES_INTERFACES_INTERFACE_STATISTICS_IN_UNICAST_PKTS_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_statistics_in_unicast_pkts,
        },
        {
            INTERFACES_INTERFACES_INTERFACE_STATISTICS_IN_BROADCAST_PKTS_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_statistics_in_broadcast_pkts,
        },
        {
            INTERFACES_INTERFACES_INTERFACE_STATISTICS_IN_MULTICAST_PKTS_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_statistics_in_multicast_pkts,
        },
        {
            INTERFACES_INTERFACES_INTERFACE_STATISTICS_IN_DISCARDS_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_statistics_in_discards,
        },
        {
            INTERFACES_INTERFACES_INTERFACE_STATISTICS_IN_ERRORS_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_statistics_in_errors,
        },
        {
            INTERFACES_INTERFACES_INTERFACE_STATISTICS_IN_UNKNOWN_PROTOS_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_statistics_in_unknown_protos,
        },
        {
            INTERFACES_INTERFACES_INTERFACE_STATISTICS_OUT_OCTETS_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_statistics_out_octets,
        },
        {
            INTERFACES_INTERFACES_INTERFACE_STATISTICS_OUT_UNICAST_PKTS_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_statistics_out_unicast_pkts,
        },
        {
            INTERFACES_INTERFACES_INTERFACE_STATISTICS_OUT_BROADCAST_PKTS_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_statistics_out_broadcast_pkts,
        },
        {
            INTERFACES_INTERFACES_INTERFACE_STATISTICS_OUT_MULTICAST_PKTS_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_statistics_out_multicast_pkts,
        },
        {
            INTERFACES_INTERFACES_INTERFACE_STATISTICS_OUT_DISCARDS_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_statistics_out_discards,
        },
        {
            INTERFACES_INTERFACES_INTERFACE_STATISTICS_OUT_ERRORS_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_statistics_out_errors,
        },
        {
            INTERFACES_INTERFACES_INTERFACE_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface,
        },
    };

    connection = sr_session_get_connection(running_session);
    error = sr_session_start(connection, SR_DS_STARTUP, &startup_session);
    if (error) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "sr_session_start() error (%d): %s", error, sr_strerror(error));
        goto error_out;
    }

    ctx->startup_session = startup_session;

    error = srpc_check_empty_datastore(startup_session, INTERFACES_INTERFACES_INTERFACE_YANG_PATH, &empty_startup);
    if (error) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "Failed checking datastore contents: %d", error);
        goto error_out;
    }

    if (empty_startup) {
        SRPLG_LOG_INF(PLUGIN_NAME, "Startup datastore is empty");
        SRPLG_LOG_INF(PLUGIN_NAME, "Loading initial system data");
        error = interfaces_startup_load(ctx, startup_session);
        if (error) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "Error loading initial data into the startup datastore... exiting");
            goto error_out;
        }

        // copy contents of the startup session to the current running session
        error = sr_copy_config(running_session, BASE_YANG_MODEL, SR_DS_STARTUP, 0);
        if (error) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "sr_copy_config() error (%d): %s", error, sr_strerror(error));
            goto error_out;
        }
    } else {
        // make sure the data from startup DS is stored in the interfaces
        SRPLG_LOG_INF(PLUGIN_NAME, "Startup datastore contains data");
        SRPLG_LOG_INF(PLUGIN_NAME, "Storing startup datastore data in the system");

        error = interfaces_startup_store(ctx, startup_session);
        if (error) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "Error applying initial data from startup datastore to the system... exiting");
            goto error_out;
        }

        // copy contents of the startup session to the current running session
        error = sr_copy_config(running_session, BASE_YANG_MODEL, SR_DS_STARTUP, 0);
        if (error) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "sr_copy_config() error (%d): %s", error, sr_strerror(error));
            goto error_out;
        }
    }

    // subscribe every module change
    for (size_t i = 0; i < ARRAY_SIZE(module_changes); i++) {
        const srpc_module_change_t* change = &module_changes[i];

        // in case of work on a specific callback set it to NULL
        if (change->cb) {
            error = sr_module_change_subscribe(running_session, BASE_YANG_MODEL, change->path, change->cb, *private_data, 0, SR_SUBSCR_DEFAULT, &subscription);
            if (error) {
                SRPLG_LOG_ERR(PLUGIN_NAME, "sr_module_change_subscribe() error for \"%s\" (%d): %s", change->path, error, sr_strerror(error));
                goto error_out;
            }
        }
    }

    // subscribe every operational getter
    for (size_t i = 0; i < ARRAY_SIZE(oper); i++) {
        const srpc_operational_t* op = &oper[i];

        // in case of work on a specific callback set it to NULL
        if (op->cb) {
            error = sr_oper_get_subscribe(running_session, BASE_YANG_MODEL, op->path, op->cb, *private_data, SR_SUBSCR_DEFAULT, &subscription);
            if (error) {
                SRPLG_LOG_ERR(PLUGIN_NAME, "sr_oper_get_subscribe() error (%d): %s", error, sr_strerror(error));
                goto error_out;
            }
        }
    }

    goto out;

error_out:
    error = -1;
    SRPLG_LOG_ERR(PLUGIN_NAME, "Error occured while initializing the plugin (%d)", error);

out:
    return error ? SR_ERR_CALLBACK_FAILED : SR_ERR_OK;
}

void sr_plugin_cleanup_cb(sr_session_ctx_t* running_session, void* private_data)
{
    int error = 0;

    interfaces_ctx_t* ctx = (interfaces_ctx_t*)private_data;

    // save current running configuration into startup for next time when the plugin starts
    error = sr_copy_config(ctx->startup_session, BASE_YANG_MODEL, SR_DS_RUNNING, 0);
    if (error) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "sr_copy_config() error (%d): %s", error, sr_strerror(error));
    }

    if (ctx->nl_ctx.link_cache) {
        nl_cache_put(ctx->nl_ctx.link_cache);
    }

    if (ctx->nl_ctx.socket) {
        nl_socket_free(ctx->nl_ctx.socket);
    }

    free(ctx);
}