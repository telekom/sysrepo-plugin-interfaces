#include "change.h"
#include "netlink/errno.h"
#include "netlink/route/link.h"
#include "plugin/common.h"
#include "plugin/context.h"
#include "sysrepo/xpath.h"

#include <linux/if.h>
#include <linux/netlink.h>
#include <string.h>
#include <sysrepo.h>

#include <errno.h>

static int interfacecs_interface_extract_name(sr_session_ctx_t* session, const struct lyd_node* node, char* name_buffer, size_t buffer_size);

int interfaces_interface_change_parent_interface(void* priv, sr_session_ctx_t* session, const srpc_change_ctx_t* change_ctx)
{
    int error = 0;
    const char* node_name = LYD_NAME(change_ctx->node);
    const char* node_value = lyd_get_value(change_ctx->node);

    SRPLG_LOG_INF(PLUGIN_NAME, "Node Name: %s; Previous Value: %s; Value: %s; Operation: %d", node_name, change_ctx->previous_value, node_value, change_ctx->operation);

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

int interfaces_interface_change_max_frame_size(void* priv, sr_session_ctx_t* session, const srpc_change_ctx_t* change_ctx)
{
    int error = 0;
    const char* node_name = LYD_NAME(change_ctx->node);
    const char* node_value = lyd_get_value(change_ctx->node);

    SRPLG_LOG_INF(PLUGIN_NAME, "Node Name: %s; Previous Value: %s; Value: %s; Operation: %d", node_name, change_ctx->previous_value, node_value, change_ctx->operation);

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

int interfaces_interface_change_loopback(void* priv, sr_session_ctx_t* session, const srpc_change_ctx_t* change_ctx)
{
    int error = 0;
    const char* node_name = LYD_NAME(change_ctx->node);
    const char* node_value = lyd_get_value(change_ctx->node);

    SRPLG_LOG_INF(PLUGIN_NAME, "Node Name: %s; Previous Value: %s; Value: %s; Operation: %d", node_name, change_ctx->previous_value, node_value, change_ctx->operation);

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

int interfaces_interface_change_link_up_down_trap_enable(void* priv, sr_session_ctx_t* session, const srpc_change_ctx_t* change_ctx)
{
    int error = 0;
    const char* node_name = LYD_NAME(change_ctx->node);
    const char* node_value = lyd_get_value(change_ctx->node);

    SRPLG_LOG_INF(PLUGIN_NAME, "Node Name: %s; Previous Value: %s; Value: %s; Operation: %d", node_name, change_ctx->previous_value, node_value, change_ctx->operation);

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

int interfaces_interface_change_enabled(void* priv, sr_session_ctx_t* session, const srpc_change_ctx_t* change_ctx)
{
    int error = 0;

    char interface_name_buffer[256] = { 0 };

    interfaces_ctx_t* ctx = (interfaces_ctx_t*)priv;
    interfaces_mod_changes_ctx_t* mod_ctx = &ctx->mod_ctx;
    struct rtnl_link* current_link = NULL;
    struct rtnl_link* request_link = NULL;

    const char* node_name = LYD_NAME(change_ctx->node);
    const char* node_value = lyd_get_value(change_ctx->node);

    SRPLG_LOG_INF(PLUGIN_NAME, "Node Name: %s; Previous Value: %s; Value: %s; Operation: %d", node_name, change_ctx->previous_value, node_value, change_ctx->operation);

    // get interface name
    SRPC_SAFE_CALL_ERR(error, interfacecs_interface_extract_name(session, change_ctx->node, interface_name_buffer, sizeof(interface_name_buffer)), error_out);

    SRPLG_LOG_INF(PLUGIN_NAME, "Working with interface %s", interface_name_buffer);

    // get link by name
    SRPC_SAFE_CALL_PTR(current_link, rtnl_link_get_by_name(mod_ctx->link_cache, interface_name_buffer), error_out);

    // create request link
    SRPC_SAFE_CALL_PTR(request_link, rtnl_link_alloc(), error_out);

    // set name
    rtnl_link_set_name(request_link, interface_name_buffer);
    rtnl_link_set_type(request_link, rtnl_link_get_type(current_link));

    switch (change_ctx->operation) {
    case SR_OP_CREATED:
    case SR_OP_MODIFIED:
        // set operstate
        rtnl_link_set_flags(request_link, (strcmp(node_value, "true") == 0) ? (unsigned int)rtnl_link_str2flags("up") : (unsigned int)rtnl_link_str2flags("down"));
        rtnl_link_unset_flags(request_link, (strcmp(node_value, "true") == 0) ? (unsigned int)rtnl_link_str2flags("down") : (unsigned int)rtnl_link_str2flags("up"));
        rtnl_link_set_operstate(request_link, (strcmp(node_value, "true") == 0) ? IF_OPER_UP : IF_OPER_DOWN);
        break;
    case SR_OP_DELETED:
        // treat as set to up - default value
        rtnl_link_set_flags(request_link, (unsigned int)rtnl_link_str2flags("up"));
        rtnl_link_set_operstate(request_link, IF_OPER_UP);
        break;
    case SR_OP_MOVED:
        break;
    }

    SRPLG_LOG_INF(PLUGIN_NAME, "Current link status: %d", rtnl_link_get_operstate(current_link));
    SRPLG_LOG_INF(PLUGIN_NAME, "Changed link status: %d", rtnl_link_get_operstate(request_link));

    // error = rtnl_link_change(mod_ctx->socket, current_link, request_link, NLM_F_REPLACE);
    // if (error) {
    //     SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_change() error (%d) : %s", error, nl_geterror(error));
    //     goto error_out;
    // }

    // apply changes
    SRPC_SAFE_CALL_ERR(error, rtnl_link_change(mod_ctx->socket, current_link, request_link, NLM_F_REPLACE), error_out);

    goto out;

error_out:
    if (error < 0) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "nl_geterror(): %s", nl_geterror(error));
    }
    error = -1;

out:
    // free request link
    rtnl_link_put(request_link);

    return error;
}

int interfaces_interface_change_type(void* priv, sr_session_ctx_t* session, const srpc_change_ctx_t* change_ctx)
{
    int error = 0;

    char interface_name_buffer[256] = { 0 };

    interfaces_ctx_t* ctx = (interfaces_ctx_t*)priv;
    interfaces_mod_changes_ctx_t* mod_ctx = &ctx->mod_ctx;
    struct rtnl_link* current_link = NULL;
    struct rtnl_link* request_link = NULL;
    bool type_set = false;

    struct {
        const char* yang_type;
        const char* type;
    } type_pairs[] = {
        {
            "iana-if-type:ethernetCsmacd",
            "veth",
        },
        {
            "iana-if-type:softwareLoopback",
            "lo",
        },
        {
            "iana-if-type:l2vlan",
            "vlan",
        },
        {
            "iana-if-type:other",
            "dummy",
        },
    };

    const char* node_name = LYD_NAME(change_ctx->node);
    const char* node_value = lyd_get_value(change_ctx->node);

    SRPLG_LOG_INF(PLUGIN_NAME, "Node Name: %s; Previous Value: %s; Value: %s; Operation: %d", node_name, change_ctx->previous_value, node_value, change_ctx->operation);

    // get interface name
    SRPC_SAFE_CALL_ERR(error, interfacecs_interface_extract_name(session, change_ctx->node, interface_name_buffer, sizeof(interface_name_buffer)), error_out);

    // get link by name
    SRPC_SAFE_CALL_PTR(current_link, rtnl_link_get_by_name(mod_ctx->link_cache, interface_name_buffer), error_out);

    // create request link
    SRPC_SAFE_CALL_PTR(request_link, rtnl_link_alloc(), error_out);

    // set name
    rtnl_link_set_name(request_link, interface_name_buffer);

    switch (change_ctx->operation) {
    case SR_OP_CREATED:
    case SR_OP_MODIFIED:
        // set type
        for (size_t i = 0; i < ARRAY_SIZE(type_pairs); i++) {
            if (!strcmp(node_value, type_pairs[i].yang_type)) {
                rtnl_link_set_type(request_link, type_pairs[i].type);
                type_set = true;
                break;
            }
        }

        if (!type_set) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "Unsupported interface type (%s)", node_value);
            goto error_out;
        }

        // apply changes
        SRPC_SAFE_CALL_ERR(error, rtnl_link_change(mod_ctx->socket, current_link, request_link, 0), error_out);
        break;
    case SR_OP_DELETED:
        // unsupported - type is necessarry
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

int interfaces_interface_change_description(void* priv, sr_session_ctx_t* session, const srpc_change_ctx_t* change_ctx)
{
    int error = 0;
    const char* node_name = LYD_NAME(change_ctx->node);
    const char* node_value = lyd_get_value(change_ctx->node);

    SRPLG_LOG_INF(PLUGIN_NAME, "Node Name: %s; Previous Value: %s; Value: %s; Operation: %d", node_name, change_ctx->previous_value, node_value, change_ctx->operation);

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

int interfaces_interface_change_name(void* priv, sr_session_ctx_t* session, const srpc_change_ctx_t* change_ctx)
{
    int error = 0;

    interfaces_ctx_t* ctx = (interfaces_ctx_t*)priv;
    interfaces_mod_changes_ctx_t* mod_ctx = &ctx->mod_ctx;
    struct rtnl_link* new_link = NULL;
    struct rtnl_link* old_link = NULL;

    const char* node_name = LYD_NAME(change_ctx->node);
    const char* node_value = lyd_get_value(change_ctx->node);

    SRPLG_LOG_INF(PLUGIN_NAME, "Node Name: %s; Previous Value: %s; Value: %s; Operation: %d", node_name, change_ctx->previous_value, node_value, change_ctx->operation);

    switch (change_ctx->operation) {
    case SR_OP_CREATED:
        old_link = rtnl_link_get_by_name(mod_ctx->link_cache, node_value);

        // add a new link if such a link doesn't exist
        if (!old_link) {
            // create a new link
            SRPC_SAFE_CALL_PTR(new_link, rtnl_link_alloc(), error_out);

            // setup link and add it to the system
            rtnl_link_set_name(new_link, node_value);

            // set temp as initial type
            SRPC_SAFE_CALL_ERR(error, rtnl_link_set_type(new_link, "dummy"), error_out);

            SRPC_SAFE_CALL_ERR(error, rtnl_link_add(mod_ctx->socket, new_link, NLM_F_CREATE), error_out);
        }
        break;
    case SR_OP_MODIFIED:
        // name cannot be modified - only deleted and created again
        goto error_out;
    case SR_OP_DELETED:
        // get link and delete it
        SRPC_SAFE_CALL_PTR(old_link, rtnl_link_get_by_name(mod_ctx->link_cache, change_ctx->previous_value), error_out);

        SRPC_SAFE_CALL_ERR(error, rtnl_link_delete(mod_ctx->socket, old_link), error_out);
        break;
    case SR_OP_MOVED:
        break;
    }

    goto out;

error_out:
    if (error < 0) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "nl_geterror(): %d = %s", error, nl_geterror(error));
    }
    error = -1;

out:
    return error;
}

static int interfacecs_interface_extract_name(sr_session_ctx_t* session, const struct lyd_node* node, char* name_buffer, size_t buffer_size)
{
    int error = 0;
    int rc = 0;
    void* error_ptr = NULL;

    const char* name = NULL;

    sr_xpath_ctx_t xpath_ctx = { 0 };
    char path_buffer[PATH_MAX] = { 0 };

    // get node full path
    SRPC_SAFE_CALL_PTR(error_ptr, lyd_path(node, LYD_PATH_STD, path_buffer, sizeof(path_buffer)), error_out);

    // extract key
    SRPC_SAFE_CALL_PTR(name, sr_xpath_key_value(path_buffer, "interface", "name", &xpath_ctx), error_out);

    // store to buffer
    SRPC_SAFE_CALL_ERR_COND(rc, rc < 0, snprintf(name_buffer, buffer_size, "%s", name), error_out);

    goto out;

error_out:
    error = -1;

out:
    return error;
}