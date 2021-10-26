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

#ifndef DHCPV6_CLIENT_LEASE_DATA_H_ONCE
#define DHCPV6_CLIENT_LEASE_DATA_H_ONCE

#include <sysrepo.h>
#include <stdio.h>
#include <string.h>
#include <net/if.h>
#include <ctype.h>

#include "../dhcpv6_client_oper.h"
#include "../../dhcpv6_client_config/dhcpv6_client_config_file/dhcpv6_client_config_file.h"
#include "utils/memory.h"

#define DHCLIENT6_LEASE_FILE "/var/lib/dhcp/dhclient6.leases"

#define IA_ID_LEN 20
#define RENEW_LEN 20
#define INET6_ADDRSTRLEN 46
#define STARTS_LEN 20
#define PREF_LIFE_LEN 20
#define MAX_LIFE_LEN 20
#define IA_PREFIX_LEN 20
// TODO: check this
#define MAX_SERVER_ID 16 // DUID length 128 octets / 8 (bits per char) = 16 characters

int dhcpv6_client_leases_file_parse(oper_data_t *oper_data);

#endif // DHCPV6_CLIENT_LEASE_DATA_H_ONCE