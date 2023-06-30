#ifndef BRIDGING_PLUGIN_CONTEXT_H
#define BRIDGING_PLUGIN_CONTEXT_H

#include <sysrepo_types.h>

typedef struct bridging_ctx_s {
	sr_session_ctx_t *startup_session;
} bridging_ctx_t;

#endif // BRIDGING_PLUGIN_CONTEXT_H