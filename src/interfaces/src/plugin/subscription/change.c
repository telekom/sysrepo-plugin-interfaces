#include "change.h"
#include "plugin/common.h"
#include "plugin/context.h"

#include <libyang/libyang.h>
#include <srpc.h>
#include <sysrepo.h>

// change API
#include "plugin/api/interfaces/change.h"
#include "plugin/api/interfaces/interface/change.h"

int interfaces_subscription_change_interfaces_interface(sr_session_ctx_t* session, uint32_t subscription_id, const char* module_name, const char* xpath, sr_event_t event, uint32_t request_id, void* private_data)
{
    int error = SR_ERR_OK;
    interfaces_ctx_t* ctx = (interfaces_ctx_t*)private_data;

    // sysrepo
    sr_change_iter_t* changes_iterator = NULL;
    sr_change_oper_t operation = SR_OP_CREATED;
    const char *prev_value = NULL, *prev_list = NULL;
    int prev_default;

    const char* node_name = NULL;
    const char* node_value = NULL;

    // libyang
    const struct lyd_node* node = NULL;

    if (event == SR_EV_ABORT) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "Aborting changes for %s", xpath);
        goto error_out;
    } else if (event == SR_EV_DONE) {
        error = sr_copy_config(ctx->startup_session, BASE_YANG_MODEL, SR_DS_RUNNING, 0);
        if (error) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "sr_copy_config() error (%d): %s", error, sr_strerror(error));
            goto error_out;
        }
    } else if (event == SR_EV_CHANGE) {
        // connect change API
        error = srpc_iterate_changes(ctx, session, xpath, interfaces_change_interface);
        if (error) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "srpc_iterate_changes() for interfaces_change_interface failed: %d", error);
            goto error_out;
        }
    }

    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}
