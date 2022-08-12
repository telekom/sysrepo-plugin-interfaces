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

    error = sr_get_subtree(session, INTERFACES_INTERFACES_CONTAINER_YANG_PATH, 0, &subtree);
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
    interfaces_ctx_t *ctx = (interfaces_ctx_t *) priv;
	srpc_check_status_t check_status = srpc_check_status_none;
    interfaces_interfaces_interface_element_t *interface_head = NULL;

    struct lyd_node *interfaces_node = srpc_ly_tree_get_child_leaf(parent_container, "interfaces");
    if (interfaces_node == NULL) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "srpc_ly_tree_get_child_leaf returned NULL for 'interfaces'");
            goto error_out;
    }

    SRPLG_LOG_INF(PLUGIN_NAME, "Checking interface list");
    check_status = interfaces_check_interface(ctx, interface_head);

    switch (check_status) {
			case srpc_check_status_none:
				SRPLG_LOG_ERR(PLUGIN_NAME, "Error loading current interface list");
				goto error_out;
			case srpc_check_status_error:
				SRPLG_LOG_ERR(PLUGIN_NAME, "Error loading current interface array");
				goto error_out;
			case srpc_check_status_non_existant:
				SRPLG_LOG_INF(PLUGIN_NAME, "Storing interface array");

				error = interfaces_store_interface(ctx, interface_head);
				if (error) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "interfaces_store_interface() failed (%d)", error);
				    goto error_out;
				}
				break;
			case srpc_check_status_equal:
				SRPLG_LOG_ERR(PLUGIN_NAME, "Startup interface array is already applied on the system");
				break;
			case srpc_check_status_partial:
				/* should not be returned - treat as an error */
				SRPLG_LOG_ERR(PLUGIN_NAME, "Error loading current interface array");
				goto error_out;
	}

    goto out;

error_out:
    error = -1;
out:
    return error;
}
