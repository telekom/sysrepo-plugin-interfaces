#ifndef BRIDGING_PLUGIN_H
#define BRIDGING_PLUGIN_H

#include <sysrepo_types.h>

int bridging_sr_plugin_init_cb(sr_session_ctx_t *session, void **private_data);
void bridging_sr_plugin_cleanup_cb(sr_session_ctx_t *session, void *private_data);

#endif // BRIDGING_PLUGIN_H