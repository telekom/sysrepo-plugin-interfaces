#include "change.h"
#include "plugin/common.h"
#include "plugin/context.h"
#include "plugin/data/interfaces/interface/ipv4/address.h"
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
    void* error_ptr = NULL;

    // strings and buffers
    const char* node_name = LYD_NAME(change_ctx->node);
    const char* node_value = lyd_get_value(change_ctx->node);
    char path_buffer[PATH_MAX] = { 0 };
    char interface_name_buffer[100] = { 0 };
    char ip_buffer[100] = { 0 };
    char address_buffer[100] = { 0 };
    char old_address_buffer[100] = { 0 };

    // app context
    interfaces_ctx_t* ctx = priv;

    // mod changes context
    interfaces_mod_changes_ctx_t* mod_ctx = &ctx->mod_ctx;

    // libnl
    struct rtnl_addr* request_addr = NULL;
    struct rtnl_addr* delete_addr = NULL;
    struct rtnl_link* current_link = NULL;
    struct nl_addr* local_addr = NULL;
    struct nl_addr* old_local_addr = NULL;

    // data
    uint8_t prefix_length = 0;
    uint8_t old_prefix_length = 0;

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
        // no need to process created change
        break;
    case SR_OP_MODIFIED:
        // change netmask/prefix length
        request_addr = rtnl_addr_alloc();
        delete_addr = rtnl_addr_alloc();

        // convert netmask to prefix length
        SRPC_SAFE_CALL_ERR(error, interfaces_interface_ipv4_address_netmask2prefix(node_value, &prefix_length), error_out);
        SRPC_SAFE_CALL_ERR(error, interfaces_interface_ipv4_address_netmask2prefix(change_ctx->previous_value, &old_prefix_length), error_out);

        // get full address
        SRPC_SAFE_CALL_ERR_COND(error, error < 0, snprintf(address_buffer, sizeof(address_buffer), "%s/%d", ip_buffer, prefix_length), error_out);
        SRPC_SAFE_CALL_ERR_COND(error, error < 0, snprintf(old_address_buffer, sizeof(old_address_buffer), "%s/%d", ip_buffer, old_prefix_length), error_out);

        // parse local address
        SRPC_SAFE_CALL_ERR(error, nl_addr_parse(address_buffer, AF_INET, &local_addr), error_out);
        SRPC_SAFE_CALL_ERR(error, nl_addr_parse(old_address_buffer, AF_INET, &old_local_addr), error_out);

        // set to route address
        SRPC_SAFE_CALL_ERR(error, rtnl_addr_set_local(request_addr, local_addr), error_out);
        SRPC_SAFE_CALL_ERR(error, rtnl_addr_set_local(delete_addr, old_local_addr), error_out);

        // set interface
        rtnl_addr_set_ifindex(request_addr, rtnl_link_get_ifindex(current_link));
        rtnl_addr_set_ifindex(delete_addr, rtnl_link_get_ifindex(current_link));

        // delete old address
        SRPC_SAFE_CALL_ERR(error, rtnl_addr_delete(mod_ctx->nl_ctx.socket, delete_addr, 0), error_out);

        // add new address
        SRPC_SAFE_CALL_ERR(error, rtnl_addr_add(mod_ctx->nl_ctx.socket, request_addr, 0), error_out);

        break;
    case SR_OP_DELETED:
        // netmask is needed to find the appropriate address
        // should be processed when IP deleted
        break;
    case SR_OP_MOVED:
        break;
    }

    goto out;

error_out:
    if (error < 0) {
        // libnl error
        SRPLG_LOG_INF(PLUGIN_NAME, "nl_geterror(): %s", nl_geterror(error));
    }
    error = -1;

out:
    if (request_addr) {
        rtnl_addr_put(request_addr);
    }

    if (delete_addr) {
        rtnl_addr_put(delete_addr);
    }

    if (old_local_addr) {
        nl_addr_put(old_local_addr);
    }

    if (local_addr) {
        nl_addr_put(local_addr);
    }

    return error;
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

    // strings and buffers
    const char* node_name = LYD_NAME(change_ctx->node);
    const char* node_value = lyd_get_value(change_ctx->node);
    char path_buffer[PATH_MAX] = { 0 };
    char interface_name_buffer[100] = { 0 };
    char ip_buffer[100] = { 0 };
    char address_buffer[100] = { 0 };
    char old_address_buffer[100] = { 0 };

    // app context
    interfaces_ctx_t* ctx = priv;

    // mod changes context
    interfaces_mod_changes_ctx_t* mod_ctx = &ctx->mod_ctx;

    // libnl
    struct rtnl_addr* request_addr = NULL;
    struct rtnl_addr* delete_addr = NULL;
    struct rtnl_link* current_link = NULL;
    struct nl_addr* local_addr = NULL;
    struct nl_addr* old_local_addr = NULL;

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
        // no need to process created change
        break;
    case SR_OP_MODIFIED:
        // change prefix length
        request_addr = rtnl_addr_alloc();
        delete_addr = rtnl_addr_alloc();

        // get full address
        SRPC_SAFE_CALL_ERR_COND(error, error < 0, snprintf(address_buffer, sizeof(address_buffer), "%s/%s", ip_buffer, node_value), error_out);
        SRPC_SAFE_CALL_ERR_COND(error, error < 0, snprintf(old_address_buffer, sizeof(old_address_buffer), "%s/%s", ip_buffer, change_ctx->previous_value), error_out);

        // parse local address
        SRPC_SAFE_CALL_ERR(error, nl_addr_parse(address_buffer, AF_INET, &local_addr), error_out);
        SRPC_SAFE_CALL_ERR(error, nl_addr_parse(old_address_buffer, AF_INET, &old_local_addr), error_out);

        // set to route address
        SRPC_SAFE_CALL_ERR(error, rtnl_addr_set_local(request_addr, local_addr), error_out);
        SRPC_SAFE_CALL_ERR(error, rtnl_addr_set_local(delete_addr, old_local_addr), error_out);

        // set interface
        rtnl_addr_set_ifindex(request_addr, rtnl_link_get_ifindex(current_link));
        rtnl_addr_set_ifindex(delete_addr, rtnl_link_get_ifindex(current_link));

        // delete old address
        SRPC_SAFE_CALL_ERR(error, rtnl_addr_delete(mod_ctx->nl_ctx.socket, delete_addr, 0), error_out);

        // add new address
        SRPC_SAFE_CALL_ERR(error, rtnl_addr_add(mod_ctx->nl_ctx.socket, request_addr, 0), error_out);

        break;
    case SR_OP_DELETED:
        // prefix is needed to find the appropriate address
        // should be processed when IP deleted
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
    if (request_addr) {
        rtnl_addr_put(request_addr);
    }

    if (delete_addr) {
        rtnl_addr_put(delete_addr);
    }

    if (old_local_addr) {
        nl_addr_put(old_local_addr);
    }

    if (local_addr) {
        nl_addr_put(local_addr);
    }

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
    sr_conn_ctx_t* conn_ctx = NULL;
    sr_session_ctx_t* running_session = NULL;

    // strings and buffers
    const char* node_name = LYD_NAME(change_ctx->node);
    const char* node_value = lyd_get_value(change_ctx->node);
    char path_buffer[PATH_MAX] = { 0 };
    char interface_name_buffer[100] = { 0 };
    char address_buffer[100] = { 0 };

    // app context
    interfaces_ctx_t* ctx = priv;

    // mod changes context
    interfaces_mod_changes_ctx_t* mod_ctx = &ctx->mod_ctx;

    // libnl
    struct rtnl_addr* request_addr = NULL;
    struct rtnl_link* current_link = NULL;
    struct nl_addr* local_addr = NULL;

    // data
    uint8_t prefix_length = 0;

    SRPLG_LOG_INF(PLUGIN_NAME, "Node Name: %s; Previous Value: %s, Value: %s; Operation: %d", node_name, change_ctx->previous_value, node_value, change_ctx->operation);

    // get node path
    SRPC_SAFE_CALL_PTR(error_ptr, lyd_path(change_ctx->node, LYD_PATH_STD, path_buffer, sizeof(path_buffer)), error_out);

    // get interface name
    SRPC_SAFE_CALL_ERR(error, srpc_extract_xpath_key_value(path_buffer, "interface", "name", interface_name_buffer, sizeof(interface_name_buffer)), error_out);

    SRPLG_LOG_INF(PLUGIN_NAME, "Node Path: %s; Interface Name: %s", path_buffer, interface_name_buffer);

    // get link
    SRPC_SAFE_CALL_PTR(current_link, rtnl_link_get_by_name(mod_ctx->nl_ctx.link_cache, interface_name_buffer), error_out);

    // get connection
    SRPC_SAFE_CALL_PTR(conn_ctx, sr_session_get_connection(session), error_out);

    // start a running DS session - fetching data about prefix when deleting the address
    SRPC_SAFE_CALL_ERR(error, sr_session_start(conn_ctx, SR_DS_RUNNING, &running_session), error_out);

    switch (change_ctx->operation) {
    case SR_OP_CREATED:
        // new address
        request_addr = rtnl_addr_alloc();

        // parse local address
        SRPC_SAFE_CALL_ERR(error, nl_addr_parse(node_value, AF_INET, &local_addr), error_out);

        // get prefix length by using prefix-length or netmask leafs
        SRPC_SAFE_CALL_ERR_COND(error, error < 0, snprintf(path_buffer, sizeof(path_buffer), "%s[name=\"%s\"]/ietf-ip:ipv4/address[ip=\"%s\"]/prefix-length", INTERFACES_INTERFACES_LIST_YANG_PATH, interface_name_buffer, node_value), error_out);
        SRPC_SAFE_CALL_ERR(error, srpc_iterate_changes(ctx, session, path_buffer, interfaces_interface_ipv4_address_get_prefix_length, NULL, NULL), error_out);

        if (!mod_ctx->mod_data.ipv4.address.prefix_set) {
            // prefix not found - check for netmask
            SRPC_SAFE_CALL_ERR_COND(error, error < 0, snprintf(path_buffer, sizeof(path_buffer), "%s[name=\"%s\"]/ietf-ip:ipv4/address[ip=\"%s\"]/netmask", INTERFACES_INTERFACES_LIST_YANG_PATH, interface_name_buffer, node_value), error_out);
            SRPC_SAFE_CALL_ERR(error, srpc_iterate_changes(ctx, session, path_buffer, interfaces_interface_ipv4_address_get_netmask, NULL, NULL), error_out);

            if (!mod_ctx->mod_data.ipv4.address.prefix_set) {
                SRPLG_LOG_ERR(PLUGIN_NAME, "Unable to get prefix-length/netmask for address %s... Discarding changes", node_value);
                goto error_out;
            }
        }

        SRPLG_LOG_INF(PLUGIN_NAME, "Recieved prefix-length of %d for address %s", mod_ctx->mod_data.ipv4.address.prefix_length, node_value);

        // prefix was set and found

        // set final prefix length
        nl_addr_set_prefixlen(local_addr, mod_ctx->mod_data.ipv4.address.prefix_length);

        // set to route address
        SRPC_SAFE_CALL_ERR(error, rtnl_addr_set_local(request_addr, local_addr), error_out);

        // set interface
        rtnl_addr_set_ifindex(request_addr, rtnl_link_get_ifindex(current_link));

        // add address
        SRPC_SAFE_CALL_ERR(error, rtnl_addr_add(mod_ctx->nl_ctx.socket, request_addr, 0), error_out);

        break;
    case SR_OP_MODIFIED:
        // should be impossible - address IP can only be created and deleted
        SRPLG_LOG_ERR(PLUGIN_NAME, "Unsuported operation MODIFY for interface IPv4 address IP leaf");
        goto error_out;
        break;
    case SR_OP_DELETED:
        // fetch info about prefix-length/netmask and use the pair address/prefix to delete address
        // prefix is needed to find the appropriate address
        request_addr = rtnl_addr_alloc();

        // check for prefix-length
        SRPC_SAFE_CALL_ERR_COND(error, error < 0, snprintf(path_buffer, sizeof(path_buffer), INTERFACES_INTERFACES_INTERFACE_YANG_PATH "[name=\"%s\"]/ietf-ip:ipv4/address[ip=\"%s\"]/prefix-length", interface_name_buffer, node_value), error_out);
        SRPLG_LOG_INF(PLUGIN_NAME, "Searching running DS for %s", path_buffer);

        error = sr_get_item(running_session, path_buffer, 0, &prefix_val);
        if (error == SR_ERR_OK) {
            // parse prefix-length
            prefix_length = prefix_val->data.uint8_val;
        } else if (error == SR_ERR_NOT_FOUND) {
            // fetch netmask
            SRPC_SAFE_CALL_ERR_COND(error, error < 0, snprintf(path_buffer, sizeof(path_buffer), "%s[name=\"%s\"]/ietf-ip:ipv4/address[ip=\"%s\"]/netmask", INTERFACES_INTERFACES_LIST_YANG_PATH, interface_name_buffer, node_value), error_out);
            SRPC_SAFE_CALL_ERR(error, sr_get_item(running_session, path_buffer, 0, &netmask_val), error_out);

            // convert netmask to prefix
            SRPC_SAFE_CALL_ERR(error, interfaces_interface_ipv4_address_netmask2prefix(netmask_val->data.string_val, &prefix_length), error_out);
        } else {
            // other error - treat as invalid
            SRPLG_LOG_ERR(PLUGIN_NAME, "Error retrieving prefix-length value for address %s", node_value);
            goto error_out;
        }

        SRPLG_LOG_INF(PLUGIN_NAME, "Recieved prefix for address %s: %d", node_value, prefix_length);

        // after getting the prefix length - remove address

        // get full address
        SRPC_SAFE_CALL_ERR_COND(error, error < 0, snprintf(address_buffer, sizeof(address_buffer), "%s/%d", node_value, prefix_length), error_out);

        // parse local address
        SRPC_SAFE_CALL_ERR(error, nl_addr_parse(address_buffer, AF_INET, &local_addr), error_out);

        // set to route address
        SRPC_SAFE_CALL_ERR(error, rtnl_addr_set_local(request_addr, local_addr), error_out);

        // set interface
        rtnl_addr_set_ifindex(request_addr, rtnl_link_get_ifindex(current_link));

        // remove wanted address
        SRPC_SAFE_CALL_ERR(error, rtnl_addr_delete(mod_ctx->nl_ctx.socket, request_addr, 0), error_out);

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
    if (running_session) {
        sr_session_stop(running_session);
    }

    if (request_addr) {
        rtnl_addr_put(request_addr);
    }

    // re-initialize mod_ctx data
    mod_ctx->mod_data.ipv4.address.prefix_length = 0;
    mod_ctx->mod_data.ipv4.address.prefix_set = false;

    return error;
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
    mod_ctx->mod_data.ipv4.address.prefix_length = (uint8_t)atoi(node_value);
    mod_ctx->mod_data.ipv4.address.prefix_set = true;

    return error;
}

static int interfaces_interface_ipv4_address_get_netmask(void* priv, sr_session_ctx_t* session, const srpc_change_ctx_t* change_ctx)
{
    int error = 0;

    // ctx
    interfaces_ctx_t* ctx = priv;
    interfaces_mod_changes_ctx_t* mod_ctx = &ctx->mod_ctx;

    uint8_t prefix_length = 0;

    const char* node_name = LYD_NAME(change_ctx->node);
    const char* node_value = lyd_get_value(change_ctx->node);

    SRPLG_LOG_INF(PLUGIN_NAME, "Node Name: %s; Previous Value: %s, Value: %s; Operation: %d", node_name, change_ctx->previous_value, node_value, change_ctx->operation);

    // this callback should only be called on CREATED operation
    assert(change_ctx->operation == SR_OP_CREATED);

    // parse netmask into prefix length
    SRPC_SAFE_CALL_ERR(error, interfaces_interface_ipv4_address_netmask2prefix(node_value, &prefix_length), error_out);

    // set mod changes prefix length
    mod_ctx->mod_data.ipv4.address.prefix_length = prefix_length;
    mod_ctx->mod_data.ipv4.address.prefix_set = true;

    goto out;

error_out:
    error = -1;
    mod_ctx->mod_data.ipv4.address.prefix_set = false;

out:
    return error;
}