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
    char change_xpath_buffer[PATH_MAX] = { 0 };
    int rc = 0;

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
        // name
        SRPC_SAFE_CALL_ERR_COND(rc, rc < 0, snprintf(change_xpath_buffer, sizeof(change_xpath_buffer), "%s/name", xpath), error_out);
        SRPC_SAFE_CALL_ERR(rc, srpc_iterate_changes(ctx, session, change_xpath_buffer, interfaces_interface_change_name, interfaces_change_interface_init, interfaces_change_interface_free), error_out);

        // description
        SRPC_SAFE_CALL_ERR_COND(rc, rc < 0, snprintf(change_xpath_buffer, sizeof(change_xpath_buffer), "%s/description", xpath), error_out);
        SRPC_SAFE_CALL_ERR(rc, srpc_iterate_changes(ctx, session, change_xpath_buffer, interfaces_interface_change_description, interfaces_change_interface_init, interfaces_change_interface_free), error_out);

        // type
        SRPC_SAFE_CALL_ERR_COND(rc, rc < 0, snprintf(change_xpath_buffer, sizeof(change_xpath_buffer), "%s/type", xpath), error_out);
        SRPC_SAFE_CALL_ERR(rc, srpc_iterate_changes(ctx, session, change_xpath_buffer, interfaces_interface_change_type, interfaces_change_interface_init, interfaces_change_interface_free), error_out);

        // enabled
        SRPC_SAFE_CALL_ERR_COND(rc, rc < 0, snprintf(change_xpath_buffer, sizeof(change_xpath_buffer), "%s/enabled", xpath), error_out);
        SRPC_SAFE_CALL_ERR(rc, srpc_iterate_changes(ctx, session, change_xpath_buffer, interfaces_interface_change_enabled, interfaces_change_interface_init, interfaces_change_interface_free), error_out);

        // link-up-down-trap-enable
        SRPC_SAFE_CALL_ERR_COND(rc, rc < 0, snprintf(change_xpath_buffer, sizeof(change_xpath_buffer), "%s/link-up-down-trap-enable", xpath), error_out);
        SRPC_SAFE_CALL_ERR(rc, srpc_iterate_changes(ctx, session, change_xpath_buffer, interfaces_interface_change_link_up_down_trap_enable, interfaces_change_interface_init, interfaces_change_interface_free), error_out);
    }

    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}
