#include "operational.h"
#include "plugin/common.h"
#include "plugin/context.h"

#include <libyang/libyang.h>
#include <srpc.h>
#include <sysrepo.h>

int interfaces_subscription_operational_interfaces_interface_admin_status(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    const struct ly_ctx* ly_ctx = NULL;

    SRPLG_LOG_INF(PLUGIN_NAME, "Operational XPath: %s", request_xpath);

    if (*parent == NULL) {
        ly_ctx = sr_acquire_context(sr_session_get_connection(session));
        if (ly_ctx == NULL) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "sr_acquire_context() failed");
            goto error_out;
        }
    }

    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}

int interfaces_subscription_operational_interfaces_interface_oper_status(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    const struct ly_ctx* ly_ctx = NULL;

    if (*parent == NULL) {
        ly_ctx = sr_acquire_context(sr_session_get_connection(session));
        if (ly_ctx == NULL) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "sr_acquire_context() failed");
            goto error_out;
        }
    }

    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}

int interfaces_subscription_operational_interfaces_interface_last_change(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    const struct ly_ctx* ly_ctx = NULL;

    if (*parent == NULL) {
        ly_ctx = sr_acquire_context(sr_session_get_connection(session));
        if (ly_ctx == NULL) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "sr_acquire_context() failed");
            goto error_out;
        }
    }

    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}

int interfaces_subscription_operational_interfaces_interface_if_index(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    const struct ly_ctx* ly_ctx = NULL;

    if (*parent == NULL) {
        ly_ctx = sr_acquire_context(sr_session_get_connection(session));
        if (ly_ctx == NULL) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "sr_acquire_context() failed");
            goto error_out;
        }
    }

    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}

int interfaces_subscription_operational_interfaces_interface_phys_address(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    const struct ly_ctx* ly_ctx = NULL;

    if (*parent == NULL) {
        ly_ctx = sr_acquire_context(sr_session_get_connection(session));
        if (ly_ctx == NULL) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "sr_acquire_context() failed");
            goto error_out;
        }
    }

    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}

int interfaces_subscription_operational_interfaces_interface_higher_layer_if(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    const struct ly_ctx* ly_ctx = NULL;

    if (*parent == NULL) {
        ly_ctx = sr_acquire_context(sr_session_get_connection(session));
        if (ly_ctx == NULL) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "sr_acquire_context() failed");
            goto error_out;
        }
    }

    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}

int interfaces_subscription_operational_interfaces_interface_lower_layer_if(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    const struct ly_ctx* ly_ctx = NULL;

    if (*parent == NULL) {
        ly_ctx = sr_acquire_context(sr_session_get_connection(session));
        if (ly_ctx == NULL) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "sr_acquire_context() failed");
            goto error_out;
        }
    }

    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}

int interfaces_subscription_operational_interfaces_interface_speed(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    const struct ly_ctx* ly_ctx = NULL;

    if (*parent == NULL) {
        ly_ctx = sr_acquire_context(sr_session_get_connection(session));
        if (ly_ctx == NULL) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "sr_acquire_context() failed");
            goto error_out;
        }
    }

    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}

int interfaces_subscription_operational_interfaces_interface_statistics_discontinuity_time(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    const struct ly_ctx* ly_ctx = NULL;

    if (*parent == NULL) {
        ly_ctx = sr_acquire_context(sr_session_get_connection(session));
        if (ly_ctx == NULL) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "sr_acquire_context() failed");
            goto error_out;
        }
    }

    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}

int interfaces_subscription_operational_interfaces_interface_statistics_in_octets(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    const struct ly_ctx* ly_ctx = NULL;

    if (*parent == NULL) {
        ly_ctx = sr_acquire_context(sr_session_get_connection(session));
        if (ly_ctx == NULL) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "sr_acquire_context() failed");
            goto error_out;
        }
    }

    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}

int interfaces_subscription_operational_interfaces_interface_statistics_in_unicast_pkts(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    const struct ly_ctx* ly_ctx = NULL;

    if (*parent == NULL) {
        ly_ctx = sr_acquire_context(sr_session_get_connection(session));
        if (ly_ctx == NULL) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "sr_acquire_context() failed");
            goto error_out;
        }
    }

    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}

int interfaces_subscription_operational_interfaces_interface_statistics_in_broadcast_pkts(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    const struct ly_ctx* ly_ctx = NULL;

    if (*parent == NULL) {
        ly_ctx = sr_acquire_context(sr_session_get_connection(session));
        if (ly_ctx == NULL) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "sr_acquire_context() failed");
            goto error_out;
        }
    }

    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}

int interfaces_subscription_operational_interfaces_interface_statistics_in_multicast_pkts(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    const struct ly_ctx* ly_ctx = NULL;

    if (*parent == NULL) {
        ly_ctx = sr_acquire_context(sr_session_get_connection(session));
        if (ly_ctx == NULL) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "sr_acquire_context() failed");
            goto error_out;
        }
    }

    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}

int interfaces_subscription_operational_interfaces_interface_statistics_in_discards(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    const struct ly_ctx* ly_ctx = NULL;

    if (*parent == NULL) {
        ly_ctx = sr_acquire_context(sr_session_get_connection(session));
        if (ly_ctx == NULL) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "sr_acquire_context() failed");
            goto error_out;
        }
    }

    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}

int interfaces_subscription_operational_interfaces_interface_statistics_in_errors(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    const struct ly_ctx* ly_ctx = NULL;

    if (*parent == NULL) {
        ly_ctx = sr_acquire_context(sr_session_get_connection(session));
        if (ly_ctx == NULL) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "sr_acquire_context() failed");
            goto error_out;
        }
    }

    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}

int interfaces_subscription_operational_interfaces_interface_statistics_in_unknown_protos(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    const struct ly_ctx* ly_ctx = NULL;

    if (*parent == NULL) {
        ly_ctx = sr_acquire_context(sr_session_get_connection(session));
        if (ly_ctx == NULL) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "sr_acquire_context() failed");
            goto error_out;
        }
    }

    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}

int interfaces_subscription_operational_interfaces_interface_statistics_out_octets(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    const struct ly_ctx* ly_ctx = NULL;

    if (*parent == NULL) {
        ly_ctx = sr_acquire_context(sr_session_get_connection(session));
        if (ly_ctx == NULL) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "sr_acquire_context() failed");
            goto error_out;
        }
    }

    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}

int interfaces_subscription_operational_interfaces_interface_statistics_out_unicast_pkts(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    const struct ly_ctx* ly_ctx = NULL;

    if (*parent == NULL) {
        ly_ctx = sr_acquire_context(sr_session_get_connection(session));
        if (ly_ctx == NULL) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "sr_acquire_context() failed");
            goto error_out;
        }
    }

    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}

int interfaces_subscription_operational_interfaces_interface_statistics_out_broadcast_pkts(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    const struct ly_ctx* ly_ctx = NULL;

    if (*parent == NULL) {
        ly_ctx = sr_acquire_context(sr_session_get_connection(session));
        if (ly_ctx == NULL) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "sr_acquire_context() failed");
            goto error_out;
        }
    }

    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}

int interfaces_subscription_operational_interfaces_interface_statistics_out_multicast_pkts(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    const struct ly_ctx* ly_ctx = NULL;

    if (*parent == NULL) {
        ly_ctx = sr_acquire_context(sr_session_get_connection(session));
        if (ly_ctx == NULL) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "sr_acquire_context() failed");
            goto error_out;
        }
    }

    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}

int interfaces_subscription_operational_interfaces_interface_statistics_out_discards(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    const struct ly_ctx* ly_ctx = NULL;

    if (*parent == NULL) {
        ly_ctx = sr_acquire_context(sr_session_get_connection(session));
        if (ly_ctx == NULL) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "sr_acquire_context() failed");
            goto error_out;
        }
    }

    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}

int interfaces_subscription_operational_interfaces_interface_statistics_out_errors(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    const struct ly_ctx* ly_ctx = NULL;

    if (*parent == NULL) {
        ly_ctx = sr_acquire_context(sr_session_get_connection(session));
        if (ly_ctx == NULL) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "sr_acquire_context() failed");
            goto error_out;
        }
    }

    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}

int interfaces_subscription_operational_interfaces_interface(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    const struct ly_ctx* ly_ctx = NULL;

    if (*parent == NULL) {
        ly_ctx = sr_acquire_context(sr_session_get_connection(session));
        if (ly_ctx == NULL) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "sr_acquire_context() failed");
            goto error_out;
        }
    }

    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}