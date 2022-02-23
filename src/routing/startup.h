#ifndef ROUTING_PLUGIN_STARTUP_H
#define ROUTING_PLUGIN_STARTUP_H

#include <routing/context.h>

// loading data when the running DS is empty and storing it in startup DS
int routing_startup_load_data(struct routing_ctx *ctx, sr_session_ctx_t *session);
int routing_startup_load_ribs(struct routing_ctx *ctx, sr_session_ctx_t *session, struct lyd_node *routing_container_node);
int routing_startup_load_control_plane_protocols(struct routing_ctx *ctx, sr_session_ctx_t *session, struct lyd_node *routing_container_node);

#endif // ROUTING_PLUGIN_STARTUP_H