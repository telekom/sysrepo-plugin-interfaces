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
#ifndef INTERFACES_PLUGIN_API_INTERFACES_INTERFACE_IPV4_NEIGHBOR_LOAD_H
#define INTERFACES_PLUGIN_API_INTERFACES_INTERFACE_IPV4_NEIGHBOR_LOAD_H

#include "netlink/route/neighbour.h"
#include "plugin/context.h"
#include "plugin/types.h"

int interfaces_interface_ipv4_neighbor_load_ip(interfaces_ctx_t* ctx, interfaces_interface_ipv4_neighbor_element_t** element, struct rtnl_neigh* neigh);
int interfaces_interface_ipv4_neighbor_load_link_layer_address(interfaces_ctx_t* ctx, interfaces_interface_ipv4_neighbor_element_t** element, struct rtnl_neigh* neigh);

#endif // INTERFACES_PLUGIN_API_INTERFACES_INTERFACE_IPV4_NEIGHBOR_LOAD_H
