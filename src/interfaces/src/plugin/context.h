#ifndef INTERFACES_PLUGIN_CONTEXT_H
#define INTERFACES_PLUGIN_CONTEXT_H

#include "netlink/cache.h"
#include "plugin/types.h"
#include <pthread.h>
#include <sysrepo_types.h>

#include <netlink/route/link.h>

#include <srpc.h>

// typedefs
typedef struct interfaces_nl_ctx_s interfaces_nl_ctx_t;
typedef struct interfaces_ctx_s interfaces_ctx_t;
typedef struct interfaces_state_changes_ctx_s interfaces_state_changes_ctx_t;
typedef struct interfaces_mod_changes_ctx_s interfaces_mod_changes_ctx_t;
typedef struct interfaces_oper_ctx_s interfaces_oper_ctx_t;
typedef struct interfaces_features_ctx_s interfaces_features_ctx_t;

struct interfaces_features_ctx_s {
    srpc_feature_status_hash_t* ietf_interfaces_features;
    srpc_feature_status_hash_t* ietf_if_extensions_features;
    srpc_feature_status_hash_t* ietf_ip_features;
};

struct interfaces_nl_ctx_s {
    struct nl_sock* socket;
    struct nl_cache* link_cache;
    struct nl_cache* addr_cache;
    struct nl_cache* neigh_cache;
    struct nl_cache_mngr* link_cache_manager;
};

struct interfaces_mod_changes_ctx_s {
    // libnl links data
    interfaces_nl_ctx_t nl_ctx;

    // temporary module changing data
    struct {
        struct {
            struct {
                uint8_t prefix_length;
                uint8_t prefix_set; ///< prefix_length has been set
            } address;
            struct {
                char* link_layer_address;
                uint8_t link_layer_set; ///< link_layer_address has been set
            } neighbor;
        } ipv4;
    } mod_data;
};

struct interfaces_state_changes_ctx_s {
    // libnl data
    interfaces_nl_ctx_t nl_ctx;

    // cache manager refresh thread
    pthread_t manager_thread;

    // main hash DS for storing state info
    interfaces_interface_state_hash_element_t* state_hash;

    // mutex for accessing state hash data
    pthread_mutex_t state_hash_mutex;
};

struct interfaces_oper_ctx_s {
    // operational libnl context - refill cache of links
    interfaces_nl_ctx_t nl_ctx;

    // state changes monitoring
    interfaces_state_changes_ctx_t state_changes_ctx;
};

struct interfaces_ctx_s {
    // startup DS
    sr_session_ctx_t* startup_session;

    // module changes data
    interfaces_mod_changes_ctx_t mod_ctx;

    // operational data
    interfaces_oper_ctx_t oper_ctx;

    // features enabled on plugin load
    interfaces_features_ctx_t features;
};

#endif // INTERFACES_PLUGIN_CONTEXT_H