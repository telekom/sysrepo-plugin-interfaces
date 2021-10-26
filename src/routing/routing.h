#ifndef ROUTING_PLUGIN_H
#define ROUTING_PLUGIN_H

#include <sysrepo.h>

int sr_plugin_init_cb(sr_session_ctx_t *session, void **private_data);
void sr_plugin_cleanup_cb(sr_session_ctx_t *session, void *private_data);

#endif // ROUTING_PLUGIN_H