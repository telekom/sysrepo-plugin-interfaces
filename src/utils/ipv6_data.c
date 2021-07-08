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

#include "utils/ipv6_data.h"
#include <string.h>
#include <stdlib.h>

void ipv6_data_init(ipv6_data_t *ipv6)
{
	ipv6_autoconf_init(&ipv6->autoconf);
	ip_data_init(&ipv6->ip_data);
}

void ipv6_data_set_cga(ipv6_data_t *ipv6, char *cga)
{
	ipv6->autoconf.create_global_addr = (strcmp(cga, "true") == 0) ? 1 : 0;
}

void ipv6_data_set_cta(ipv6_data_t *ipv6, char *cta)
{
	ipv6->autoconf.create_temp_addr = (strcmp(cta, "true") == 0) ? 1 : 0;
}

void ipv6_data_set_tvl(ipv6_data_t *ipv6, char *tvl)
{
	ipv6->autoconf.temp_valid_lifetime = (uint32_t) atoi(tvl);
}

void ipv6_data_set_tpl(ipv6_data_t *ipv6, char *tpl)
{
	ipv6->autoconf.temp_preffered_lifetime = (uint32_t) atoi(tpl);
}

void ipv6_data_free(ipv6_data_t *ipv6)
{
	ipv6_autoconf_init(&ipv6->autoconf);
	ip_data_free(&ipv6->ip_data);
}

void ipv6_autoconf_init(ipv6_autoconf_t *a)
{
	a->create_global_addr = 0;
	a->create_temp_addr = 0;
	a->temp_preffered_lifetime = 0;
	a->temp_valid_lifetime = 0;
}
