#ifndef BRIDGING_PLUGIN_SUBSCRIPTION_CHANGE_H
#define BRIDGING_PLUGIN_SUBSCRIPTION_CHANGE_H

#include <sysrepo_types.h>

int bridging_bridge_list_change_cb(sr_session_ctx_t* session, uint32_t subscription_id, const char* module_name, const char* xpath, sr_event_t event, uint32_t request_id, void* private_data);

int bridge_port_change_cb(sr_session_ctx_t* session, uint32_t subscription_id, const char* module_name, const char* xpath, sr_event_t event, uint32_t request_id, void* private_data);

#endif // BRIDGING_PLUGIN_SUBSCRIPTION_CHANGE_H
