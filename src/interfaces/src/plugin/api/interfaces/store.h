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
#ifndef INTERFACES_PLUGIN_API_INTERFACES_STORE_H
#define INTERFACES_PLUGIN_API_INTERFACES_STORE_H

#include "plugin/context.h"
#include <utarray.h>

int interfaces_store_interface(interfaces_ctx_t* ctx, const interfaces_interface_hash_element_t* if_hash);

#endif // INTERFACES_PLUGIN_API_INTERFACES_STORE_H
