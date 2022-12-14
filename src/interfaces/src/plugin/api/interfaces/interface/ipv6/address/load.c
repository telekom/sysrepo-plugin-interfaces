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
#include "netlink/addr.h"
#include "netlink/route/addr.h"
#include "plugin/data/interfaces/interface/ipv6/address.h"

int interfaces_interface_ipv6_address_load_ip(interfaces_ctx_t* ctx, interfaces_interface_ipv6_address_element_t** element, struct rtnl_addr* addr)
{
    int error = 0;
    void* error_ptr = NULL;
    char ip_buffer[100] = { 0 };

    // convert to nl_addr
    struct nl_addr* local = rtnl_addr_get_local(addr);

    // parse to string
    SRPC_SAFE_CALL_PTR(error_ptr, nl_addr2str(local, ip_buffer, sizeof(ip_buffer)), error_out);

    char* prefix = strchr(ip_buffer, '/');
    if (prefix) {
        *prefix = 0;
    }

    // set IP
    SRPC_SAFE_CALL_ERR(error, interfaces_interface_ipv6_address_element_set_ip(element, ip_buffer), error_out);

    goto out;

error_out:
    error = -1;

out:

    return error;
}

int interfaces_interface_ipv6_address_load_prefix_length(interfaces_ctx_t* ctx, interfaces_interface_ipv6_address_element_t** element, struct rtnl_addr* addr)
{
    int error = 0;

    const uint8_t prefix = (uint8_t)rtnl_addr_get_prefixlen(addr);

    // set prefix length
    SRPC_SAFE_CALL_ERR(error, interfaces_interface_ipv6_address_element_set_prefix_length(element, prefix), error_out);

    goto out;

error_out:
    error = -1;

out:
    return error;
}
