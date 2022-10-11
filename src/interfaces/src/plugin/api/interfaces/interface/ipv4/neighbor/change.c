#include "change.h"
#include "netlink/addr.h"
#include "plugin/common.h"
#include "plugin/context.h"

#include <assert.h>
#include <linux/netlink.h>
#include <sysrepo.h>

#include <netlink/route/addr.h>
#include <netlink/route/link.h>
#include <netlink/route/neighbour.h>

static int interfaces_interface_ipv4_address_get_link_layer_address(void* priv, sr_session_ctx_t* session, const srpc_change_ctx_t* change_ctx);

int interfaces_interface_ipv4_neighbor_change_link_layer_address_init(void* priv)
{
    int error = 0;
    return error;
}

int interfaces_interface_ipv4_neighbor_change_link_layer_address(void* priv, sr_session_ctx_t* session, const srpc_change_ctx_t* change_ctx)
{
    int error = 0;
    void* error_ptr = NULL;

    // strings and buffers
    const char* node_name = LYD_NAME(change_ctx->node);
    const char* node_value = lyd_get_value(change_ctx->node);
    char path_buffer[PATH_MAX] = { 0 };
    char interface_name_buffer[100] = { 0 };
    char ip_buffer[100] = { 0 };

    // app context
    interfaces_ctx_t* ctx = priv;

    // mod changes context
    interfaces_mod_changes_ctx_t* mod_ctx = &ctx->mod_ctx;

    // libnl
    struct rtnl_neigh* request_neigh = NULL;
    struct rtnl_link* current_link = NULL;
    struct nl_addr* dst_addr = NULL;
    struct nl_addr* ll_addr = NULL;

    SRPLG_LOG_INF(PLUGIN_NAME, "Node Name: %s; Previous Value: %s, Value: %s; Operation: %d", node_name, change_ctx->previous_value, node_value, change_ctx->operation);

    // get node path
    SRPC_SAFE_CALL_PTR(error_ptr, lyd_path(change_ctx->node, LYD_PATH_STD, path_buffer, sizeof(path_buffer)), error_out);

    // get interface name
    SRPC_SAFE_CALL_ERR(error, srpc_extract_xpath_key_value(path_buffer, "interface", "name", interface_name_buffer, sizeof(interface_name_buffer)), error_out);

    // get IP
    SRPC_SAFE_CALL_ERR(error, srpc_extract_xpath_key_value(path_buffer, "neighbor", "ip", ip_buffer, sizeof(ip_buffer)), error_out);

    SRPLG_LOG_INF(PLUGIN_NAME, "Node Path: %s; Interface Name: %s; Neighbor IP: %s", path_buffer, interface_name_buffer, ip_buffer);

    // get link
    SRPC_SAFE_CALL_PTR(current_link, rtnl_link_get_by_name(mod_ctx->nl_ctx.link_cache, interface_name_buffer), error_out);

    switch (change_ctx->operation) {
    case SR_OP_CREATED:
        // not used - used only in IP change callback
        break;
    case SR_OP_MODIFIED:
        // change lladdr
        request_neigh = rtnl_neigh_alloc();

        // set interface
        rtnl_neigh_set_ifindex(request_neigh, rtnl_link_get_ifindex(current_link));

        // parse destination and LL address
        SRPC_SAFE_CALL_ERR(error, nl_addr_parse(ip_buffer, AF_INET, &dst_addr), error_out);
        SRPC_SAFE_CALL_ERR(error, nl_addr_parse(node_value, AF_LLC, &ll_addr), error_out);

        // set destination and LL address
        SRPC_SAFE_CALL_ERR(error, rtnl_neigh_set_dst(request_neigh, dst_addr), error_out);
        rtnl_neigh_set_lladdr(request_neigh, ll_addr);

        // change neighbor
        SRPC_SAFE_CALL_ERR(error, rtnl_neigh_add(mod_ctx->nl_ctx.socket, request_neigh, NLM_F_REPLACE), error_out);

        break;
    case SR_OP_DELETED:
        // not used - used only in IP change callback
        break;
    case SR_OP_MOVED:
        break;
    }

    goto out;

error_out:
    error = -1;

out:

    return error;
}

void interfaces_interface_ipv4_neighbor_change_link_layer_address_free(void* priv)
{
}

int interfaces_interface_ipv4_neighbor_change_ip_init(void* priv)
{
    int error = 0;
    return error;
}

int interfaces_interface_ipv4_neighbor_change_ip(void* priv, sr_session_ctx_t* session, const srpc_change_ctx_t* change_ctx)
{
    int error = 0;
    void* error_ptr = NULL;

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
    struct rtnl_neigh* request_neigh = NULL;
    struct rtnl_link* current_link = NULL;
    struct nl_addr* dst_addr = NULL;
    struct nl_addr* ll_addr = NULL;

    SRPLG_LOG_INF(PLUGIN_NAME, "Node Name: %s; Previous Value: %s, Value: %s; Operation: %d", node_name, change_ctx->previous_value, node_value, change_ctx->operation);

    // get node path
    SRPC_SAFE_CALL_PTR(error_ptr, lyd_path(change_ctx->node, LYD_PATH_STD, path_buffer, sizeof(path_buffer)), error_out);

    // get interface name
    SRPC_SAFE_CALL_ERR(error, srpc_extract_xpath_key_value(path_buffer, "interface", "name", interface_name_buffer, sizeof(interface_name_buffer)), error_out);

    SRPLG_LOG_INF(PLUGIN_NAME, "Node Path: %s; Interface Name: %s", path_buffer, interface_name_buffer);

    // get link
    SRPC_SAFE_CALL_PTR(current_link, rtnl_link_get_by_name(mod_ctx->nl_ctx.link_cache, interface_name_buffer), error_out);

    switch (change_ctx->operation) {
    case SR_OP_CREATED:
        // new neighbor
        request_neigh = rtnl_neigh_alloc();

        // parse destination address
        SRPC_SAFE_CALL_ERR(error, nl_addr_parse(node_value, AF_INET, &dst_addr), error_out);

        // get prefix length by using prefix-length or netmask leafs
        SRPC_SAFE_CALL_ERR_COND(error, error < 0, snprintf(path_buffer, sizeof(path_buffer), INTERFACES_INTERFACES_INTERFACE_YANG_PATH "[name=\"%s\"]/ietf-ip:ipv4/neighbor[ip=\"%s\"]/link-layer-address", interface_name_buffer, node_value), error_out);
        SRPC_SAFE_CALL_ERR(error, srpc_iterate_changes(ctx, session, path_buffer, interfaces_interface_ipv4_address_get_link_layer_address, NULL, NULL), error_out);

        SRPLG_LOG_INF(PLUGIN_NAME, "Recieved link-layer-address %s for neighbor address %s", mod_ctx->mod_data.ipv4.neighbor.link_layer_address, node_value);

        // parse link-layer address
        SRPC_SAFE_CALL_ERR(error, nl_addr_parse(mod_ctx->mod_data.ipv4.neighbor.link_layer_address, AF_LLC, &ll_addr), error_out);

        // set addresses to the new neighbor
        SRPC_SAFE_CALL_ERR(error, rtnl_neigh_set_dst(request_neigh, dst_addr), error_out);
        rtnl_neigh_set_lladdr(request_neigh, ll_addr);

        // set interface
        rtnl_neigh_set_ifindex(request_neigh, rtnl_link_get_ifindex(current_link));

        // add neighbor
        SRPC_SAFE_CALL_ERR(error, rtnl_neigh_add(mod_ctx->nl_ctx.socket, request_neigh, NLM_F_CREATE), error_out);

        break;
    case SR_OP_MODIFIED:
        // should be impossible - address IP can only be created and deleted
        SRPLG_LOG_ERR(PLUGIN_NAME, "Unsuported operation MODIFY for interface IPv4 neighbor IP leaf");
        goto error_out;
        break;
    case SR_OP_DELETED:
        request_neigh = rtnl_neigh_alloc();

        // set interface
        rtnl_neigh_set_ifindex(request_neigh, rtnl_link_get_ifindex(current_link));

        // parse destination
        SRPC_SAFE_CALL_ERR(error, nl_addr_parse(node_value, AF_INET, &dst_addr), error_out);

        // set destination
        SRPC_SAFE_CALL_ERR(error, rtnl_neigh_set_dst(request_neigh, dst_addr), error_out);

        // remove wanted neighbor
        SRPC_SAFE_CALL_ERR(error, rtnl_neigh_delete(mod_ctx->nl_ctx.socket, request_neigh, 0), error_out);

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

    // re-initialize mod_ctx data
    if (mod_ctx->mod_data.ipv4.neighbor.link_layer_address) {
        free(mod_ctx->mod_data.ipv4.neighbor.link_layer_address);
    }
    mod_ctx->mod_data.ipv4.neighbor.link_layer_address = NULL;
    mod_ctx->mod_data.ipv4.neighbor.link_layer_set = false;

    return error;
}

void interfaces_interface_ipv4_neighbor_change_ip_free(void* priv)
{
}

static int interfaces_interface_ipv4_address_get_link_layer_address(void* priv, sr_session_ctx_t* session, const srpc_change_ctx_t* change_ctx)
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

    // set mod changes prefix length
    mod_ctx->mod_data.ipv4.neighbor.link_layer_address = strdup(node_value);
    if (!mod_ctx->mod_data.ipv4.neighbor.link_layer_address) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "Unable to set link-layer-address value for module changes context");
        goto error_out;
    }
    mod_ctx->mod_data.ipv4.neighbor.link_layer_set = true;

    goto out;

error_out:
    error = -1;
    mod_ctx->mod_data.ipv4.neighbor.link_layer_set = false;

out:
    return error;
}