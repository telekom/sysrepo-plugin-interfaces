#include "plugin.h"
#include "netlink/cache.h"
#include "netlink/route/link.h"
#include "netlink/socket.h"
#include "plugin/common.h"
#include "plugin/context.h"

// startup
#include "plugin/data/interfaces/interface/state.h"
#include "plugin/startup/load.h"
#include "plugin/startup/store.h"

// subscription
#include "plugin/subscription/change.h"
#include "plugin/subscription/operational.h"
#include "plugin/subscription/rpc.h"
#include "srpc/common.h"
#include "srpc/feature_status.h"
#include "srpc/types.h"

#include <libyang/libyang.h>
#include <pthread.h>
#include <srpc.h>
#include <sysrepo.h>

static int interfaces_init_state_changes_tracking(interfaces_state_changes_ctx_t* ctx);
static void interfaces_link_cache_change_cb(struct nl_cache* cache, struct nl_object* obj, int val, void* arg);
static void* interfaces_link_manager_thread_cb(void* data);

int sr_plugin_init_cb(sr_session_ctx_t* running_session, void** private_data)
{
    int error = 0;

    bool empty_startup = false;

    // sysrepo
    sr_session_ctx_t* startup_session = NULL;
    sr_conn_ctx_t* connection = NULL;
    sr_subscription_ctx_t* subscription = NULL;
    srpc_feature_status_t* ietf_interfaces_features = NULL;
    srpc_feature_status_t* ietf_if_extensions_features = NULL;

    // plugin
    interfaces_ctx_t* ctx = NULL;

    // init context
    ctx = malloc(sizeof(*ctx));
    *ctx = (interfaces_ctx_t) { 0 };

    *private_data = ctx;

    // initialize feature status hashes
    ietf_interfaces_features = srpc_feature_status_hash_new();
    ietf_if_extensions_features = srpc_feature_status_hash_new();

    // load enabled features from modules in sysrepo
    SRPC_SAFE_CALL_ERR(error, srpc_feature_status_hash_load(&ietf_interfaces_features, running_session, "ietf-interfaces"), error_out);
    SRPC_SAFE_CALL_ERR(error, srpc_feature_status_hash_load(&ietf_if_extensions_features, running_session, "ietf-if-extensions"), error_out);

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
            INTERFACES_INTERFACES_INTERFACE_STATISTICS_IN_DISCARD_UNKNOWN_ENCAPS_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_statistics_in_discard_unknown_encaps,
        },
        {
            INTERFACES_INTERFACES_INTERFACE_CARRIER_DELAY_CARRIER_TRANSITIONS_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_carrier_delay_carrier_transitions,
        },
        {
            INTERFACES_INTERFACES_INTERFACE_CARRIER_DELAY_TIMER_RUNNING_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_carrier_delay_timer_running,
        },
        {
            INTERFACES_INTERFACES_INTERFACE_DAMPENING_PENALTY_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_dampening_penalty,
        },
        {
            INTERFACES_INTERFACES_INTERFACE_DAMPENING_SUPPRESSED_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_dampening_suppressed,
        },
        {
            INTERFACES_INTERFACES_INTERFACE_DAMPENING_TIME_REMAINING_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_dampening_time_remaining,
        },
        {
            INTERFACES_INTERFACES_INTERFACE_FORWARDING_MODE_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_forwarding_mode,
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

    // tracking oper-status changes for interfaces
    SRPC_SAFE_CALL_ERR(error, interfaces_init_state_changes_tracking(&ctx->state_ctx), error_out);

    goto out;

error_out:
    error = -1;
    SRPLG_LOG_ERR(PLUGIN_NAME, "Error occured while initializing the plugin (%d)", error);

out:
    srpc_feature_status_hash_free(&ietf_interfaces_features);
    srpc_feature_status_hash_free(&ietf_if_extensions_features);

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

    pthread_mutex_lock(&ctx->state_ctx.state_hash_mutex);

    if (ctx->state_ctx.socket) {
        nl_socket_free(ctx->state_ctx.socket);
    }

    if (ctx->state_ctx.link_cache_manager) {
        nl_cache_mngr_free(ctx->state_ctx.link_cache_manager);
    }

    if (ctx->state_ctx.state_hash) {
        interfaces_interface_state_hash_free(&ctx->state_ctx.state_hash);
    }

    pthread_mutex_unlock(&ctx->state_ctx.state_hash_mutex);

    free(ctx);
}

static int interfaces_init_state_changes_tracking(interfaces_state_changes_ctx_t* ctx)
{
    int error = 0;
    struct rtnl_link* link = NULL;
    pthread_attr_t attr;

    // init hash
    ctx->state_hash = interfaces_interface_state_hash_new();

    // init libnl data
    SRPC_SAFE_CALL_PTR(ctx->socket, nl_socket_alloc(), error_out);

    // connect and get all links
    SRPC_SAFE_CALL_ERR(error, nl_connect(ctx->socket, NETLINK_ROUTE), error_out);
    SRPC_SAFE_CALL_ERR(error, rtnl_link_alloc_cache(ctx->socket, AF_UNSPEC, &ctx->link_cache), error_out);

    // init hash mutex
    SRPC_SAFE_CALL_ERR(error, pthread_mutex_init(&ctx->state_hash_mutex, NULL), error_out);

    link = (struct rtnl_link*)nl_cache_get_first(ctx->link_cache);

    while (link != NULL) {
        // create hash entries
        const uint8_t oper_state = rtnl_link_get_operstate(link);
        const time_t current_time = time(NULL);
        const char* link_name = rtnl_link_get_name(link);

        // add entry to the hash table
        SRPC_SAFE_CALL_ERR(error, interfaces_interface_state_hash_add(&ctx->state_hash, link_name, oper_state, current_time), error_out);

        link = (struct rtnl_link*)nl_cache_get_next((struct nl_object*)link);
    }

    // setup cache manager
    SRPC_SAFE_CALL_ERR(error, nl_cache_mngr_alloc(NULL, NETLINK_ROUTE, 0, &ctx->link_cache_manager), error_out);
    SRPC_SAFE_CALL_ERR(error, nl_cache_mngr_add(ctx->link_cache_manager, "route/link", interfaces_link_cache_change_cb, ctx, &ctx->link_cache), error_out);

    // setup detatched thread for sending change signals to the cache manager
    SRPC_SAFE_CALL_ERR(error, pthread_attr_init(&attr), error_out);
    SRPC_SAFE_CALL_ERR(error, pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED), error_out);
    SRPC_SAFE_CALL_ERR(error, pthread_create(&ctx->manager_thread, &attr, interfaces_link_manager_thread_cb, ctx), error_out);

    goto out;

error_out:
    error = -1;

out:

    return error;
}

static void interfaces_link_cache_change_cb(struct nl_cache* cache, struct nl_object* obj, int val, void* arg)
{
    interfaces_state_changes_ctx_t* ctx = arg;
    char time_buffer[100] = { 0 };
    struct tm* last_change = NULL;

    // block further access using mutex

    pthread_mutex_lock(&ctx->state_hash_mutex);

    struct rtnl_link* link = NULL;

    SRPLG_LOG_INF(PLUGIN_NAME, "Entered callback function for handling link cache changes");

    link = (struct rtnl_link*)nl_cache_get_first(cache);

    while (link != NULL) {
        const char* link_name = rtnl_link_get_name(link);
        interfaces_interface_state_t* state = interfaces_interface_state_hash_get(ctx->state_hash, link_name);
        const uint8_t oper_state = rtnl_link_get_operstate(link);

        if (state) {
            if (oper_state != state->state) {
                const time_t current = time(NULL);
                last_change = localtime(&current);
                strftime(time_buffer, sizeof(time_buffer), "%FT%TZ", last_change);

                SRPLG_LOG_INF(PLUGIN_NAME, "Interface %s changed oper-state from %d to %d at %s", link_name, state->state, oper_state, time_buffer);
                state->state = oper_state;
                state->last_change = time(NULL);
            }
        }

        link = (struct rtnl_link*)nl_cache_get_next((struct nl_object*)link);
    }

    // unlock further access to the state hash
    pthread_mutex_unlock(&ctx->state_hash_mutex);
}

static void* interfaces_link_manager_thread_cb(void* data)
{
    interfaces_state_changes_ctx_t* ctx = data;

    do {
        nl_cache_mngr_data_ready(ctx->link_cache_manager);
        sleep(1);
    } while (1);

    return NULL;
}