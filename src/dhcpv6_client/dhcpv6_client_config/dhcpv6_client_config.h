/*
 * telekom / sysrepo-plugin-interfaces
 *
 * This program is made available under the terms of the
 * BSD 3-Clause license which is available at
 * https://opensource.org/licenses/BSD-3-Clause
 *
 * SPDX-FileCopyrightText: 2021 Deutsche Telekom AG
 * SPDX-FileContributor: Sartura Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef DHCPV6_CLIENT_CONFIG_ONCE_H
#define DHCPV6_CLIENT_CONFIG_ONCE_H

#include "../dhcpv6_client_sub/dhcpv6_client_sub.h"
#include "../dhcpv6_client_list/dhcpv6_client_list.h"
#include "utils/memory.h"

#include <sysrepo.h>
#include <sysrepo/xpath.h>

int dhcpv6_client_config_load_data(sr_session_ctx_t *session);
int dhcpv6_client_config_set_value(sr_session_ctx_t *session, sr_change_oper_t operation, const char *xpath, const char *value, const char *prev_value, config_data_list_t *ccl);

#endif /* DHCPV6_CLIENT_CONFIG_ONCE_H */