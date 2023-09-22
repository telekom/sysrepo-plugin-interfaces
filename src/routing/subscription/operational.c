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

#include "operational.h"
#include <routing/common.h>
#include <routing/rib.h>
#include <routing/rib/list.h>
#include <routing/route.h>
#include <routing/route/list_hash.h>
#include <routing/control_plane_protocol.h>

// sysrepo
#include <sysrepo.h>

// libnl
#include <netlink/route/link.h>
#include <netlink/route/route.h>

// uthash
#include <utlist.h>

int routing_oper_get_rib_routes_cb(sr_session_ctx_t* session, uint32_t subscription_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    int nl_err = 0;
    LY_ERR ly_err = LY_SUCCESS;

    // libyang
    const struct ly_ctx* ly_ctx = NULL;
    const struct lys_module *ly_uv4mod = NULL, *ly_uv6mod = NULL;
    struct lyd_node *ly_node = NULL, *routes_node = NULL, *nh_node = NULL, *nh_list_node = NULL;

    // libnl
    struct nl_sock* socket = NULL;
    struct nl_cache* cache = NULL;
    struct nl_cache* link_cache = NULL;
    struct rib_list_element *ribs_head = NULL, *ribs_iter = NULL;
    struct route_list_hash_element* routes_hash_iter = NULL;
    struct route_list_element* routes_iter = NULL;

    // temp buffers
    char routes_buffer[PATH_MAX];
    char value_buffer[PATH_MAX];
    char ip_buffer[INET6_ADDRSTRLEN];
    char prefix_buffer[INET6_ADDRSTRLEN + 1 + 3];
    char xpath_buffer[256] = { 0 };

    ly_ctx = sr_acquire_context(sr_session_get_connection(session));

    ly_uv4mod = ly_ctx_get_module(ly_ctx, "ietf-ipv4-unicast-routing", "2018-03-13");
    ly_uv6mod = ly_ctx_get_module(ly_ctx, "ietf-ipv6-unicast-routing", "2018-03-13");

    socket = nl_socket_alloc();
    if (socket == NULL) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "unable to init nl_sock struct...");
        goto error_out;
    }

    nl_err = nl_connect(socket, NETLINK_ROUTE);
    if (nl_err != 0) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "nl_connect failed (%d): %s", nl_err, nl_geterror(nl_err));
        goto error_out;
    }

    nl_err = rtnl_route_alloc_cache(socket, AF_UNSPEC, 0, &cache);
    if (nl_err != 0) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_route_alloc_cache failed (%d): %s", nl_err, nl_geterror(nl_err));
        goto error_out;
    }

    nl_err = rtnl_link_alloc_cache(socket, AF_UNSPEC, &link_cache);
    if (nl_err != 0) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_alloc_cache failed (%d): %s", nl_err, nl_geterror(nl_err));
        goto error_out;
    }

    nl_err = rtnl_route_read_table_names("/etc/iproute2/rt_tables");
    if (nl_err != 0) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_route_read_table_names failed (%d): %s", nl_err, nl_geterror(nl_err));
        goto error_out;
    }

    nl_err = rtnl_route_read_protocol_names("/etc/iproute2/rt_protos");
    if (nl_err != 0) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_route_read_table_names failed (%d): %s", nl_err, nl_geterror(nl_err));
        goto error_out;
    }

    error = routing_collect_routes(cache, link_cache, &ribs_head);
    if (error != 0) {
        goto error_out;
    }

    LL_FOREACH(ribs_head, ribs_iter)
    {
        // create new routes container for every table
        struct route_list_hash_element** routes_hash_head = &ribs_iter->rib.routes_head;
        const int ADDR_FAMILY = ribs_iter->rib.address_family;
        const char* TABLE_NAME = ribs_iter->rib.name;
        snprintf(routes_buffer, sizeof(routes_buffer), "%s[name='%s-%s']/routes", ROUTING_RIB_LIST_YANG_PATH, ADDR_FAMILY == AF_INET ? "ipv4" : "ipv6", TABLE_NAME);
        ly_err = lyd_new_path(*parent, ly_ctx, routes_buffer, NULL, LYD_NEW_PATH_UPDATE, &routes_node);
        if (ly_err != LY_SUCCESS) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "unable to create new routes node");
            goto error_out;
        }

        LL_FOREACH(*routes_hash_head, routes_hash_iter)
        {
            struct route_list_element** routes_list_head = &routes_hash_iter->routes_head;
            struct nl_addr* dst_prefix = routes_hash_iter->prefix;
            nl_addr2str(dst_prefix, ip_buffer, sizeof(ip_buffer));

            // check for prefix - libnl doesn't write prefix into the buffer if its 8*4/8*6 i.e. only that address/no subnet
            if (strchr(ip_buffer, '/') == NULL) {
                snprintf(prefix_buffer, sizeof(prefix_buffer), "%s/%d", ip_buffer, nl_addr_get_prefixlen(dst_prefix));
            } else {
                snprintf(prefix_buffer, sizeof(prefix_buffer), "%s", ip_buffer);
            }

            LL_FOREACH(*routes_list_head, routes_iter)
            {
                const struct route* ROUTE = &routes_iter->route;
                // create a new list and after that add properties to it
                ly_err = lyd_new_path(routes_node, ly_ctx, "routes/route", NULL, 0, &ly_node);
                if (ly_err != LY_SUCCESS) {
                    SRPLG_LOG_ERR(PLUGIN_NAME, "unable to create new route node");
                    goto error_out;
                }

                // route-preference
                snprintf(value_buffer, sizeof(value_buffer), "%u", ROUTE->preference);
                SRPLG_LOG_DBG(PLUGIN_NAME, "route-preference = %s", value_buffer);
                ly_err = lyd_new_path(ly_node, ly_ctx, "route-preference", (void*)value_buffer, 0, NULL);
                if (ly_err != LY_SUCCESS) {
                    SRPLG_LOG_ERR(PLUGIN_NAME, "unable to create new route-preference node");
                    goto error_out;
                }

                // next-hop container
                const union route_next_hop_value* NEXTHOP = &ROUTE->next_hop.value;
                ly_err = lyd_new_path(ly_node, ly_ctx, "next-hop", NULL, 0, &nh_node);
                if (ly_err != LY_SUCCESS) {
                    SRPLG_LOG_ERR(PLUGIN_NAME, "unable to create new next-hop node");
                    goto error_out;
                }

                switch (ROUTE->next_hop.kind) {
                case route_next_hop_kind_none:
                    break;
                case route_next_hop_kind_simple: {
                    struct rtnl_link* iface = rtnl_link_get(link_cache, ROUTE->next_hop.value.simple.ifindex);
                    const char* if_name = rtnl_link_get_name(iface);

                    // outgoing-interface
                    SRPLG_LOG_DBG(PLUGIN_NAME, "outgoing-interface = %s", if_name);
                    ly_err = lyd_new_path(nh_node, ly_ctx, "outgoing-interface", (void*)if_name, 0, NULL);
                    if (ly_err != LY_SUCCESS) {
                        SRPLG_LOG_ERR(PLUGIN_NAME, "unable to create new outgoing-interface node");
                        goto error_out;
                    }

                    // next-hop-address
                    if (NEXTHOP->simple.addr) {
                        nl_addr2str(NEXTHOP->simple.addr, ip_buffer, sizeof(ip_buffer));
                        if (ADDR_FAMILY == AF_INET && ly_uv4mod != NULL) {
                            SRPLG_LOG_DBG(PLUGIN_NAME, "IPv4 next-hop-address = %s", ip_buffer);
                            ly_err = lyd_new_term(nh_node, ly_uv4mod, "next-hop-address", ip_buffer, false, NULL);
                            if (ly_err != LY_SUCCESS) {
                                SRPLG_LOG_ERR(PLUGIN_NAME, "unable to create new IPv4 next-hop-address node");
                                goto error_out;
                            }
                        } else if (ADDR_FAMILY == AF_INET6 && ly_uv6mod != NULL) {
                            SRPLG_LOG_DBG(PLUGIN_NAME, "IPv6 next-hop-address = %s", ip_buffer);
                            ly_err = lyd_new_term(nh_node, ly_uv6mod, "next-hop-address", ip_buffer, false, NULL);
                            if (ly_err != LY_SUCCESS) {
                                SRPLG_LOG_ERR(PLUGIN_NAME, "unable to create new IPv6 next-hop-address node");
                                goto error_out;
                            }
                        }
                    }
                    rtnl_link_put(iface);
                    break;
                }
                case route_next_hop_kind_special:
                    // SRPLG_LOG_DBG(PLUGIN_NAME, "special-next-hop = %s", NEXTHOP->special.value);
                    // ly_err = lyd_new_path(nh_node, ly_ctx, "special-next-hop", NEXTHOP->special.value, false, NULL);
                    // if (ly_err != LY_SUCCESS) {
                    // 	SRPLG_LOG_ERR(PLUGIN_NAME, "unable to create new special-next-hop node");
                    // 	goto error_out;
                    // }
                    break;
                case route_next_hop_kind_list: {
                    struct route_next_hop_list_element* nexthop_iter = NULL;

                    LL_FOREACH(ROUTE->next_hop.value.list_head, nexthop_iter)
                    {
                        struct rtnl_link* iface = rtnl_link_get(link_cache, nexthop_iter->simple.ifindex);
                        const char* if_name = rtnl_link_get_name(iface);

                        error = snprintf(xpath_buffer, sizeof(xpath_buffer), "next-hop/next-hop-list/next-hop[index=%d]", nexthop_iter->simple.ifindex);
                        if (error < 0) {
                            SRPLG_LOG_ERR(PLUGIN_NAME, "unable to create new next-hop-list/next-hop node, couldn't retrieve interface index");
                            goto error_out;
                        }
                        ly_err = lyd_new_path(nh_node, ly_ctx, xpath_buffer, NULL, 0, &nh_list_node);
                        if (ly_err != LY_SUCCESS) {
                            SRPLG_LOG_ERR(PLUGIN_NAME, "unable to create new next-hop-list/next-hop node");
                            goto error_out;
                        }

                        // outgoing-interface
                        SRPLG_LOG_DBG(PLUGIN_NAME, "outgoing-interface = %s", if_name);
                        ly_err = lyd_new_path(nh_list_node, ly_ctx, "outgoing-interface", (void*)if_name, 0, NULL);
                        if (ly_err != LY_SUCCESS) {
                            SRPLG_LOG_ERR(PLUGIN_NAME, "unable to create new outgoing-interface node");
                            goto error_out;
                        }

                        // next-hop-address
                        if (nexthop_iter->simple.addr) {
                            nl_addr2str(nexthop_iter->simple.addr, ip_buffer, sizeof(ip_buffer));
                            if (ADDR_FAMILY == AF_INET && ly_uv4mod != NULL) {
                                SRPLG_LOG_DBG(PLUGIN_NAME, "IPv4 next-hop/next-hop-list/next-hop/next-hop-address = %s", ip_buffer);
                                ly_err = lyd_new_term(nh_list_node, ly_uv4mod, "next-hop-address", ip_buffer, false, NULL);
                                if (ly_err != LY_SUCCESS) {
                                    SRPLG_LOG_ERR(PLUGIN_NAME, "unable to create new IPv4 next-hop-address node");
                                    goto error_out;
                                }
                            } else if (ADDR_FAMILY == AF_INET6 && ly_uv6mod != NULL) {
                                SRPLG_LOG_DBG(PLUGIN_NAME, "IPv6 next-hop/next-hop-list/next-hop/next-hop-address = %s", ip_buffer);
                                ly_err = lyd_new_term(nh_list_node, ly_uv6mod, "next-hop-address", ip_buffer, false, NULL);
                                if (ly_err != LY_SUCCESS) {
                                    SRPLG_LOG_ERR(PLUGIN_NAME, "unable to create new IPv6 next-hop-address node");
                                    goto error_out;
                                }
                            }
                        }
                        rtnl_link_put(iface);
                    }
                    break;
                }
                }

                // destination-prefix
                if (ADDR_FAMILY == AF_INET && ly_uv4mod != NULL) {
                    if (strncmp("none/0", prefix_buffer, strnlen(prefix_buffer, sizeof(prefix_buffer))) == 0) {
                        strcpy(prefix_buffer, "0.0.0.0/0");
                    }

                    SRPLG_LOG_DBG(PLUGIN_NAME, "destination-prefix = %s", prefix_buffer);
                    ly_err = lyd_new_term(ly_node, ly_uv4mod, "destination-prefix", prefix_buffer, false, NULL);
                    if (ly_err != LY_SUCCESS) {
                        SRPLG_LOG_ERR(PLUGIN_NAME, "unable to create new IPv4 destination-prefix node");
                        goto error_out;
                    }
                } else if (ADDR_FAMILY == AF_INET6 && ly_uv6mod != NULL) {
                    if (strncmp("none/0", prefix_buffer, strnlen(prefix_buffer, sizeof(prefix_buffer))) == 0) {
                        strcpy(prefix_buffer, "::/0");
                    }

                    SRPLG_LOG_DBG(PLUGIN_NAME, "destination-prefix = %s", prefix_buffer);
                    ly_err = lyd_new_term(ly_node, ly_uv6mod, "destination-prefix", prefix_buffer, false, NULL);
                    if (ly_err != LY_SUCCESS) {
                        SRPLG_LOG_ERR(PLUGIN_NAME, "unable to create new IPv6 destination-prefix node");
                        goto error_out;
                    }
                }

                // route-metadata/source-protocol
                SRPLG_LOG_DBG(PLUGIN_NAME, "source-protocol = %s", ROUTE->metadata.source_protocol);
                ly_err = lyd_new_path(ly_node, ly_ctx, "source-protocol", ROUTE->metadata.source_protocol, 0, NULL);
                if (ly_err != LY_SUCCESS) {
                    SRPLG_LOG_ERR(PLUGIN_NAME, "unable to create new source-protocol node");
                    goto error_out;
                }

                // route-metadata/active
                if (ROUTE->metadata.active == 1) {
                    SRPLG_LOG_DBG(PLUGIN_NAME, "active = %d", ROUTE->metadata.active);
                    ly_err = lyd_new_path(ly_node, ly_ctx, "active", NULL, 0, NULL);
                    if (ly_err != LY_SUCCESS) {
                        SRPLG_LOG_ERR(PLUGIN_NAME, "unable to create new active node");
                        goto error_out;
                    }
                }
            }
        }
    }

    if (error != 0) {
        goto error_out;
    }

    goto out;

error_out:
    SRPLG_LOG_ERR(PLUGIN_NAME, "unable to return routes for routing tables");
    error = SR_ERR_CALLBACK_FAILED;

out:
    rib_list_free(&ribs_head);
    nl_socket_free(socket);
    nl_cache_free(cache);
    nl_cache_free(link_cache);
    return error;
}

int routing_oper_get_interfaces_cb(sr_session_ctx_t* session, uint32_t subscription_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    // error handling
    int error = SR_ERR_OK;
    LY_ERR ly_err = LY_SUCCESS;

    // libyang
    const struct ly_ctx* ly_ctx = NULL;

    // libnl
    struct nl_sock* socket = NULL;
    struct nl_cache* cache = NULL;
    struct rtnl_link* link = NULL;

    if (*parent == NULL) {
        ly_ctx = sr_acquire_context(sr_session_get_connection(session));
        if (ly_ctx == NULL) {
            goto error_out;
        }

        ly_err = lyd_new_path(*parent, ly_ctx, request_xpath, NULL, 0, NULL);
        if (ly_err != LY_SUCCESS) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "unable to create new node");
            goto error_out;
        }
    }

    socket = nl_socket_alloc();
    if (socket == NULL) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "unable to init nl_sock struct...");
        goto error_out;
    }

    error = nl_connect(socket, NETLINK_ROUTE);
    if (error != 0) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "nl_connect failed (%d): %s", error, nl_geterror(error));
        goto error_out;
    }

    error = rtnl_link_alloc_cache(socket, AF_UNSPEC, &cache);
    if (error != 0) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_alloc_cache failed (%d): %s", error, nl_geterror(error));
        goto error_out;
    }

    SRPLG_LOG_DBG(PLUGIN_NAME, "adding interfaces to the list");

    link = (struct rtnl_link*)nl_cache_get_first(cache);
    while (link) {
        const char* name = rtnl_link_get_name(link);
        SRPLG_LOG_DBG(PLUGIN_NAME, "adding interface '%s' ", name);

        ly_err = lyd_new_path(*parent, ly_ctx, ROUTING_INTERFACE_LEAF_LIST_YANG_PATH, (void*)name, 0, NULL);
        if (ly_err != LY_SUCCESS) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "unable to create new interface node");
            goto error_out;
        }

        link = (struct rtnl_link*)nl_cache_get_next((struct nl_object*)link);
    }

    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;
out:
    // free allocated objects
    nl_cache_free(cache);
    nl_socket_free(socket);

    return error;
}