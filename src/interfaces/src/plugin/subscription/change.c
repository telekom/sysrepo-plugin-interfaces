#include "change.h"
#include "plugin/common.h"
#include "plugin/context.h"

#include <libyang/libyang.h>
#include <linux/limits.h>
#include <srpc.h>
#include <sysrepo.h>

// change API
#include "plugin/api/interfaces/change.h"
#include "plugin/api/interfaces/interface/change.h"
#include "plugin/api/interfaces/interface/ipv4/address/change.h"
#include "plugin/api/interfaces/interface/ipv4/change.h"
#include "plugin/api/interfaces/interface/ipv4/neighbor/change.h"
#include "plugin/api/interfaces/interface/ipv6/address/change.h"
#include "plugin/api/interfaces/interface/ipv6/neighbor/change.h"

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
        // when all changes processed - copy running DS contents to startup
        SRPC_SAFE_CALL_ERR(error, sr_copy_config(ctx->startup_session, IETF_INTERFACES_YANG_MODULE, SR_DS_RUNNING, 0), error_out);
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

        // ipv4/mtu
        SRPC_SAFE_CALL_ERR_COND(rc, rc < 0, snprintf(change_xpath_buffer, sizeof(change_xpath_buffer), "%s/ipv4/mtu", xpath), error_out);
        SRPC_SAFE_CALL_ERR(rc, srpc_iterate_changes(ctx, session, change_xpath_buffer, interfaces_interface_ipv4_change_mtu, interfaces_change_interface_init, interfaces_change_interface_free), error_out);

        // ipv4/enabled
        SRPC_SAFE_CALL_ERR_COND(rc, rc < 0, snprintf(change_xpath_buffer, sizeof(change_xpath_buffer), "%s/ipv4/enabled", xpath), error_out);
        SRPC_SAFE_CALL_ERR(rc, srpc_iterate_changes(ctx, session, change_xpath_buffer, interfaces_interface_ipv4_change_enabled, interfaces_change_interface_init, interfaces_change_interface_free), error_out);

        // ipv4/address/ip
        SRPC_SAFE_CALL_ERR_COND(rc, rc < 0, snprintf(change_xpath_buffer, sizeof(change_xpath_buffer), "%s/ipv4/address/ip", xpath), error_out);
        SRPC_SAFE_CALL_ERR(rc, srpc_iterate_changes(ctx, session, change_xpath_buffer, interfaces_interface_ipv4_address_change_ip, interfaces_change_interface_init, interfaces_change_interface_free), error_out);

        // ipv4/address/prefix-length
        SRPC_SAFE_CALL_ERR_COND(rc, rc < 0, snprintf(change_xpath_buffer, sizeof(change_xpath_buffer), "%s/ipv4/address/prefix-length", xpath), error_out);
        SRPC_SAFE_CALL_ERR(rc, srpc_iterate_changes(ctx, session, change_xpath_buffer, interfaces_interface_ipv4_address_change_prefix_length, interfaces_change_interface_init, interfaces_change_interface_free), error_out);

        // ipv4/address/netmask
        SRPC_SAFE_CALL_ERR_COND(rc, rc < 0, snprintf(change_xpath_buffer, sizeof(change_xpath_buffer), "%s/ipv4/address/netmask", xpath), error_out);
        SRPC_SAFE_CALL_ERR(rc, srpc_iterate_changes(ctx, session, change_xpath_buffer, interfaces_interface_ipv4_address_change_netmask, interfaces_change_interface_init, interfaces_change_interface_free), error_out);

        // ipv4/neighbor/ip
        SRPC_SAFE_CALL_ERR_COND(rc, rc < 0, snprintf(change_xpath_buffer, sizeof(change_xpath_buffer), "%s/ipv4/neighbor/ip", xpath), error_out);
        SRPC_SAFE_CALL_ERR(rc, srpc_iterate_changes(ctx, session, change_xpath_buffer, interfaces_interface_ipv4_neighbor_change_ip, interfaces_change_interface_init, interfaces_change_interface_free), error_out);

        // ipv4/neighbor/link-layer-address
        SRPC_SAFE_CALL_ERR_COND(rc, rc < 0, snprintf(change_xpath_buffer, sizeof(change_xpath_buffer), "%s/ipv4/neighbor/link-layer-address", xpath), error_out);
        SRPC_SAFE_CALL_ERR(rc, srpc_iterate_changes(ctx, session, change_xpath_buffer, interfaces_interface_ipv4_neighbor_change_link_layer_address, interfaces_change_interface_init, interfaces_change_interface_free), error_out);

        // ipv6/address/ip
        SRPC_SAFE_CALL_ERR_COND(rc, rc < 0, snprintf(change_xpath_buffer, sizeof(change_xpath_buffer), "%s/ipv6/address/ip", xpath), error_out);
        SRPC_SAFE_CALL_ERR(rc, srpc_iterate_changes(ctx, session, change_xpath_buffer, interfaces_interface_ipv6_address_change_ip, interfaces_change_interface_init, interfaces_change_interface_free), error_out);

        // ipv6/address/prefix-length
        SRPC_SAFE_CALL_ERR_COND(rc, rc < 0, snprintf(change_xpath_buffer, sizeof(change_xpath_buffer), "%s/ipv6/address/prefix-length", xpath), error_out);
        SRPC_SAFE_CALL_ERR(rc, srpc_iterate_changes(ctx, session, change_xpath_buffer, interfaces_interface_ipv6_address_change_prefix_length, interfaces_change_interface_init, interfaces_change_interface_free), error_out);
    }

    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}
