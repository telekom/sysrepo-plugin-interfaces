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
#ifndef INTERFACES_PLUGIN_SUBSCRIPTION_CHANGE_H
#define INTERFACES_PLUGIN_SUBSCRIPTION_CHANGE_H

#include <sysrepo_types.h>

int interfaces_subscription_change_interfaces_interface(sr_session_ctx_t* session, uint32_t subscription_id, const char* module_name, const char* xpath, sr_event_t event, uint32_t request_id, void* private_data);

#endif // INTERFACES_PLUGIN_SUBSCRIPTION_CHANGE_H