#ifndef INTERFACES_PLUGIN_CONTEXT_H
#define INTERFACES_PLUGIN_CONTEXT_H

#include "netlink/cache.h"
#include "plugin/types.h"
#include <pthread.h>
#include <sysrepo_types.h>

#include <netlink/route/link.h>

// typedefs
typedef struct interfaces_nl_ctx_s interfaces_nl_ctx_t;
typedef struct interfaces_ctx_s interfaces_ctx_t;
typedef struct interfaces_state_changes_ctx_s interfaces_state_changes_ctx_t;

struct interfaces_state_changes_ctx_s {
    // libnl data
    struct nl_sock* socket;
    struct nl_cache* link_cache;
    struct nl_cache_mngr* link_cache_manager;
    pthread_t manager_thread;

    // main hash DS for storing state info
    interfaces_interface_state_t* state_hash;

    // mutex for accessing state hash data
    pthread_mutex_t state_hash_mutex;
};

struct interfaces_nl_ctx_s {
    struct nl_sock* socket;
    struct nl_cache* link_cache;
};

struct interfaces_ctx_s {
    sr_session_ctx_t* startup_session;
    interfaces_nl_ctx_t nl_ctx;
    interfaces_state_changes_ctx_t state_ctx;
};

#endif // INTERFACES_PLUGIN_CONTEXT_H