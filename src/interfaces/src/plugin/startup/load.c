#include "load.h"
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
#undef INTERFACES_PLUGIN_LOAD_STARTUP
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
    interfaces_interface_hash_element_t* interface_head = NULL;
    interfaces_interface_hash_element_t *iter = NULL, *tmp = NULL;

    interfaces_interface_ipv4_address_element_t* v4_addr_iter = NULL;
    interfaces_interface_ipv6_address_element_t* v6_addr_iter = NULL;

    interfaces_interface_ipv4_neighbor_element_t* v4_neigh_iter = NULL;
    interfaces_interface_ipv6_neighbor_element_t* v6_neigh_iter = NULL;

    SRPC_SAFE_CALL_ERR(error, interfaces_load_interface(ctx, &interface_head), error_out);

    HASH_ITER(hh, interface_head, iter, tmp)
    {
        SRPLG_LOG_INF(PLUGIN_NAME, "Name: %s", iter->interface.name);
        SRPLG_LOG_INF(PLUGIN_NAME, "Type: %s", iter->interface.type);
        SRPLG_LOG_INF(PLUGIN_NAME, "Enabled: %d", iter->interface.enabled);

        // v4
        SRPLG_LOG_INF(PLUGIN_NAME, "v4 MTU: %d", iter->interface.ipv4.mtu);
        SRPLG_LOG_INF(PLUGIN_NAME, "v4 Enabled: %d", iter->interface.ipv4.enabled);
        SRPLG_LOG_INF(PLUGIN_NAME, "v4 Forwarding: %d", iter->interface.ipv4.forwarding);

        LL_FOREACH(iter->interface.ipv4.address, v4_addr_iter)
        {
            SRPLG_LOG_INF(PLUGIN_NAME, "v4 Address : %s/%d", v4_addr_iter->address.ip, v4_addr_iter->address.subnet.prefix_length);
        }
        LL_FOREACH(iter->interface.ipv4.neighbor, v4_neigh_iter)
        {
            SRPLG_LOG_INF(PLUGIN_NAME, "v4 Neighbor : %s : %s", v4_neigh_iter->neighbor.ip, v4_neigh_iter->neighbor.link_layer_address);
        }

        // v6
        SRPLG_LOG_INF(PLUGIN_NAME, "v6 MTU: %d", iter->interface.ipv6.mtu);
        SRPLG_LOG_INF(PLUGIN_NAME, "v6 Enabled: %d", iter->interface.ipv6.enabled);
        SRPLG_LOG_INF(PLUGIN_NAME, "v6 Forwarding: %d", iter->interface.ipv6.forwarding);

        LL_FOREACH(iter->interface.ipv6.address, v6_addr_iter)
        {
            SRPLG_LOG_INF(PLUGIN_NAME, "v6 Address : %s/%d", v6_addr_iter->address.ip, v6_addr_iter->address.prefix_length);
        }
        LL_FOREACH(iter->interface.ipv6.neighbor, v6_neigh_iter)
        {
            SRPLG_LOG_INF(PLUGIN_NAME, "v6 Neighbor : %s : %s", v6_neigh_iter->neighbor.ip, v6_neigh_iter->neighbor.link_layer_address);
        }
    }

    goto out;

error_out:
    error = -1;
out:
    interfaces_interface_hash_free(&interface_head);

    return error;
}
