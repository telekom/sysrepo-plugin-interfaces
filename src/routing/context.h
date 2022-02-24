#ifndef ROUTING_PLUGIN_CONTEXT_H
#define ROUTING_PLUGIN_CONTEXT_H

#include <sysrepo_types.h>

#include "route/list_hash.h"

struct routing_ctx {
	sr_session_ctx_t *startup_session;
};

#endif // ROUTING_PLUGIN_CONTEXT_H