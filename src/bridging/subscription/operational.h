#ifndef BRIDGING_PLUGIN_SUBSCRIPTION_OPERATIONAL_H
#define BRIDGING_PLUGIN_SUBSCRIPTION_OPERATIONAL_H

#include <sysrepo_types.h>

int bridging_oper_get_bridges(sr_session_ctx_t *session, uint32_t sub_id, const char *module_name, const char *path, const char *request_xpath, uint32_t request_id, struct lyd_node **parent, void *private_data);

#endif // BRIDGING_PLUGIN_SUBSCRIPTION_OPERATIONAL_H