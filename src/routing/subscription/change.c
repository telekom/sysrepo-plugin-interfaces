/*
 * telekom / sysrepo-plugin-interfaces
 *
 * This program is made available under the terms of the
 * BSD 3-Clause license which is available at
 * https://opensource.org/licenses/BSD-3-Clause
 *
 * SPDX-FileCopyrightText: 2022 Deutsche Telekom AG
 * SPDX-FileContributor: Sartura Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "change.h"
#include "libyang/tree_data.h"
#include "netlink/errno.h"
#include "netlink/socket.h"
#include "route.h"
#include "route/list_hash.h"
#include "route/next_hop.h"
#include "sysrepo_types.h"
#include <linux/rtnetlink.h>
#include <routing/context.h>
#include <routing/common.h>
#include <utils/memory.h>

// stdlib
#include <net/if.h>

// sysrepo
#include <sysrepo.h>
#include <sysrepo/xpath.h>

// libnl
#include <netlink/route/route.h>

// uthash
#include <utlist.h>

// helpers
static int apply_static_routes_changes(struct routing_ctx* ctx, sr_session_ctx_t* session, const char* base_xpath);

int routing_control_plane_protocol_list_change_cb(sr_session_ctx_t* session, uint32_t subscription_id, const char* module_name, const char* xpath, sr_event_t event, uint32_t request_id, void* private_data)
{
    int error = 0;

    // context
    struct routing_ctx* ctx = (struct routing_ctx*)private_data;

    // xpath buffer
    char xpath_buffer[PATH_MAX] = { 0 };

    SRPLG_LOG_INF(PLUGIN_NAME, "Module Name: %s; XPath: %s; Event: %d, Request ID: %u", module_name, xpath, event, request_id);

    if (event == SR_EV_ABORT) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "aborting changes for: %s", xpath);
        error = -1;
        goto error_out;
    } else if (event == SR_EV_DONE) {
        error = sr_copy_config(ctx->startup_session, BASE_YANG_MODEL, SR_DS_RUNNING, 0);
        if (error) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "sr_copy_config error (%d): %s", error, sr_strerror(error));
            goto error_out;
        }
    } else if (event == SR_EV_CHANGE) {
        // ipv4 static routes changes
        error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/static-routes/ietf-ipv4-unicast-routing:ipv4//.", xpath);
        if (error < 0) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() failed (%d)", error);
            goto error_out;
        }
        error = apply_static_routes_changes(ctx, session, xpath_buffer);
        if (error) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "apply_static_routes_changes() failed (%d) - ipv4", error);
            goto error_out;
        }

        // ipv6 static routes changes
        error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/static-routes/ietf-ipv6-unicast-routing:ipv6//.", xpath);
        if (error < 0) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() failed (%d)", error);
            goto error_out;
        }
        error = apply_static_routes_changes(ctx, session, xpath_buffer);
        if (error) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "apply_static_routes_changes() failed (%d) - ipv6", error);
            goto error_out;
        }
    }
    goto out;

error_out:
    SRPLG_LOG_ERR(PLUGIN_NAME, "error applying control plane protocols module changes");
    error = -1;

out:
    return error != 0 ? SR_ERR_CALLBACK_FAILED : SR_ERR_OK;
}

static int apply_static_routes_changes(struct routing_ctx* ctx, sr_session_ctx_t* session, const char* xpath)
{
    int error = 0;

    // buffers
    char change_path[PATH_MAX] = { 0 };

    // sysrepo
    sr_change_iter_t* changes_iterator = NULL;
    sr_change_oper_t operation = SR_OP_CREATED;
    const char* prev_value = NULL;
    const char* prev_list = NULL;
    int prev_default = 0;
    const char* node_value = NULL;
    const char* node_name = NULL;

    // libnl
    struct nl_addr *prefix = NULL, *gateway = NULL;
    struct nl_sock* socket = NULL;

    // libyang
    const struct lyd_node* node = NULL;

    // new routes to add
    struct route_list_hash_element* new_routes = NULL;

    // routes to modify
    struct route_list_hash_element* modify_routes = NULL;

    // existing routes to delete
    struct route_list_hash_element* delete_routes = NULL;

    // helper value
    struct route_list_element** routes_head = NULL;

    error = sr_get_changes_iter(session, xpath, &changes_iterator);
    if (error != SR_ERR_OK) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "sr_get_changes_iter() failed (%d): %s", error, sr_strerror(error));
        goto error_out;
    }

    while (sr_get_change_tree_next(session, changes_iterator, &operation, &node, &prev_value, &prev_list, &prev_default) == SR_ERR_OK) {
        error = (lyd_path(node, LYD_PATH_STD, change_path, sizeof(change_path)) == NULL);
        if (error) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "lyd_path() error");
            goto error_out;
        }

        node_name = sr_xpath_node_name(change_path);
        node_value = lyd_get_value(node);

        SRPLG_LOG_DBG(PLUGIN_NAME, "Node Path: %s", change_path);
        SRPLG_LOG_DBG(PLUGIN_NAME, "Node Name: %s", node_name);
        SRPLG_LOG_DBG(PLUGIN_NAME, "Previous Value: %s; Value: %s; Operation: %d", prev_value, node_value, operation);

        switch (operation) {
        case SR_OP_CREATED:
            // data needed: destination-prefix and next-hop address
            if (!strncmp(node_name, "destination-prefix", sizeof("destination-prefix") - 1)) {
                // create new route
                error = nl_addr_parse(node_value, AF_UNSPEC, &prefix);
                if (error != 0) {
                    SRPLG_LOG_ERR(PLUGIN_NAME, "nl_addr_parse() error (%d): %s", error, nl_geterror(error));
                    goto error_out;
                }

                // add empty route to the start of the list for 'prefix'
                route_list_hash_add(&new_routes, prefix, &(struct route) { 0 });
                routes_head = route_list_hash_get(&new_routes, prefix);

                // free created prefix
                nl_addr_put(prefix);
            } else if (!strncmp(node_name, "description", sizeof("description") - 1)) {
                if (!routes_head) {
                    // error
                    SRPLG_LOG_ERR(PLUGIN_NAME, "invalid routes_head value");
                    goto error_out;
                } else {
                    route_set_description(&(*routes_head)->route, node_value);
                }
            } else if (!strncmp(node_name, "next-hop-address", sizeof("next-hop-address") - 1)) {
                // simple next hop
                if (!routes_head) {
                    SRPLG_LOG_ERR(PLUGIN_NAME, "invalid routes_head value");
                    goto error_out;
                } else {
                    error = nl_addr_parse(node_value, AF_UNSPEC, &gateway);
                    if (error != 0) {
                        SRPLG_LOG_ERR(PLUGIN_NAME, "nl_addr_parse() error (%d): %s", error, nl_geterror(error));
                        goto error_out;
                    }
                    route_next_hop_set_simple_gateway(&(*routes_head)->route.next_hop, gateway);

                    // free created gateway
                    nl_addr_put(gateway);
                }
            }
            break;
        case SR_OP_MODIFIED:
            // data needed: next-hop container value
            break;
        case SR_OP_DELETED:
            // data needed: destination-prefix (optional next-hop value)
            if (!strncmp(node_name, "destination-prefix", sizeof("destination-prefix") - 1)) {
                // create new route
                error = nl_addr_parse(node_value, AF_UNSPEC, &prefix);
                if (error != 0) {
                    SRPLG_LOG_ERR(PLUGIN_NAME, "nl_addr_parse() error (%d): %s", error, nl_geterror(error));
                    goto error_out;
                }

                // add empty route to the start of the list for 'prefix'
                route_list_hash_add(&delete_routes, prefix, &(struct route) { 0 });
                routes_head = route_list_hash_get(&delete_routes, prefix);

                // free created prefix
                nl_addr_put(prefix);
            } else if (!strncmp(node_name, "description", sizeof("description") - 1)) {
                if (!routes_head) {
                    // error
                    SRPLG_LOG_ERR(PLUGIN_NAME, "invalid routes_head value");
                    goto error_out;
                } else {
                    route_set_description(&(*routes_head)->route, node_value);
                }
            } else if (!strncmp(node_name, "next-hop-address", sizeof("next-hop-address") - 1)) {
                // simple next hop
                if (!routes_head) {
                    SRPLG_LOG_ERR(PLUGIN_NAME, "invalid routes_head value");
                    goto error_out;
                } else {
                    error = nl_addr_parse(node_value, AF_UNSPEC, &gateway);
                    if (error != 0) {
                        SRPLG_LOG_ERR(PLUGIN_NAME, "nl_addr_parse() error (%d): %s", error, nl_geterror(error));
                        goto error_out;
                    }
                    route_next_hop_set_simple_gateway(&(*routes_head)->route.next_hop, gateway);

                    // free created gateway
                    nl_addr_put(gateway);
                }
            }
            break;
        default:
            break;
        }
    }

    // allocate libnl socket for all modifications (new, modify and delete)
    socket = nl_socket_alloc();
    if (socket == NULL) {
        error = -1;
        SRPLG_LOG_ERR(PLUGIN_NAME, "unable to init nl_sock struct...");
        goto error_out;
    }

    error = nl_connect(socket, NETLINK_ROUTE);
    if (error != 0) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "nl_connect() failed (%d): %s", error, nl_geterror(error));
        goto error_out;
    }

    SRPLG_LOG_INF(PLUGIN_NAME, "applying recieved changes for static routes");

    error = routing_apply_new_routes(socket, new_routes);
    if (error) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "routing_apply_new_routes() error (%d)", error);
        goto error_out;
    }

    error = routing_apply_modify_routes(socket, modify_routes);
    if (error) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "routing_apply_modify_routes() error (%d)", error);
        goto error_out;
    }

    error = routing_apply_delete_routes(socket, delete_routes);
    if (error) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "routing_apply_delete_routes() error (%d)", error);
        goto error_out;
    }

    goto out;

error_out:
    error = -1;

out:
    // libnl
    if (socket) {
        nl_socket_free(socket);
    }

    // list hashes
    route_list_hash_free(&new_routes);
    route_list_hash_free(&modify_routes);
    route_list_hash_free(&delete_routes);

    // iterator
    sr_free_change_iter(changes_iterator);

    return error;
}