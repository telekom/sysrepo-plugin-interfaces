#ifndef INTERFACES_PLUGIN_CONTEXT_H
#define INTERFACES_PLUGIN_CONTEXT_H

#include "plugin/types.h"
#include <sysrepo_types.h>

typedef struct interfaces_ctx_s interfaces_ctx_t;

struct interfaces_ctx_s {
    sr_session_ctx_t* startup_session;
};

#endif // INTERFACES_PLUGIN_CONTEXT_H