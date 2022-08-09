#include "load.h"
#include "plugin/common.h"

#include <libyang/libyang.h>
#include <srpc.h>
#include <sysrepo.h>

static int interfaces_startup_load_interface(void* priv, sr_session_ctx_t* session, const struct ly_ctx* ly_ctx, struct lyd_node* parent_node);

int interfaces_startup_load(interfaces_ctx_t* ctx, sr_session_ctx_t* session)
{
    int error = 0;

    const struct ly_ctx* ly_ctx = NULL;
    struct lyd_node* root_node = NULL;
    sr_conn_ctx_t* conn_ctx = NULL;

    srpc_startup_load_t load_values[] = {
        {
            "/ietf-interfaces:interfaces/interface[name='%s']",
            interfaces_startup_load_interface,
        },
    };

    conn_ctx = sr_session_get_connection(session);
    ly_ctx = sr_acquire_context(conn_ctx);
    if (ly_ctx == NULL) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "Unable to get ly_ctx variable");
        goto error_out;
    }

    // load system container info
    // [LOAD ROOT NODE HERE]
    goto out;
    for (size_t i = 0; i < ARRAY_SIZE(load_values); i++) {
        const srpc_startup_load_t* load = &load_values[i];

        error = load->cb((void*)ctx, session, ly_ctx, root_node);
        if (error) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "Node creation callback failed for value %s", load->name);
            goto error_out;
        }
    }

    error = sr_edit_batch(session, root_node, "merge");
    if (error != SR_ERR_OK) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "sr_edit_batch() error (%d): %s", error, sr_strerror(error));
        goto error_out;
    }

    error = sr_apply_changes(session, 0);
    if (error != 0) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "sr_apply_changes() error (%d): %s", error, sr_strerror(error));
        goto error_out;
    }

    goto out;

error_out:
    error = -1;

out:
    if (root_node) {
        lyd_free_tree(root_node);
    }
    sr_release_context(conn_ctx);
    return error;
}

static int interfaces_startup_load_interface(void* priv, sr_session_ctx_t* session, const struct ly_ctx* ly_ctx, struct lyd_node* parent_node)
{
    int error = 0;
    return error;
}
