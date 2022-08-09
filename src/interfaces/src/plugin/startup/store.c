#include "store.h"
#include "plugin/common.h"

#include <libyang/libyang.h>
#include <srpc.h>
#include <sysrepo.h>

static int interfaces_startup_store_interface(void* priv, const struct lyd_node* parent_container);

int interfaces_startup_store(interfaces_ctx_t* ctx, sr_session_ctx_t* session)
{
    int error = 0;
    sr_data_t* subtree = NULL;

    error = sr_get_subtree(session, "[ENTER_ROOT_CONFIG_PATH]", 0, &subtree);
    if (error) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "sr_get_subtree() error (%d): %s", error, sr_strerror(error));
        goto error_out;
    }

    srpc_startup_store_t store_values[] = {
        {
            "/ietf-interfaces:interfaces/interface[name='%s']",
            interfaces_startup_store_interface,
        },
    };

    for (size_t i = 0; i < ARRAY_SIZE(store_values); i++) {
        const srpc_startup_store_t* store = &store_values[i];

        error = store->cb(ctx, subtree->tree);
        if (error != 0) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "Startup store callback failed for value %s", store->name);
            goto error_out;
        }
    }

    goto out;

error_out:
    error = -1;

out:
    if (subtree) {
        sr_release_data(subtree);
    }

    return error;
}

static int interfaces_startup_store_interface(void* priv, const struct lyd_node* parent_container)
{
    int error = 0;
    return error;
}
