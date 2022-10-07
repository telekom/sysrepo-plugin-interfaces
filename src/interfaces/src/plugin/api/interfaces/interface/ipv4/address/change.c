#include "change.h"
#include "plugin/common.h"
#include "plugin/context.h"
#include "srpc/common.h"
#include "srpc/types.h"
#include "sysrepo_types.h"

#include <sysrepo.h>

#include <assert.h>

#include <netlink/addr.h>
#include <netlink/route/addr.h>
#include <netlink/route/link.h>

static int interfaces_interface_ipv4_address_get_prefix_length(void* priv, sr_session_ctx_t* session, const srpc_change_ctx_t* change_ctx);
static int interfaces_interface_ipv4_address_get_netmask(void* priv, sr_session_ctx_t* session, const srpc_change_ctx_t* change_ctx);

int interfaces_interface_ipv4_address_change_netmask_init(void* priv)
{
    int error = 0;
    return error;
}

int interfaces_interface_ipv4_address_change_netmask(void* priv, sr_session_ctx_t* session, const srpc_change_ctx_t* change_ctx)
{
    int error = 0;
    const char* node_name = LYD_NAME(change_ctx->node);
    const char* node_value = lyd_get_value(change_ctx->node);

    SRPLG_LOG_INF(PLUGIN_NAME, "Node Name: %s; Previous Value: %s, Value: %s; Operation: %d", node_name, change_ctx->previous_value, node_value, change_ctx->operation);

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

    return -1;
}

void interfaces_interface_ipv4_address_change_netmask_free(void* priv)
{
}

int interfaces_interface_ipv4_address_change_prefix_length_init(void* priv)
{
    int error = 0;
    return error;
}

int interfaces_interface_ipv4_address_change_prefix_length(void* priv, sr_session_ctx_t* session, const srpc_change_ctx_t* change_ctx)
{
    int error = 0;
    void* error_ptr = NULL;

    // sysrepo
    sr_val_t* prefix_val = NULL;
    sr_val_t* netmask_val = NULL;

    // strings and buffers
    const char* node_name = LYD_NAME(change_ctx->node);
    const char* node_value = lyd_get_value(change_ctx->node);
    char path_buffer[PATH_MAX] = { 0 };
    char interface_name_buffer[100] = { 0 };
    char ip_buffer[100] = { 0 };
    char address_buffer[100] = { 0 };

    // app context
    interfaces_ctx_t* ctx = priv;

    // mod changes context
    interfaces_mod_changes_ctx_t* mod_ctx = &ctx->mod_ctx;

    // libnl
    struct rtnl_addr* request_addr = NULL;
    struct rtnl_link* current_link = NULL;
    struct nl_addr* local_addr = NULL;

    SRPLG_LOG_INF(PLUGIN_NAME, "Node Name: %s; Previous Value: %s, Value: %s; Operation: %d", node_name, change_ctx->previous_value, node_value, change_ctx->operation);

    // get node path
    SRPC_SAFE_CALL_PTR(error_ptr, lyd_path(change_ctx->node, LYD_PATH_STD, path_buffer, sizeof(path_buffer)), error_out);

    // get interface name
    SRPC_SAFE_CALL_ERR(error, srpc_extract_xpath_key_value(path_buffer, "interface", "name", interface_name_buffer, sizeof(interface_name_buffer)), error_out);

    // get IP
    SRPC_SAFE_CALL_ERR(error, srpc_extract_xpath_key_value(path_buffer, "address", "ip", ip_buffer, sizeof(ip_buffer)), error_out);

    SRPLG_LOG_INF(PLUGIN_NAME, "Node Path: %s; Interface Name: %s; Address IP: %s", path_buffer, interface_name_buffer, ip_buffer);

    // get link
    SRPC_SAFE_CALL_PTR(current_link, rtnl_link_get_by_name(mod_ctx->nl_ctx.link_cache, interface_name_buffer), error_out);

    switch (change_ctx->operation) {
    case SR_OP_CREATED:
        // this change case should be handled only when creating the whole address in the IP callback
        // if handled in this callback - report an error
        break;
    case SR_OP_MODIFIED:
        // change prefix length

        // get full address
        SRPC_SAFE_CALL_ERR_COND(error, error < 0, snprintf(address_buffer, sizeof(address_buffer), "%s/%s", ip_buffer, node_value), error_out);

        // create local address
        SRPC_SAFE_CALL_ERR(error, nl_addr_parse(address_buffer, AF_INET, &local_addr), error_out);

        // fetch rtnl_address
        break;
    case SR_OP_DELETED:
        // fetch info about prefix-length/netmask and use the pair address/prefix to delete address
        // prefix is needed to find the appropriate address
        break;
    case SR_OP_MOVED:
        break;
    }

    goto out;

error_out:
    if (error < 0) {
        SRPLG_LOG_INF(PLUGIN_NAME, "nl_geterror(): %s", nl_geterror(error));
    }
    error = -1;

out:
    return error;
}

void interfaces_interface_ipv4_address_change_prefix_length_free(void* priv)
{
}

int interfaces_interface_ipv4_address_change_ip_init(void* priv)
{
    int error = 0;
    return error;
}

int interfaces_interface_ipv4_address_change_ip(void* priv, sr_session_ctx_t* session, const srpc_change_ctx_t* change_ctx)
{
    int error = 0;
    void* error_ptr = NULL;

    // sysrepo
    sr_val_t* prefix_val = NULL;
    sr_val_t* netmask_val = NULL;

    // strings and buffers
    const char* node_name = LYD_NAME(change_ctx->node);
    const char* node_value = lyd_get_value(change_ctx->node);
    char path_buffer[PATH_MAX] = { 0 };
    char interface_name_buffer[100] = { 0 };

    // app context
    interfaces_ctx_t* ctx = priv;

    // mod changes context
    interfaces_mod_changes_ctx_t* mod_ctx = &ctx->mod_ctx;

    // libnl
    struct rtnl_addr* request_addr = NULL;
    struct rtnl_link* current_link = NULL;
    struct nl_addr* local_addr = NULL;

    SRPLG_LOG_INF(PLUGIN_NAME, "Node Name: %s; Previous Value: %s, Value: %s; Operation: %d", node_name, change_ctx->previous_value, node_value, change_ctx->operation);

    // get node path
    SRPC_SAFE_CALL_PTR(error_ptr, lyd_path(change_ctx->node, LYD_PATH_STD, path_buffer, sizeof(path_buffer)), error_out);

    // get interface name
    SRPC_SAFE_CALL_ERR(error, srpc_extract_xpath_key_value(path_buffer, "interface", "name", interface_name_buffer, sizeof(interface_name_buffer)), error_out);

    SRPLG_LOG_INF(PLUGIN_NAME, "Node Path: %s; Interface Name: %s", path_buffer, interface_name_buffer);

    // get link
    SRPC_SAFE_CALL_PTR(current_link, rtnl_link_get_by_name(mod_ctx->nl_ctx.link_cache, interface_name_buffer), error_out);

    // get interface index
    const int ifindex = rtnl_link_get_ifindex(current_link);

    switch (change_ctx->operation) {
    case SR_OP_CREATED:
        // new address
        request_addr = rtnl_addr_alloc();

        // parse local address
        SRPC_SAFE_CALL_ERR(error, nl_addr_parse(node_value, AF_INET, &local_addr), error_out);

        // get prefix length by using prefix-length or netmask leafs
        SRPC_SAFE_CALL_ERR_COND(error, error < 0, snprintf(path_buffer, sizeof(path_buffer), "%s[name=\"%s\"]/ietf-ip:ipv4/address/prefix-length", INTERFACES_INTERFACES_LIST_YANG_PATH, interface_name_buffer), error_out);
        SRPC_SAFE_CALL_ERR(error, srpc_iterate_changes(ctx, session, path_buffer, interfaces_interface_ipv4_address_get_prefix_length, NULL, NULL), error_out);

        SRPLG_LOG_INF(PLUGIN_NAME, "prefix-length(%s) = %d", node_value, mod_ctx->mod_data.prefix_length);

        // set final prefix length
        nl_addr_set_prefixlen(local_addr, mod_ctx->mod_data.prefix_length);

        // set to route address
        SRPC_SAFE_CALL_ERR(error, rtnl_addr_set_local(request_addr, local_addr), error_out);

        // set interface
        rtnl_addr_set_ifindex(request_addr, ifindex);

        // add address
        SRPC_SAFE_CALL_ERR(error, rtnl_addr_add(mod_ctx->nl_ctx.socket, request_addr, 0), error_out);

        break;
    case SR_OP_MODIFIED:
        // should be impossible - address IP can only be created and deleted
        SRPLG_LOG_ERR(PLUGIN_NAME, "Unsuported operation MODIFY for ietf-ip:ipv4/address/ip");
        goto error_out;
        break;
    case SR_OP_DELETED:
        // fetch info about prefix-length/netmask and use the pair address/prefix to delete address
        // prefix is needed to find the appropriate address
        break;
    case SR_OP_MOVED:
        break;
    }

    goto out;

error_out:
    if (error < 0) {
        SRPLG_LOG_INF(PLUGIN_NAME, "nl_geterror(): %s", nl_geterror(error));
    }
    error = -1;

out:
    return -1;
}

void interfaces_interface_ipv4_address_change_ip_free(void* priv)
{
}

static int interfaces_interface_ipv4_address_get_prefix_length(void* priv, sr_session_ctx_t* session, const srpc_change_ctx_t* change_ctx)
{
    int error = 0;

    // ctx
    interfaces_ctx_t* ctx = priv;
    interfaces_mod_changes_ctx_t* mod_ctx = &ctx->mod_ctx;

    const char* node_name = LYD_NAME(change_ctx->node);
    const char* node_value = lyd_get_value(change_ctx->node);

    SRPLG_LOG_INF(PLUGIN_NAME, "Node Name: %s; Previous Value: %s, Value: %s; Operation: %d", node_name, change_ctx->previous_value, node_value, change_ctx->operation);

    // this callback should only be called on CREATED operation
    assert(change_ctx->operation == SR_OP_CREATED);

    // parse prefix length
    mod_ctx->mod_data.prefix_length = atoi(node_value);

    return 0;
}

static int interfaces_interface_ipv4_address_get_netmask(void* priv, sr_session_ctx_t* session, const srpc_change_ctx_t* change_ctx)
{
}