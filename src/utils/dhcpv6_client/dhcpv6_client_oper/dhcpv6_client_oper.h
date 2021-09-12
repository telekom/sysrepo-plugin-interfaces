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

#include <sysrepo.h>
#include <sysrepo/xpath.h>
#include <libyang/libyang.h>
#include <libyang/tree_data.h>

#include "../../memory.h"

#ifndef DHCPV6_CLIENT_OPER_DATA_H_ONCE
#define DHCPV6_CLIENT_OPER_DATA_H_ONCE

#define MAX_IA_NA 5
#define MAX_IA_PD 5
#define MAX_IFS 5

struct ia_na_s{
	char *ia_id;
	char *ia_na_address;
	char *last_renew_rebind;
	char *allocation_time;
	char *preferred_lifetime;
	char *valid_lifetime;
	char *server_duid;
};

typedef struct ia_na_s ia_na_t;

struct ia_pd_s{
	char *ia_id;
	char *iaprefix;
	char *last_renew_rebind;
	char *allocation_time;
	char *preferred_lifetime;
	char *valid_lifetime;
	char *server_duid;
};

typedef struct ia_pd_s ia_pd_t;

struct oper_if_data_s{
	char *if_name;
	ia_na_t ia_na[MAX_IA_NA];
	ia_pd_t ia_pd[MAX_IA_PD];
	int na_count;
	int pd_count;
};

typedef struct oper_if_data_s oper_if_data_t;

struct oper_data_s{
	oper_if_data_t if_data[MAX_IFS];
	int count;
};

typedef struct oper_data_s oper_data_t;

int dhcpv6_client_oper_create(oper_data_t *oper_data, struct lyd_node **parent, const struct ly_ctx *ly_ctx);
void dhcpv6_client_oper_cleanup(oper_data_t *oper_data);

#endif // DHCPV6_CLIENT_OPER_DATA_H_ONCE