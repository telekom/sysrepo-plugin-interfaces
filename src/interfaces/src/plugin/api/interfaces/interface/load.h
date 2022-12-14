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
#ifndef INTERFACES_PLUGIN_API_INTERFACES_INTERFACE_LOAD_H
#define INTERFACES_PLUGIN_API_INTERFACES_INTERFACE_LOAD_H

#include "plugin/context.h"
#include "plugin/types.h"

#include <netlink/route/link.h>

int interfaces_interface_load_name(interfaces_ctx_t* ctx, interfaces_interface_hash_element_t** element, struct rtnl_link* link);
int interfaces_interface_load_type(interfaces_ctx_t* ctx, interfaces_interface_hash_element_t** element, struct rtnl_link* link);
int interfaces_interface_load_enabled(interfaces_ctx_t* ctx, interfaces_interface_hash_element_t** element, struct rtnl_link* link);

#endif // INTERFACES_PLUGIN_API_INTERFACES_INTERFACE_LOAD_H