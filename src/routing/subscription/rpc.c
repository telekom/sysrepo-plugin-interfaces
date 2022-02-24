#include "rpc.h"
#include <routing/common.h>

#include <sysrepo.h>

int routing_rpc_active_route_cb(sr_session_ctx_t *session, uint32_t subscription_id, const char *op_path, const sr_val_t *input, const size_t input_cnt, sr_event_t event, uint32_t request_id, sr_val_t **output, size_t *output_cnt, void *private_data)
{
	int error = SR_ERR_OK;
	SRPLG_LOG_DBG(PLUGIN_NAME, "xpath for RPC: %s", op_path);
	return error;
}