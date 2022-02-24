#ifndef ROUTING_PLUGIN_SUBSCRIPTION_RPC_H
#define ROUTING_PLUGIN_SUBSCRIPTION_RPC_H

#include <sysrepo_types.h>

int routing_rpc_active_route_cb(sr_session_ctx_t *session, uint32_t subscription_id, const char *op_path, const sr_val_t *input, const size_t input_cnt, sr_event_t event, uint32_t request_id, sr_val_t **output, size_t *output_cnt, void *private_data);

#endif // ROUTING_PLUGIN_SUBSCRIPTION_RPC_H
