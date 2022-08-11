#ifndef INTERFACES_PLUGIN_CONTEXT_H
#define INTERFACES_PLUGIN_CONTEXT_H

#include "plugin/types.h"
#include <sysrepo_types.h>

#include <netlink/route/link.h>

// typedefs
typedef struct interfaces_nl_ctx_s interfaces_nl_ctx_t;
typedef struct interfaces_ctx_s interfaces_ctx_t;

struct interfaces_nl_ctx_s {
    struct nl_sock* socket;
    struct nl_cache* link_cache;
};

struct interfaces_ctx_s {
    sr_session_ctx_t* startup_session;
    interfaces_nl_ctx_t nl_ctx;
};

#endif // INTERFACES_PLUGIN_CONTEXT_H