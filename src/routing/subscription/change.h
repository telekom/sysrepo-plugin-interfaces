#ifndef ROUTING_PLUGIN_SUBSCRIPTION_CHANGE_H
#define ROUTING_PLUGIN_SUBSCRIPTION_CHANGE_H

#include <sysrepo_types.h>

int routing_module_change_cb(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data);

#endif // ROUTING_PLUGIN_SUBSCRIPTION_CHANGE_H