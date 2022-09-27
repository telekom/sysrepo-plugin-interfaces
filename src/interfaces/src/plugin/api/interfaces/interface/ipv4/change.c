#include "change.h"
#include "libyang/tree_data.h"
#include "netlink/route/link.h"
#include "plugin/common.h"
#include "plugin/context.h"
#include "sysrepo_types.h"

#include <linux/limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <sysrepo.h>

int interfaces_interface_ipv4_change_neighbor_init(void* priv)
{
    int error = 0;
    return error;
}

int interfaces_interface_ipv4_change_neighbor(void* priv, sr_session_ctx_t* session, const srpc_change_ctx_t* change_ctx)
{
    int error = 0;
    const char* node_name = LYD_NAME(change_ctx->node);
    const char* node_value = lyd_get_value(change_ctx->node);

    SRPLG_LOG_DBG(PLUGIN_NAME, "Node Name: %s; Previous Value: %s, Value: %s; Operation: %d", node_name, change_ctx->previous_value, node_value, change_ctx->operation);

    switch (change_ctx->operation) {
    case SR_OP_CREATED:
        break;
    case SR_OP_MODIFIED:
        break;
    case SR_OP_DELETED:
        break;
    case SR_OP_MOVED:
        break;
    }

    return error;
}

void interfaces_interface_ipv4_change_neighbor_free(void* priv)
{
}

int interfaces_interface_ipv4_change_address_init(void* priv)
{
    int error = 0;
    return error;
}

int interfaces_interface_ipv4_change_address(void* priv, sr_session_ctx_t* session, const srpc_change_ctx_t* change_ctx)
{
    int error = 0;
    const char* node_name = LYD_NAME(change_ctx->node);
    const char* node_value = lyd_get_value(change_ctx->node);

    SRPLG_LOG_DBG(PLUGIN_NAME, "Node Name: %s; Previous Value: %s, Value: %s; Operation: %d", node_name, change_ctx->previous_value, node_value, change_ctx->operation);

    switch (change_ctx->operation) {
    case SR_OP_CREATED:
        break;
    case SR_OP_MODIFIED:
        break;
    case SR_OP_DELETED:
        break;
    case SR_OP_MOVED:
        break;
    }

    return error;
}

void interfaces_interface_ipv4_change_address_free(void* priv)
{
}

int interfaces_interface_ipv4_change_mtu_init(void* priv)
{
    int error = 0;
    return error;
}

int interfaces_interface_ipv4_change_mtu(void* priv, sr_session_ctx_t* session, const srpc_change_ctx_t* change_ctx)
{
    int error = SR_ERR_OK;
    void* error_ptr = NULL;
    const char* node_name = LYD_NAME(change_ctx->node);
    const char* node_value = lyd_get_value(change_ctx->node);
    char path_buffer[PATH_MAX] = { 0 };
    char interface_name_buffer[100] = { 0 };

    // app context
    interfaces_ctx_t* ctx = priv;

    // mod changes context
    interfaces_mod_changes_ctx_t* mod_ctx = &ctx->mod_ctx;

    // libnl
    struct rtnl_link* current_link = NULL;
    struct rtnl_link* request_link = NULL;

    SRPLG_LOG_INF(PLUGIN_NAME, "Node Name: %s; Previous Value: %s, Value: %s; Operation: %d", node_name, change_ctx->previous_value, node_value, change_ctx->operation);

    // get node path
    SRPC_SAFE_CALL_PTR(error_ptr, lyd_path(change_ctx->node, LYD_PATH_STD, path_buffer, sizeof(path_buffer)), error_out);

    // get interface name
    SRPC_SAFE_CALL_ERR(error, srpc_extract_xpath_key_value(path_buffer, "interface", "name", interface_name_buffer, sizeof(interface_name_buffer)), error_out);

    SRPLG_LOG_INF(PLUGIN_NAME, "Node Path: %s; Interface Name: %s", path_buffer, interface_name_buffer);

    // get link
    SRPC_SAFE_CALL_PTR(current_link, rtnl_link_get_by_name(mod_ctx->nl_ctx.link_cache, interface_name_buffer), error_out);

    // create request
    request_link = rtnl_link_alloc();
    rtnl_link_set_name(request_link, rtnl_link_get_name(current_link));
    SRPC_SAFE_CALL_ERR(error, rtnl_link_set_type(request_link, rtnl_link_get_type(current_link)), error_out);

    switch (change_ctx->operation) {
    case SR_OP_CREATED:
    case SR_OP_MODIFIED: {
        // convert to int
        uint16_t mtu = (uint16_t)atoi(node_value);

        // set data
        rtnl_link_set_mtu(request_link, mtu);
        break;
    }
    case SR_OP_DELETED:
        // set default MTU
        rtnl_link_set_mtu(request_link, 1500);
        break;
    case SR_OP_MOVED:
        break;
    }

    // apply changes
    SRPC_SAFE_CALL_ERR(error, rtnl_link_change(mod_ctx->nl_ctx.socket, current_link, request_link, 0), error_out);

    goto out;

error_out:
    if (error < 0) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "nl_geterror(): %s", nl_geterror(error));
    }
    error = -1;

out:
    return error;
}

void interfaces_interface_ipv4_change_mtu_free(void* priv)
{
}

int interfaces_interface_ipv4_change_forwarding_init(void* priv)
{
    int error = 0;
    return error;
}

int interfaces_interface_ipv4_change_forwarding(void* priv, sr_session_ctx_t* session, const srpc_change_ctx_t* change_ctx)
{
    int error = 0;
    const char* node_name = LYD_NAME(change_ctx->node);
    const char* node_value = lyd_get_value(change_ctx->node);

    SRPLG_LOG_DBG(PLUGIN_NAME, "Node Name: %s; Previous Value: %s, Value: %s; Operation: %d", node_name, change_ctx->previous_value, node_value, change_ctx->operation);

    switch (change_ctx->operation) {
    case SR_OP_CREATED:
        break;
    case SR_OP_MODIFIED:
        break;
    case SR_OP_DELETED:
        break;
    case SR_OP_MOVED:
        break;
    }

    return error;
}

void interfaces_interface_ipv4_change_forwarding_free(void* priv)
{
}

int interfaces_interface_ipv4_change_enabled_init(void* priv)
{
    int error = 0;
    return error;
}

int interfaces_interface_ipv4_change_enabled(void* priv, sr_session_ctx_t* session, const srpc_change_ctx_t* change_ctx)
{
    int error = 0;
    const char* node_name = LYD_NAME(change_ctx->node);
    const char* node_value = lyd_get_value(change_ctx->node);

    SRPLG_LOG_DBG(PLUGIN_NAME, "Node Name: %s; Previous Value: %s, Value: %s; Operation: %d", node_name, change_ctx->previous_value, node_value, change_ctx->operation);

    switch (change_ctx->operation) {
    case SR_OP_CREATED:
        break;
    case SR_OP_MODIFIED:
        break;
    case SR_OP_DELETED:
        break;
    case SR_OP_MOVED:
        break;
    }

    return error;
}

void interfaces_interface_ipv4_change_enabled_free(void* priv)
{
}
