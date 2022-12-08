#include "load.h"
#include "libyang/printer_data.h"
#include "plugin/common.h"
#include "plugin/data/interfaces/interface.h"
#include "plugin/ly_tree.h"

#include "plugin/api/interfaces/load.h"
#include "plugin/types.h"
#include "src/uthash.h"
#include "src/utlist.h"
#include "srpc/common.h"

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
            "/ietf-interfaces:interfaces/interface",
            interfaces_startup_load_interface,
        },
    };

    SRPC_SAFE_CALL_PTR(conn_ctx, sr_session_get_connection(session), error_out);
    SRPC_SAFE_CALL_PTR(ly_ctx, sr_acquire_context(conn_ctx), error_out);

    SRPC_SAFE_CALL_ERR(error, interfaces_ly_tree_create_interfaces(ly_ctx, &root_node), error_out);

    for (size_t i = 0; i < ARRAY_SIZE(load_values); i++) {
        const srpc_startup_load_t* load = &load_values[i];

        error = load->cb((void*)ctx, session, ly_ctx, root_node);
        if (error) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "Node creation callback failed for value %s", load->name);
            goto error_out;
        }
    }

/* enable or disable storing into startup, use for testing */
#define INTERFACES_PLUGIN_LOAD_STARTUP
/* disable for now */
// #undef INTERFACES_PLUGIN_LOAD_STARTUP
#ifdef INTERFACES_PLUGIN_LOAD_STARTUP
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
#endif

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
    interfaces_ctx_t* ctx = (interfaces_ctx_t*)priv;
    interfaces_interface_hash_element_t* interface_hash = NULL;

    // load interfaces data
    SRPC_SAFE_CALL_ERR(error, interfaces_load_interface(ctx, &interface_hash), error_out);

    // convert to libyang
    SRPC_SAFE_CALL_ERR(error, interfaces_interface_hash_to_ly(ly_ctx, interface_hash, &parent_node), error_out);

    // print created tree
    // lyd_print_file(stdout, parent_node, LYD_XML, 0);

    goto out;

error_out:
    error = -1;
out:
    interfaces_interface_hash_free(&interface_hash);

    return error;
}
