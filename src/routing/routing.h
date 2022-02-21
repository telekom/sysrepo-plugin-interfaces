#ifndef ROUTING_PLUGIN_H
#define ROUTING_PLUGIN_H

#include <sysrepo.h>

#define PLUGIN_NAME "routing plugin"

int routing_sr_plugin_init_cb(sr_session_ctx_t *session, void **private_data);
void routing_sr_plugin_cleanup_cb(sr_session_ctx_t *session, void *private_data);

#endif // ROUTING_PLUGIN_H