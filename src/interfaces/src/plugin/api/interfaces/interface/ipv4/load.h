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
#ifndef INTERFACES_PLUGIN_API_INTERFACES_INTERFACE_IPV4_LOAD_H
#define INTERFACES_PLUGIN_API_INTERFACES_INTERFACE_IPV4_LOAD_H

#include "plugin/context.h"
#include "plugin/data/interfaces/interface.h"
#include "plugin/data/interfaces/interface/linked_list.h"
#include "plugin/types.h"

int interfaces_interface_ipv4_load_enabled(interfaces_ctx_t* ctx, interfaces_interface_ipv4_t* ipv4, struct rtnl_link* link);
int interfaces_interface_ipv4_load_forwarding(interfaces_ctx_t* ctx, interfaces_interface_ipv4_t* ipv4, struct rtnl_link* link);
int interfaces_interface_ipv4_load_mtu(interfaces_ctx_t* ctx, interfaces_interface_ipv4_t* ipv4, struct rtnl_link* link);
int interfaces_interface_ipv4_load_address(interfaces_ctx_t* ctx, interfaces_interface_ipv4_t* ipv4, struct rtnl_link* link);
int interfaces_interface_ipv4_load_neighbor(interfaces_ctx_t* ctx, interfaces_interface_ipv4_t* ipv4, struct rtnl_link* link);

#endif // INTERFACES_PLUGIN_API_INTERFACES_INTERFACE_IPV4_LOAD_H
