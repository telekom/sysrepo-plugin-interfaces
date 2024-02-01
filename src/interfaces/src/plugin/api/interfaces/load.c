/*
 * telekom / sysrepo-plugin-system
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
#include "load.h"
#include "interface/ipv4/load.h"
#include "interface/ipv6/load.h"
#include "plugin/common.h"
#include "plugin/context.h"
#include "plugin/data/interfaces/interface.h"
#include "plugin/types.h"
#include "read.h"
#include "utils/memory.h"
#include "utlist.h"

// load APIs
#include "interface/load.h"

#include <errno.h>
#include <linux/if.h>
#include <linux/limits.h>

#include <linux/if_arp.h>

#include <netlink/addr.h>
#include <netlink/cache.h>
#include <netlink/errno.h>
#include <netlink/netlink.h>
#include <netlink/route/addr.h>
#include <netlink/route/link.h>
#include <netlink/route/link/inet.h>
#include <netlink/route/link/inet6.h>
#include <netlink/route/link/vlan.h>
#include <netlink/route/neighbour.h>
#include <netlink/route/qdisc.h>
#include <netlink/route/tc.h>
#include <netlink/socket.h>

#include <sysrepo.h>

int interfaces_load_interface(interfaces_ctx_t* ctx, interfaces_interface_hash_element_t** if_hash)
{
    int error = 0;

    // ctx
    interfaces_startup_ctx_t* startup_ctx = &ctx->startup_ctx;
    interfaces_nl_ctx_t* nl_ctx = &startup_ctx->nl_ctx;

    // temp data
    interfaces_interface_hash_element_t* new_element = NULL;
    uint8_t element_added = 0;

    struct rtnl_link* link_iter = NULL;

    // init hash
    *if_hash = interfaces_interface_hash_new();

    // socket + cache
    SRPC_SAFE_CALL_PTR(nl_ctx->socket, nl_socket_alloc(), error_out);
    SRPC_SAFE_CALL_ERR(error, nl_connect(nl_ctx->socket, NETLINK_ROUTE), error_out);
    SRPC_SAFE_CALL_ERR(error, rtnl_link_alloc_cache(nl_ctx->socket, AF_UNSPEC, &nl_ctx->link_cache), error_out);

    // get link iterator
    SRPC_SAFE_CALL_PTR(link_iter, (struct rtnl_link*)nl_cache_get_first(nl_ctx->link_cache), error_out);

    // iterate links
    while (link_iter) {
        // allocate new element
        SRPC_SAFE_CALL_PTR(new_element, interfaces_interface_hash_element_new(), error_out);
        element_added = 0;

        // load interface data
        SRPC_SAFE_CALL_ERR(error, interfaces_interface_load_name(ctx, &new_element, link_iter), error_out);
        SRPC_SAFE_CALL_ERR(error, interfaces_interface_load_type(ctx, &new_element, link_iter), error_out);
        SRPC_SAFE_CALL_ERR(error, interfaces_interface_load_enabled(ctx, &new_element, link_iter), error_out);
        // depends on sub-interfaces feature as well as on the interface type
        if (srpc_feature_status_hash_check(ctx->features.ietf_if_extensions_features, "sub-interfaces") && strcmp(new_element->interface.type, "iana-if-type:l2vlan") == 0)
        {
            SRPC_SAFE_CALL_ERR(error, interfaces_interface_load_parent_interface(nl_ctx, ctx, &new_element, link_iter), error_out);
        }

        // load interface IPv4 data
        SRPC_SAFE_CALL_ERR(error, interfaces_interface_ipv4_load_enabled(ctx, &new_element->interface.ipv4, link_iter), error_out);
        SRPC_SAFE_CALL_ERR(error, interfaces_interface_ipv4_load_forwarding(ctx, &new_element->interface.ipv4, link_iter), error_out);
        SRPC_SAFE_CALL_ERR(error, interfaces_interface_ipv4_load_mtu(ctx, &new_element->interface.ipv4, link_iter), error_out);

        // load interface IPv6 data
        SRPC_SAFE_CALL_ERR(error, interfaces_interface_ipv6_load_enabled(ctx, &new_element->interface.ipv6, link_iter), error_out);
        SRPC_SAFE_CALL_ERR(error, interfaces_interface_ipv6_load_forwarding(ctx, &new_element->interface.ipv6, link_iter), error_out);
        SRPC_SAFE_CALL_ERR(error, interfaces_interface_ipv6_load_mtu(ctx, &new_element->interface.ipv6, link_iter), error_out);

        // load IPv4 address and neighbor lists
        SRPC_SAFE_CALL_ERR(error, interfaces_interface_ipv4_load_address(ctx, &new_element->interface.ipv4, link_iter), error_out);
        SRPC_SAFE_CALL_ERR(error, interfaces_interface_ipv4_load_neighbor(ctx, &new_element->interface.ipv4, link_iter), error_out);

        // load IPv6 address and neighbor lists
        SRPC_SAFE_CALL_ERR(error, interfaces_interface_ipv6_load_address(ctx, &new_element->interface.ipv6, link_iter), error_out);
        SRPC_SAFE_CALL_ERR(error, interfaces_interface_ipv6_load_neighbor(ctx, &new_element->interface.ipv6, link_iter), error_out);

        // add element to the hash
        SRPC_SAFE_CALL_ERR(error, interfaces_interface_hash_add_element(if_hash, new_element), error_out);
        element_added = 1;

        // iterate
        link_iter = (struct rtnl_link*)nl_cache_get_next((struct nl_object*)link_iter);
    }

    goto out;

error_out:
    error = -1;

    if (!element_added) {
        interfaces_interface_hash_element_free(&new_element);
    }

out:
    // dealloc nl_ctx data

    if (nl_ctx->socket != NULL) {
        nl_socket_free(nl_ctx->socket);
    }

    if (nl_ctx->link_cache != NULL) {
        nl_cache_free(nl_ctx->link_cache);
    }

    // address and neighbor caches should be freed by their functions (_load_address and _load_neighbor)

    return error;
}