#include "plugin.h"
#include "netlink/cache.h"
#include "netlink/route/link.h"
#include "netlink/socket.h"
#include "plugin/common.h"
#include "plugin/context.h"
#include "plugin/data/interfaces/interface_state.h"

// startup DS
#include "plugin/startup/load.h"
#include "plugin/startup/store.h"

// running DS
#include "plugin/running/load.h"
#include "plugin/running/store.h"

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

    // plugin
    interfaces_ctx_t* ctx = NULL;

    // init context
    ctx = malloc(sizeof(*ctx));
    *ctx = (interfaces_ctx_t) { 0 };

    *private_data = ctx;

    // load enabled features from modules in sysrepo
    SRPC_SAFE_CALL_ERR(error, srpc_feature_status_hash_load(&ctx->features.ietf_interfaces_features, running_session, "ietf-interfaces"), error_out);
    SRPC_SAFE_CALL_ERR(error, srpc_feature_status_hash_load(&ctx->features.ietf_if_extensions_features, running_session, "ietf-if-extensions"), error_out);
    SRPC_SAFE_CALL_ERR(error, srpc_feature_status_hash_load(&ctx->features.ietf_ip_features, running_session, "ietf-ip"), error_out);

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
            // depends on if-mib feature
            IETF_INTERFACES_YANG_MODULE,
            INTERFACES_INTERFACES_INTERFACE_ADMIN_STATUS_YANG_PATH,
            srpc_feature_status_hash_check(ctx->features.ietf_interfaces_features, "if-mib") ? interfaces_subscription_operational_interfaces_interface_admin_status : NULL,
        },
        {
            IETF_INTERFACES_YANG_MODULE,
            INTERFACES_INTERFACES_INTERFACE_OPER_STATUS_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_oper_status,
        },
        {
            IETF_INTERFACES_YANG_MODULE,
            INTERFACES_INTERFACES_INTERFACE_LAST_CHANGE_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_last_change,
        },
        {
            IETF_INTERFACES_YANG_MODULE,
            // depends on if-mib feature
            INTERFACES_INTERFACES_INTERFACE_IF_INDEX_YANG_PATH,
            srpc_feature_status_hash_check(ctx->features.ietf_interfaces_features, "if-mib") ? interfaces_subscription_operational_interfaces_interface_if_index : NULL,
        },
        {
            IETF_INTERFACES_YANG_MODULE,
            INTERFACES_INTERFACES_INTERFACE_PHYS_ADDRESS_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_phys_address,
        },
        {
            IETF_INTERFACES_YANG_MODULE,
            INTERFACES_INTERFACES_INTERFACE_HIGHER_LAYER_IF_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_higher_layer_if,
        },
        {
            IETF_INTERFACES_YANG_MODULE,
            INTERFACES_INTERFACES_INTERFACE_LOWER_LAYER_IF_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_lower_layer_if,
        },
        {
            IETF_INTERFACES_YANG_MODULE,
            INTERFACES_INTERFACES_INTERFACE_SPEED_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_speed,
        },
        {
            IETF_INTERFACES_YANG_MODULE,
            INTERFACES_INTERFACES_INTERFACE_STATISTICS_DISCONTINUITY_TIME_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_statistics_discontinuity_time,
        },
        {
            IETF_INTERFACES_YANG_MODULE,
            INTERFACES_INTERFACES_INTERFACE_STATISTICS_IN_OCTETS_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_statistics_in_octets,
        },
        {
            IETF_INTERFACES_YANG_MODULE,
            INTERFACES_INTERFACES_INTERFACE_STATISTICS_IN_UNICAST_PKTS_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_statistics_in_unicast_pkts,
        },
        {
            IETF_INTERFACES_YANG_MODULE,
            INTERFACES_INTERFACES_INTERFACE_STATISTICS_IN_BROADCAST_PKTS_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_statistics_in_broadcast_pkts,
        },
        {
            IETF_INTERFACES_YANG_MODULE,
            INTERFACES_INTERFACES_INTERFACE_STATISTICS_IN_MULTICAST_PKTS_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_statistics_in_multicast_pkts,
        },
        {
            IETF_INTERFACES_YANG_MODULE,
            INTERFACES_INTERFACES_INTERFACE_STATISTICS_IN_DISCARDS_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_statistics_in_discards,
        },
        {
            IETF_INTERFACES_YANG_MODULE,
            INTERFACES_INTERFACES_INTERFACE_STATISTICS_IN_ERRORS_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_statistics_in_errors,
        },
        {
            IETF_INTERFACES_YANG_MODULE,
            INTERFACES_INTERFACES_INTERFACE_STATISTICS_IN_UNKNOWN_PROTOS_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_statistics_in_unknown_protos,
        },
        {
            IETF_INTERFACES_YANG_MODULE,
            INTERFACES_INTERFACES_INTERFACE_STATISTICS_OUT_OCTETS_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_statistics_out_octets,
        },
        {
            IETF_INTERFACES_YANG_MODULE,
            INTERFACES_INTERFACES_INTERFACE_STATISTICS_OUT_UNICAST_PKTS_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_statistics_out_unicast_pkts,
        },
        {
            IETF_INTERFACES_YANG_MODULE,
            INTERFACES_INTERFACES_INTERFACE_STATISTICS_OUT_BROADCAST_PKTS_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_statistics_out_broadcast_pkts,
        },
        {
            IETF_INTERFACES_YANG_MODULE,
            INTERFACES_INTERFACES_INTERFACE_STATISTICS_OUT_MULTICAST_PKTS_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_statistics_out_multicast_pkts,
        },
        {
            IETF_INTERFACES_YANG_MODULE,
            INTERFACES_INTERFACES_INTERFACE_STATISTICS_OUT_DISCARDS_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_statistics_out_discards,
        },
        {
            IETF_INTERFACES_YANG_MODULE,
            INTERFACES_INTERFACES_INTERFACE_STATISTICS_OUT_ERRORS_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_statistics_out_errors,
        },
        {
            IETF_INTERFACES_YANG_MODULE,
            INTERFACES_INTERFACES_INTERFACE_STATISTICS_IN_DISCARD_UNKNOWN_ENCAPS_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_statistics_in_discard_unknown_encaps,
        },
        {
            IETF_INTERFACES_YANG_MODULE,
            // depends on carrier-delay feature
            INTERFACES_INTERFACES_INTERFACE_CARRIER_DELAY_CARRIER_TRANSITIONS_YANG_PATH,
            srpc_feature_status_hash_check(ctx->features.ietf_if_extensions_features, "carrier-delay") ? interfaces_subscription_operational_interfaces_interface_carrier_delay_carrier_transitions : NULL,
        },
        {
            IETF_INTERFACES_YANG_MODULE,
            // depends on carrier-delay feature
            INTERFACES_INTERFACES_INTERFACE_CARRIER_DELAY_TIMER_RUNNING_YANG_PATH,
            srpc_feature_status_hash_check(ctx->features.ietf_if_extensions_features, "carrier-delay") ? interfaces_subscription_operational_interfaces_interface_carrier_delay_timer_running : NULL,
        },
        {
            IETF_INTERFACES_YANG_MODULE,
            // depends on dampening feature
            INTERFACES_INTERFACES_INTERFACE_DAMPENING_PENALTY_YANG_PATH,
            srpc_feature_status_hash_check(ctx->features.ietf_if_extensions_features, "dampening") ? interfaces_subscription_operational_interfaces_interface_dampening_penalty : NULL,
        },
        {
            IETF_INTERFACES_YANG_MODULE,
            // depends on dampening feature
            INTERFACES_INTERFACES_INTERFACE_DAMPENING_SUPPRESSED_YANG_PATH,
            srpc_feature_status_hash_check(ctx->features.ietf_if_extensions_features, "dampening") ? interfaces_subscription_operational_interfaces_interface_dampening_suppressed : NULL,
        },
        {
            IETF_INTERFACES_YANG_MODULE,
            // depends on dampening feature
            INTERFACES_INTERFACES_INTERFACE_DAMPENING_TIME_REMAINING_YANG_PATH,
            srpc_feature_status_hash_check(ctx->features.ietf_if_extensions_features, "dampening") ? interfaces_subscription_operational_interfaces_interface_dampening_time_remaining : NULL,
        },
        {
            IETF_INTERFACES_YANG_MODULE,
            INTERFACES_INTERFACES_INTERFACE_FORWARDING_MODE_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_forwarding_mode,
        },
        {
            IETF_INTERFACES_YANG_MODULE,
            INTERFACES_INTERFACES_INTERFACE_IPV4_ADDRESS_ORIGIN_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_ipv4_address_origin,
        },
        {
            IETF_INTERFACES_YANG_MODULE,
            INTERFACES_INTERFACES_INTERFACE_IPV4_ADDRESS_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_ipv4_address,
        },
        {
            IETF_INTERFACES_YANG_MODULE,
            INTERFACES_INTERFACES_INTERFACE_IPV4_NEIGHBOR_ORIGIN_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_ipv4_neighbor_origin,
        },
        {
            IETF_INTERFACES_YANG_MODULE,
            INTERFACES_INTERFACES_INTERFACE_IPV4_NEIGHBOR_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_ipv4_neighbor,
        },
        {
            IETF_INTERFACES_YANG_MODULE,
            INTERFACES_INTERFACES_INTERFACE_IPV6_ADDRESS_ORIGIN_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_ipv6_address_origin,
        },
        {
            IETF_INTERFACES_YANG_MODULE,
            INTERFACES_INTERFACES_INTERFACE_IPV6_ADDRESS_STATUS_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_ipv6_address_status,
        },
        {
            IETF_INTERFACES_YANG_MODULE,
            INTERFACES_INTERFACES_INTERFACE_IPV6_ADDRESS_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_ipv6_address,
        },
        {
            IETF_INTERFACES_YANG_MODULE,
            INTERFACES_INTERFACES_INTERFACE_IPV6_NEIGHBOR_ORIGIN_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_ipv6_neighbor_origin,
        },
        {
            IETF_INTERFACES_YANG_MODULE,
            INTERFACES_INTERFACES_INTERFACE_IPV6_NEIGHBOR_IS_ROUTER_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_ipv6_neighbor_is_router,
        },
        {
            IETF_INTERFACES_YANG_MODULE,
            INTERFACES_INTERFACES_INTERFACE_IPV6_NEIGHBOR_STATE_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_ipv6_neighbor_state,
        },
        {
            IETF_INTERFACES_YANG_MODULE,
            INTERFACES_INTERFACES_INTERFACE_IPV6_NEIGHBOR_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface_ipv6_neighbor,
        },
        {
            IETF_INTERFACES_YANG_MODULE,
            INTERFACES_INTERFACES_INTERFACE_YANG_PATH,
            interfaces_subscription_operational_interfaces_interface,
        },
    };

    // get connection
    SRPC_SAFE_CALL_PTR(connection, sr_session_get_connection(running_session), error_out);

    // start a session
    SRPC_SAFE_CALL_ERR(error, sr_session_start(connection, SR_DS_STARTUP, &startup_session), error_out);

    ctx->startup_ctx.startup_session = startup_session;

    SRPC_SAFE_CALL_ERR(error, srpc_check_empty_datastore(running_session, INTERFACES_INTERFACES_INTERFACE_YANG_PATH, &empty_startup), error_out);

    if (empty_startup) {
        SRPLG_LOG_INF(PLUGIN_NAME, "Running datastore is empty");
        SRPLG_LOG_INF(PLUGIN_NAME, "Loading initial system data");

        // load initial running DS data on the system
        SRPC_SAFE_CALL_ERR(error, interfaces_running_load(ctx, running_session), error_out);
    } else {
        // make sure the data from startup DS is stored in the interfaces
        SRPLG_LOG_INF(PLUGIN_NAME, "Running datastore contains data");
        SRPLG_LOG_INF(PLUGIN_NAME, "Checking running datastore against system data");

        // check and store running DS data on the system
        SRPC_SAFE_CALL_ERR(error, interfaces_running_store(ctx, running_session), error_out);
    }

    // subscribe every module change
    for (size_t i = 0; i < ARRAY_SIZE(module_changes); i++) {
        const srpc_module_change_t* change = &module_changes[i];

        SRPLG_LOG_INF(PLUGIN_NAME, "Subscribing module change callback %s", change->path);

        // in case of work on a specific callback set it to NULL
        if (change->cb) {
            error = sr_module_change_subscribe(running_session, IETF_INTERFACES_YANG_MODULE, change->path, change->cb, *private_data, 0, SR_SUBSCR_DEFAULT, &subscription);
            if (error) {
                SRPLG_LOG_ERR(PLUGIN_NAME, "sr_module_change_subscribe() error for \"%s\" (%d): %s", change->path, error, sr_strerror(error));
                goto error_out;
            }
        }
    }

    // subscribe every operational getter
    for (size_t i = 0; i < ARRAY_SIZE(oper); i++) {
        const srpc_operational_t* op = &oper[i];

        SRPLG_LOG_INF(PLUGIN_NAME, "Subscribing operational callback %s:%s", op->module, op->path);

        // in case of work on a specific callback set it to NULL
        if (op->cb) {
            error = sr_oper_get_subscribe(running_session, op->module, op->path, op->cb, *private_data, SR_SUBSCR_DEFAULT, &subscription);
            if (error) {
                SRPLG_LOG_ERR(PLUGIN_NAME, "sr_oper_get_subscribe() error for \"%s\" (%d): %s", op->path, error, sr_strerror(error));
                goto error_out;
            }
        }
    }

    // tracking oper-status changes for interfaces
    SRPC_SAFE_CALL_ERR(error, interfaces_init_state_changes_tracking(&ctx->oper_ctx.state_changes_ctx), error_out);

    goto out;

error_out:
    error = -1;
    SRPLG_LOG_ERR(PLUGIN_NAME, "Error occured while initializing the plugin (%d)", error);

out:

    return error ? SR_ERR_CALLBACK_FAILED : SR_ERR_OK;
}

void sr_plugin_cleanup_cb(sr_session_ctx_t* running_session, void* private_data)
{
    interfaces_ctx_t* ctx = (interfaces_ctx_t*)private_data;

    if (ctx->oper_ctx.nl_ctx.link_cache) {
        nl_cache_put(ctx->oper_ctx.nl_ctx.link_cache);
    }

    if (ctx->oper_ctx.nl_ctx.socket) {
        nl_socket_free(ctx->oper_ctx.nl_ctx.socket);
    }

    pthread_mutex_lock(&ctx->oper_ctx.state_changes_ctx.state_hash_mutex);

    if (ctx->oper_ctx.state_changes_ctx.nl_ctx.socket) {
        nl_socket_free(ctx->oper_ctx.state_changes_ctx.nl_ctx.socket);
    }

    if (ctx->oper_ctx.state_changes_ctx.nl_ctx.link_cache_manager) {
        nl_cache_mngr_free(ctx->oper_ctx.state_changes_ctx.nl_ctx.link_cache_manager);
    }

    if (ctx->oper_ctx.state_changes_ctx.state_hash) {
        interfaces_interface_state_hash_free(&ctx->oper_ctx.state_changes_ctx.state_hash);
    }

    pthread_mutex_unlock(&ctx->oper_ctx.state_changes_ctx.state_hash_mutex);

    // free feature status hashes
    srpc_feature_status_hash_free(&ctx->features.ietf_interfaces_features);
    srpc_feature_status_hash_free(&ctx->features.ietf_if_extensions_features);
    srpc_feature_status_hash_free(&ctx->features.ietf_ip_features);

    free(ctx);
}

static int interfaces_init_state_changes_tracking(interfaces_state_changes_ctx_t* ctx)
{
    int error = 0;
    struct rtnl_link* link = NULL;
    pthread_attr_t attr;
    interfaces_interface_state_hash_element_t* new_element = NULL;

    // init hash
    ctx->state_hash = interfaces_interface_state_hash_new();

    // init libnl data
    SRPC_SAFE_CALL_PTR(ctx->nl_ctx.socket, nl_socket_alloc(), error_out);

    // connect and get all links
    SRPC_SAFE_CALL_ERR(error, nl_connect(ctx->nl_ctx.socket, NETLINK_ROUTE), error_out);
    SRPC_SAFE_CALL_ERR(error, rtnl_link_alloc_cache(ctx->nl_ctx.socket, AF_UNSPEC, &ctx->nl_ctx.link_cache), error_out);

    // init hash mutex
    SRPC_SAFE_CALL_ERR(error, pthread_mutex_init(&ctx->state_hash_mutex, NULL), error_out);

    link = (struct rtnl_link*)nl_cache_get_first(ctx->nl_ctx.link_cache);

    while (link != NULL) {
        // create hash entries
        const uint8_t oper_state = rtnl_link_get_operstate(link);
        const time_t current_time = time(NULL);
        const char* link_name = rtnl_link_get_name(link);

        // create new state element
        SRPC_SAFE_CALL_PTR(new_element, interfaces_interface_state_hash_element_new(), error_out);

        // set element data
        SRPC_SAFE_CALL_ERR(error, interfaces_interface_state_hash_element_set_name(&new_element, link_name), error_out);
        interfaces_interface_state_hash_element_set_state(&new_element, oper_state);
        interfaces_interface_state_hash_element_set_last_change(&new_element, current_time);

        // add entry to the hash table
        SRPC_SAFE_CALL_ERR(error, interfaces_interface_state_hash_add(&ctx->state_hash, new_element), error_out);

        link = (struct rtnl_link*)nl_cache_get_next((struct nl_object*)link);
    }

    // setup cache manager
    SRPC_SAFE_CALL_ERR(error, nl_cache_mngr_alloc(NULL, NETLINK_ROUTE, 0, &ctx->nl_ctx.link_cache_manager), error_out);
    SRPC_SAFE_CALL_ERR(error, nl_cache_mngr_add(ctx->nl_ctx.link_cache_manager, "route/link", interfaces_link_cache_change_cb, ctx, &ctx->nl_ctx.link_cache), error_out);

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
    time_t current = 0;
    interfaces_interface_state_hash_element_t* new_element = NULL;
    int rc = 0;

    // block further access using mutex

    pthread_mutex_lock(&ctx->state_hash_mutex);

    struct rtnl_link* link = NULL;

    SRPLG_LOG_INF(PLUGIN_NAME, "Entered callback function for handling link cache changes");

    link = (struct rtnl_link*)nl_cache_get_first(cache);

    while (link != NULL) {
        const char* link_name = rtnl_link_get_name(link);
        interfaces_interface_state_hash_element_t* state_element = interfaces_interface_state_hash_get(ctx->state_hash, link_name);
        const uint8_t oper_state = rtnl_link_get_operstate(link);

        if (state_element) {
            SRPLG_LOG_INF(PLUGIN_NAME, "State for interface %s already exists - updating last change", link_name);
            if (oper_state != state_element->state.state) {
                current = time(NULL);
                last_change = localtime(&current);
                strftime(time_buffer, sizeof(time_buffer), "%FT%TZ", last_change);

                SRPLG_LOG_INF(PLUGIN_NAME, "Interface %s changed oper-state from %d to %d at %s", link_name, state_element->state.state, oper_state, time_buffer);

                // update state info
                interfaces_interface_state_hash_element_set_state(&state_element, oper_state);
                interfaces_interface_state_hash_element_set_last_change(&state_element, time(NULL));
            }
        } else {
            SRPLG_LOG_INF(PLUGIN_NAME, "State for interface %s doesn\'t exist - creating a new entry in the state data hash table", link_name);
            // new link has been added - add new data to the hash
            current = time(NULL);
            last_change = localtime(&current);

            // create new state element
            new_element = interfaces_interface_state_hash_element_new();
            if (!new_element) {
                SRPLG_LOG_ERR(PLUGIN_NAME, "Unable to create new hash element for interface %s", link_name);
            } else {
                rc = interfaces_interface_state_hash_element_set_name(&new_element, link_name);
                if (rc) {
                    SRPLG_LOG_ERR(PLUGIN_NAME, "Error setting interface name for a newly created hash element for interface %s", link_name);
                } else {
                    interfaces_interface_state_hash_element_set_state(&new_element, oper_state);
                    interfaces_interface_state_hash_element_set_last_change(&new_element, current);
                    rc = interfaces_interface_state_hash_add(&ctx->state_hash, new_element);
                    if (rc) {
                        SRPLG_LOG_ERR(PLUGIN_NAME, "Unable to add new interface %s to the interface hash");
                    } else {
                        strftime(time_buffer, sizeof(time_buffer), "%FT%TZ", last_change);
                        SRPLG_LOG_INF(PLUGIN_NAME, "Interface %s added to the state data hash: state = %d, time = %s", link_name, oper_state, time_buffer);
                    }
                }
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
        nl_cache_mngr_data_ready(ctx->nl_ctx.link_cache_manager);
        sleep(1);
    } while (1);

    return NULL;
}