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

#ifndef ROUTING_PLUGIN_PERSIST_H
#define ROUTING_PLUGIN_PERSIST_H

#include <sysrepo_types.h>

int routing_persist_static_routes(sr_session_ctx_t *session);

#endif // ROUTING_PLUGIN_PERSIST_H