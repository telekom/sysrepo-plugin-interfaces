#include "operational.h"
#include "libyang/tree_data.h"
#include "netlink/addr.h"
#include "netlink/cache.h"
#include "netlink/route/tc.h"
#include "netlink/socket.h"
#include "plugin/common.h"
#include "plugin/context.h"
#include "plugin/ly_tree.h"
#include "srpc/common.h"
#include "sysrepo_types.h"

#include <assert.h>
#include <libyang/libyang.h>
#include <srpc.h>
#include <sysrepo.h>
#include <sysrepo/xpath.h>

#include <linux/netdevice.h>
#include <netlink/route/link.h>
#include <netlink/route/qdisc.h>

static int interfaces_extract_interface_name(sr_session_ctx_t* session, const char* xpath, char* buffer, size_t buffer_size);

int interfaces_subscription_operational_interfaces_interface_admin_status(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
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

int interfaces_subscription_operational_interfaces_interface_oper_status(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;

    // context
    const struct ly_ctx* ly_ctx = NULL;
    interfaces_ctx_t* ctx = private_data;
    interfaces_nl_ctx_t* nl_ctx = &ctx->nl_ctx;

    // buffers
    char interface_name_buffer[100] = { 0 };

    // libnl
    struct rtnl_link* link = NULL;

    const char* operstate_map[] = {
        [IF_OPER_UNKNOWN] = "unknown",
        [IF_OPER_NOTPRESENT] = "not-present",
        [IF_OPER_DOWN] = "down",
        [IF_OPER_LOWERLAYERDOWN] = "lower-layer-down",
        [IF_OPER_TESTING] = "testing",
        [IF_OPER_DORMANT] = "dormant",
        [IF_OPER_UP] = "up",
    };

    // there needs to be an allocated link cache in memory
    assert(ctx->nl_ctx.link_cache != NULL);
    assert(*parent != NULL);
    assert(strcmp(LYD_NAME(*parent), "interface") == 0);

    // extract interface name
    SRPC_SAFE_CALL_ERR(error, interfaces_extract_interface_name(session, request_xpath, interface_name_buffer, sizeof(interface_name_buffer)), error_out);

    // get link by name
    SRPC_SAFE_CALL_PTR(link, rtnl_link_get_by_name(nl_ctx->link_cache, interface_name_buffer), error_out);

    SRPLG_LOG_INF(PLUGIN_NAME, "Getting oper-status(%s)", interface_name_buffer);

    // get oper status
    const uint8_t oper_status = rtnl_link_get_operstate(link);
    const char* oper_status_str = operstate_map[oper_status];

    SRPLG_LOG_INF(PLUGIN_NAME, "oper-status(%s) = %s", interface_name_buffer, oper_status_str);

    // add oper-status node
    SRPC_SAFE_CALL_ERR(error, interfaces_ly_tree_create_interfaces_interface_oper_status(ly_ctx, *parent, oper_status_str), error_out);

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

    // context
    const struct ly_ctx* ly_ctx = NULL;
    interfaces_ctx_t* ctx = private_data;
    interfaces_nl_ctx_t* nl_ctx = &ctx->nl_ctx;

    // buffers
    char interface_name_buffer[100] = { 0 };
    char ifindex_buffer[100] = { 0 };

    // libnl
    struct rtnl_link* link = NULL;

    // there needs to be an allocated link cache in memory
    assert(ctx->nl_ctx.link_cache != NULL);
    assert(*parent != NULL);
    assert(strcmp(LYD_NAME(*parent), "interface") == 0);

    // extract interface name
    SRPC_SAFE_CALL_ERR(error, interfaces_extract_interface_name(session, request_xpath, interface_name_buffer, sizeof(interface_name_buffer)), error_out);

    // get link by name
    SRPC_SAFE_CALL_PTR(link, rtnl_link_get_by_name(nl_ctx->link_cache, interface_name_buffer), error_out);

    SRPLG_LOG_INF(PLUGIN_NAME, "Getting if-index(%s)", interface_name_buffer);

    // get if-index
    const int ifindex = rtnl_link_get_ifindex(link);

    error = snprintf(ifindex_buffer, sizeof(ifindex_buffer), "%d", ifindex);
    if (error < 0) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() failed (%d)", error);
        goto error_out;
    }

    SRPLG_LOG_INF(PLUGIN_NAME, "if-index(%s) = %s", interface_name_buffer, ifindex_buffer);

    // add ifindex node
    SRPC_SAFE_CALL_ERR(error, interfaces_ly_tree_create_interfaces_interface_if_index(ly_ctx, *parent, ifindex_buffer), error_out);

    error = 0;
    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}

int interfaces_subscription_operational_interfaces_interface_phys_address(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    void* error_ptr = NULL;

    // context
    const struct ly_ctx* ly_ctx = NULL;
    interfaces_ctx_t* ctx = private_data;
    interfaces_nl_ctx_t* nl_ctx = &ctx->nl_ctx;

    // buffers
    char interface_name_buffer[100] = { 0 };
    char phys_address_buffer[100] = { 0 };

    // libnl
    struct rtnl_link* link = NULL;
    struct nl_addr* addr = NULL;

    // there needs to be an allocated link cache in memory
    assert(ctx->nl_ctx.link_cache != NULL);
    assert(*parent != NULL);
    assert(strcmp(LYD_NAME(*parent), "interface") == 0);

    // extract interface name
    SRPC_SAFE_CALL_ERR(error, interfaces_extract_interface_name(session, request_xpath, interface_name_buffer, sizeof(interface_name_buffer)), error_out);

    // get link by name
    SRPC_SAFE_CALL_PTR(link, rtnl_link_get_by_name(nl_ctx->link_cache, interface_name_buffer), error_out);

    SRPLG_LOG_INF(PLUGIN_NAME, "Getting phys-address(%s)", interface_name_buffer);

    // get phys-address
    SRPC_SAFE_CALL_PTR(addr, rtnl_link_get_addr(link), error_out);
    SRPC_SAFE_CALL_PTR(error_ptr, nl_addr2str(addr, phys_address_buffer, sizeof(phys_address_buffer)), error_out);

    SRPLG_LOG_INF(PLUGIN_NAME, "phys-address(%s) = %s", interface_name_buffer, phys_address_buffer);

    // add phys-address node
    SRPC_SAFE_CALL_ERR(error, interfaces_ly_tree_create_interfaces_interface_phys_address(ly_ctx, *parent, phys_address_buffer), error_out);

    error = 0;
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
    // void* error_ptr = NULL;

    // context
    const struct ly_ctx* ly_ctx = NULL;
    interfaces_ctx_t* ctx = private_data;
    interfaces_nl_ctx_t* nl_ctx = &ctx->nl_ctx;

    // buffers
    char interface_name_buffer[100] = { 0 };
    char speed_buffer[100] = { 0 };

    // libnl
    struct rtnl_link* link = NULL;
    struct rtnl_qdisc* qdisc = NULL;
    struct rtnl_tc* tc = NULL;

    // there needs to be an allocated link cache in memory
    assert(ctx->nl_ctx.link_cache != NULL);
    assert(*parent != NULL);
    assert(strcmp(LYD_NAME(*parent), "interface") == 0);

    // extract interface name
    SRPC_SAFE_CALL_ERR(error, interfaces_extract_interface_name(session, request_xpath, interface_name_buffer, sizeof(interface_name_buffer)), error_out);

    // get link by name
    SRPC_SAFE_CALL_PTR(link, rtnl_link_get_by_name(nl_ctx->link_cache, interface_name_buffer), error_out);

    SRPLG_LOG_INF(PLUGIN_NAME, "Getting speed(%s)", interface_name_buffer);

    qdisc = rtnl_qdisc_alloc();

    // setup traffic control
    tc = TC_CAST(qdisc);
    rtnl_tc_set_link(tc, link);

    // get speed
    const uint64_t speed = rtnl_tc_get_stat(tc, RTNL_TC_RATE_BPS);
    error = snprintf(speed_buffer, sizeof(speed_buffer), "%lu", speed);
    if (error < 0) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() failed (%d)", error);
        goto error_out;
    }

    SRPLG_LOG_INF(PLUGIN_NAME, "speed(%s) = %s", interface_name_buffer, speed_buffer);

    // add phys-address node
    SRPC_SAFE_CALL_ERR(error, interfaces_ly_tree_create_interfaces_interface_speed(ly_ctx, *parent, speed_buffer), error_out);

    error = 0;
    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    rtnl_qdisc_put(qdisc);

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

int interfaces_subscription_operational_interfaces_interface_statistics_in_discard_unknown_encaps(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
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

int interfaces_subscription_operational_interfaces_interface_carrier_delay_carrier_transitions(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
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

int interfaces_subscription_operational_interfaces_interface_carrier_delay_timer_running(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
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

int interfaces_subscription_operational_interfaces_interface_dampening_penalty(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
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

int interfaces_subscription_operational_interfaces_interface_dampening_suppressed(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
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

int interfaces_subscription_operational_interfaces_interface_dampening_time_remaining(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
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

int interfaces_subscription_operational_interfaces_interface_forwarding_mode(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
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

    // setup nl socket
    if (!nl_ctx->socket) {
        // netlink
        SRPC_SAFE_CALL_PTR(nl_ctx->socket, nl_socket_alloc(), error_out);

        // connect
        SRPC_SAFE_CALL_ERR(error, nl_connect(nl_ctx->socket, NETLINK_ROUTE), error_out);
    }

    // cache was already allocated - free existing cache
    if (nl_ctx->link_cache) {
        nl_cache_refill(nl_ctx->socket, nl_ctx->link_cache);
    } else {
        // allocate new link cache
        SRPC_SAFE_CALL_ERR(error, rtnl_link_alloc_cache(nl_ctx->socket, 0, &nl_ctx->link_cache), error_out);
    }

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

static int interfaces_extract_interface_name(sr_session_ctx_t* session, const char* xpath, char* buffer, size_t buffer_size)
{
    int error = 0;

    const char* name = NULL;

    sr_xpath_ctx_t xpath_ctx = { 0 };

    // extract key
    SRPC_SAFE_CALL_PTR(name, sr_xpath_key_value((char*)xpath, "interface", "name", &xpath_ctx), error_out);

    // store to buffer
    error = snprintf(buffer, buffer_size, "%s", name);
    if (error < 0) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() failed");
        goto error_out;
    }

    error = 0;
    goto out;

error_out:
    error = -1;

out:
    return error;
}