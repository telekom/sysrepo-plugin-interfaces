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

#ifndef IPv6_DATA_H_ONCE
#define IPv6_DATA_H_ONCE

#include "ip_data.h"

typedef struct ipv6_data_s ipv6_data_t;
typedef struct ipv6_autoconf_s ipv6_autoconf_t;

struct ipv6_autoconf_s {
	uint8_t create_global_addr;
	uint8_t create_temp_addr;
	uint32_t temp_valid_lifetime;
	uint32_t temp_preffered_lifetime;
};

struct ipv6_data_s {
	ip_data_t ip_data;
	ipv6_autoconf_t autoconf;
};

void ipv6_data_init(ipv6_data_t *ipv6);
void ipv6_data_set_cga(ipv6_data_t *ipv6, char *cga);
void ipv6_data_set_cta(ipv6_data_t *ipv6, char *cta);
void ipv6_data_set_tvl(ipv6_data_t *ipv6, char *tvl);
void ipv6_data_set_tpl(ipv6_data_t *ipv6, char *tpl);
void ipv6_data_free(ipv6_data_t *ipv6);

void ipv6_autoconf_init(ipv6_autoconf_t *a);

#endif // IPv4_DATA_H_ONCE
