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
        error = srpc_iterate_changes(ctx, session, xpath, interfaces_change_interface, interfaces_change_interface_init, interfaces_change_interface_free);
        if (error) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "srpc_iterate_changes() for interfaces_change_interface failed: %d", error);
            goto error_out;
        }
    }

    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return SR_ERR_CALLBACK_FAILED;
}
