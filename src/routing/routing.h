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

#ifndef ROUTING_PLUGIN_H
#define ROUTING_PLUGIN_H

#include <sysrepo.h>

int routing_sr_plugin_init_cb(sr_session_ctx_t *session, void **private_data);
void routing_sr_plugin_cleanup_cb(sr_session_ctx_t *session, void *private_data);

#endif // ROUTING_PLUGIN_H