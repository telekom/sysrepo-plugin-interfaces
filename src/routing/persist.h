#ifndef ROUTING_PLUGIN_PERSIST_H
#define ROUTING_PLUGIN_PERSIST_H

#include <sysrepo_types.h>

int routing_persist_static_routes(sr_session_ctx_t *session);

#endif // ROUTING_PLUGIN_PERSIST_H