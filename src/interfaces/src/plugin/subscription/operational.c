#include "operational.h"
#include "libyang/tree_data.h"
#include "netlink/cache.h"
#include "netlink/socket.h"
#include "plugin/common.h"
#include "plugin/context.h"
#include "plugin/ly_tree.h"
#include "srpc/common.h"

#include <libyang/libyang.h>
#include <srpc.h>
#include <sysrepo.h>

#include <netlink/route/link.h>

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
    interfaces_ctx_t* ctx = private_data;
    interfaces_nl_ctx_t* nl_ctx = &ctx->nl_ctx;
    struct rtnl_link* link_iter = NULL;

    // libyang
    struct lyd_node* interface_list_node = NULL;

    SRPLG_LOG_INF(PLUGIN_NAME, "Operational callback: %s", request_xpath);

    // setup nl socket
    if (!nl_ctx->socket) {
        // netlink
        SRPC_SAFE_CALL_PTR(nl_ctx->socket, nl_socket_alloc(), error_out);

        // connect
        SRPC_SAFE_CALL_ERR(error, nl_connect(nl_ctx->socket, NETLINK_ROUTE), error_out);
    }

    // cache was already allocated - free existing cache
    if (nl_ctx->link_cache) {
        nl_cache_free(nl_ctx->link_cache);
    }

    // allocate new link cache
    SRPC_SAFE_CALL_ERR(error, rtnl_link_alloc_cache(nl_ctx->socket, 0, &nl_ctx->link_cache), error_out);

    if (*parent == NULL) {
        ly_ctx = sr_acquire_context(sr_session_get_connection(session));
        if (ly_ctx == NULL) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "sr_acquire_context() failed");
            goto error_out;
        }

        // set parent to the interfaces container
        SRPC_SAFE_CALL_ERR(error, interfaces_ly_tree_create_interfaces(ly_ctx, parent), error_out);
    }

    // iterate links and add them to the operational DS
    link_iter = (struct rtnl_link*)nl_cache_get_first(nl_ctx->link_cache);
    while (link_iter) {
        SRPLG_LOG_INF(PLUGIN_NAME, "Interface %s", rtnl_link_get_name(link_iter));

        // add interface
        SRPC_SAFE_CALL_ERR(error, interfaces_ly_tree_create_interfaces_interface(ly_ctx, *parent, &interface_list_node, rtnl_link_get_name(link_iter)), error_out);

        link_iter = (struct rtnl_link*)nl_cache_get_next((struct nl_object*)link_iter);
    }

    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:

    return error;
}