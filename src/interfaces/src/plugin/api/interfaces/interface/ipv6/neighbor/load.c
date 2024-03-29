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
#include "netlink/route/neighbour.h"
#include "plugin/data/interfaces/interface/ipv6/neighbor.h"

int interfaces_interface_ipv6_neighbor_load_ip(interfaces_ctx_t* ctx, interfaces_interface_ipv6_neighbor_element_t** element, struct rtnl_neigh* neigh)
{
    int error = 0;
    void* error_ptr = NULL;
    char ip_buffer[100] = { 0 };

    // convert to nl_addr
    struct nl_addr* dst = rtnl_neigh_get_dst(neigh);

    // parse to string
    SRPC_SAFE_CALL_PTR(error_ptr, nl_addr2str(dst, ip_buffer, sizeof(ip_buffer)), error_out);

    char* prefix = strchr(ip_buffer, '/');
    if (prefix) {
        *prefix = 0;
    }

    // set IP
    SRPC_SAFE_CALL_ERR(error, interfaces_interface_ipv6_neighbor_element_set_ip(element, ip_buffer), error_out);

    goto out;

error_out:
    error = -1;

out:

    return error;
}

int interfaces_interface_ipv6_neighbor_load_link_layer_address(interfaces_ctx_t* ctx, interfaces_interface_ipv6_neighbor_element_t** element, struct rtnl_neigh* neigh)
{
    int error = 0;
    void* error_ptr = NULL;
    char lladdr_buffer[100] = { 0 };

    // convert to nl_addr
    struct nl_addr* dst = rtnl_neigh_get_lladdr(neigh);

    // parse to string
    SRPC_SAFE_CALL_PTR(error_ptr, nl_addr2str(dst, lladdr_buffer, sizeof(lladdr_buffer)), error_out);

    // set lladdr
    SRPC_SAFE_CALL_ERR(error, interfaces_interface_ipv6_neighbor_element_set_link_layer_address(element, lladdr_buffer), error_out);

    goto out;

error_out:
    error = -1;

out:

    return error;
}
