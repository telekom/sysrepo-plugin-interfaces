#ifndef INTERFACES_PLUGIN_SUBSCRIPTION_CHANGE_H
#define INTERFACES_PLUGIN_SUBSCRIPTION_CHANGE_H

#include <sysrepo_types.h>

int interfaces_subscription_change_interfaces_interface(sr_session_ctx_t* session, uint32_t subscription_id, const char* module_name, const char* xpath, sr_event_t event, uint32_t request_id, void* private_data);

#endif // INTERFACES_PLUGIN_SUBSCRIPTION_CHANGE_H