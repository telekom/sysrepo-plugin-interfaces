#ifndef BRIDGING_PLUGIN_STARTUP_H
#define BRIDGING_PLUGIN_STARTUP_H

#include "plugin/context.h"
#include <sysrepo_types.h>

int bridging_startup_load_data(bridging_ctx_t *ctx, sr_session_ctx_t *session);

#endif // BRIDGING_PLUGIN_STARTUP_H
