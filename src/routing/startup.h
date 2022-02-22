#ifndef ROUTING_PLUGIN_STARTUP_H
#define ROUTING_PLUGIN_STARTUP_H

#include <routing/context.h>

int routing_load_data(struct routing_ctx *ctx, sr_session_ctx_t *session);
int routing_load_ribs(struct routing_ctx *ctx, sr_session_ctx_t *session, struct lyd_node *routing_container_node);
int routing_load_control_plane_protocols(struct routing_ctx *ctx, sr_session_ctx_t *session, struct lyd_node *routing_container_node);

#endif // ROUTING_PLUGIN_STARTUP_H