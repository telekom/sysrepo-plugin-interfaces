#ifndef ROUTING_PLUGIN_SUBSCRIPTION_OPERATIONAL_H
#define ROUTING_PLUGIN_SUBSCRIPTION_OPERATIONAL_H

#include <sysrepo_types.h>

int routing_oper_get_rib_routes_cb(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *path, const char *request_xpath, uint32_t request_id, struct lyd_node **parent, void *private_data);
int routing_oper_get_interfaces_cb(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *path, const char *request_xpath, uint32_t request_id, struct lyd_node **parent, void *private_data);

#endif // ROUTING_PLUGIN_SUBSCRIPTION_OPERATIONAL_H
