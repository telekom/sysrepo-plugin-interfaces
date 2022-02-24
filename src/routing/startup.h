#ifndef ROUTING_PLUGIN_STARTUP_H
#define ROUTING_PLUGIN_STARTUP_H

#include <routing/context.h>

// loading data when the running DS is empty and storing it in startup DS
int routing_startup_load_data(struct routing_ctx *ctx, sr_session_ctx_t *session);

#endif // ROUTING_PLUGIN_STARTUP_H