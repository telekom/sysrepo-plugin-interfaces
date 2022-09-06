#include "change.h"
#include "plugin/common.h"
#include "plugin/context.h"

#include "interface/change.h"

#include <srpc.h>
#include <sysrepo.h>

int interfaces_change_interface_init(void* priv)
{
    int error = 0;
    interfaces_ctx_t* ctx = (interfaces_ctx_t*)priv;
    (void)ctx;

    SRPLG_LOG_INF(PLUGIN_NAME, "Initializing context data for interface changes");

    return error;
}

int interfaces_change_interface(void* priv, sr_session_ctx_t* session, const srpc_change_ctx_t* change_ctx)
{
    int error = 0;
    int rc = 0;

    interfaces_ctx_t* ctx = (interfaces_ctx_t*)priv;

    char xpath_buffer[PATH_MAX] = { 0 };
    char change_xpath_buffer[PATH_MAX] = { 0 };
    const char* node_name = LYD_NAME(change_ctx->node);
    const char* node_value = lyd_get_value(change_ctx->node);

    // get node xpath
    lyd_path(change_ctx->node, LYD_PATH_STD, xpath_buffer, sizeof(xpath_buffer));

    SRPLG_LOG_INF(PLUGIN_NAME, "Node Name: %s; Previous Value: %s, Value: %s; Operation: %d, Node XPath: %s", node_name, change_ctx->previous_value, node_value, change_ctx->operation, xpath_buffer);

    // name
    SRPC_SAFE_CALL_ERR_COND(rc, rc < 0, snprintf(change_xpath_buffer, sizeof(change_xpath_buffer), "%s/name", xpath_buffer), error_out);
    SRPC_SAFE_CALL_ERR(rc, srpc_iterate_changes(ctx, session, change_xpath_buffer, interfaces_interface_change_name, NULL, NULL), error_out);

    // description
    SRPC_SAFE_CALL_ERR_COND(rc, rc < 0, snprintf(change_xpath_buffer, sizeof(change_xpath_buffer), "%s/description", xpath_buffer), error_out);
    SRPC_SAFE_CALL_ERR(rc, srpc_iterate_changes(ctx, session, change_xpath_buffer, interfaces_interface_change_description, NULL, NULL), error_out);

    goto out;

error_out:
    error = -1;

out:
    return error;
}

void interfaces_change_interface_free(void* priv)
{
    SRPLG_LOG_INF(PLUGIN_NAME, "Freeing context data for interface changes");
}
