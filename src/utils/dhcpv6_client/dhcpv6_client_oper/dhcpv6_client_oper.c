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

#include "dhcpv6_client_oper.h"

int dhcpv6_client_oper_create(oper_data_t *oper_data, struct lyd_node **parent, const struct ly_ctx *ly_ctx)
{
	int num_ifs = oper_data->count;
	char xpath_buffer[PATH_MAX] = {0};

	for (int i = 0; i < num_ifs+1; i++) {
		/*// interface name
		snprintf(xpath_buffer, sizeof(xpath_buffer), "%s[name='%s']/if-name", "/ietf-dhcpv6-client:dhcpv6-client/client-if", oper_data->if_data[i].if_name);

		lyd_new_path(*parent, ly_ctx, xpath_buffer, oper_data->if_data[i].if_name, LYD_ANYDATA_STRING, 0);

		// interface enabled
		snprintf(xpath_buffer, sizeof(xpath_buffer), "%s[name='%s']/enabled", "/ietf-dhcpv6-client:dhcpv6-client/client-if", );

		lyd_new_path(*parent, ly_ctx, xpath_buffer, oper_data->if_data[i].if_name, LYD_ANYDATA_STRING, 0);*/

		int na_count = oper_data->if_data[i].na_count;

		for (int j = 0; j < na_count+1; j++) {

			// ia-id
			snprintf(xpath_buffer, sizeof(xpath_buffer), "%s[name='%s']/ia-na[ia-id='%s']/ia-id", "/ietf-dhcpv6-client:dhcpv6-client/client-if", oper_data->if_data[i].if_name,  oper_data->if_data[i].ia_na[j].ia_id);

			lyd_new_path(*parent, ly_ctx, xpath_buffer, oper_data->if_data[i].ia_na[j].ia_id, LYD_ANYDATA_STRING, 0);

			// ia-na-address
			snprintf(xpath_buffer, sizeof(xpath_buffer), "%s[name='%s']/ia-na[ia-id='%s']/ia-na-address", "/ietf-dhcpv6-client:dhcpv6-client/client-if", oper_data->if_data[i].if_name,  oper_data->if_data[i].ia_na[j].ia_id);

			lyd_new_path(*parent, ly_ctx, xpath_buffer, oper_data->if_data[i].ia_na[j].ia_na_address, LYD_ANYDATA_STRING, 0);

			// preferred-lifetime
			snprintf(xpath_buffer, sizeof(xpath_buffer), "%s[name='%s']/ia-na[ia-id='%s']/preferred-lifetime", "/ietf-dhcpv6-client:dhcpv6-client/client-if", oper_data->if_data[i].if_name,  oper_data->if_data[i].ia_na[j].ia_id);

			lyd_new_path(*parent, ly_ctx, xpath_buffer, oper_data->if_data[i].ia_na[j].preferred_lifetime, LYD_ANYDATA_STRING, 0);

			// valid-lifetime
			snprintf(xpath_buffer, sizeof(xpath_buffer), "%s[name='%s']/ia-na[ia-id='%s']/valid-lifetime", "/ietf-dhcpv6-client:dhcpv6-client/client-if", oper_data->if_data[i].if_name,  oper_data->if_data[i].ia_na[j].ia_id);

			lyd_new_path(*parent, ly_ctx, xpath_buffer, oper_data->if_data[i].ia_na[j].valid_lifetime, LYD_ANYDATA_STRING, 0);

			// allocation-time
			snprintf(xpath_buffer, sizeof(xpath_buffer), "%s[name='%s']/ia-na[ia-id='%s']/allocation-time", "/ietf-dhcpv6-client:dhcpv6-client/client-if", oper_data->if_data[i].if_name,  oper_data->if_data[i].ia_na[j].ia_id);

			lyd_new_path(*parent, ly_ctx, xpath_buffer, oper_data->if_data[i].ia_na[j].allocation_time, LYD_ANYDATA_STRING, 0);

			// last-renew-rebind
			snprintf(xpath_buffer, sizeof(xpath_buffer), "%s[name='%s']/ia-na[ia-id='%s']/last-renew-rebind", "/ietf-dhcpv6-client:dhcpv6-client/client-if", oper_data->if_data[i].if_name,  oper_data->if_data[i].ia_na[j].ia_id);

			lyd_new_path(*parent, ly_ctx, xpath_buffer, oper_data->if_data[i].ia_na[j].last_renew_rebind, LYD_ANYDATA_STRING, 0);

			// server-duid
			snprintf(xpath_buffer, sizeof(xpath_buffer), "%s[name='%s']/ia-na[ia-id='%s']/server-duid", "/ietf-dhcpv6-client:dhcpv6-client/client-if", oper_data->if_data[i].if_name,  oper_data->if_data[i].ia_na[j].ia_id);

			lyd_new_path(*parent, ly_ctx, xpath_buffer, oper_data->if_data[i].ia_na[j].server_duid, LYD_ANYDATA_STRING, 0);
		}

		int pd_count = oper_data->if_data[i].pd_count;

		for (int j = 0; j < pd_count+1; j++) {

			// ia-id
			snprintf(xpath_buffer, sizeof(xpath_buffer), "%s[name='%s']/ia-pd[ia-id='%s']/ia-id", "/ietf-dhcpv6-client:dhcpv6-client/client-if", oper_data->if_data[i].if_name,  oper_data->if_data[i].ia_pd[j].ia_id);

			lyd_new_path(*parent, ly_ctx, xpath_buffer, oper_data->if_data[i].ia_pd[j].ia_id, LYD_ANYDATA_STRING, 0);

			// ia-pd-prefix
			snprintf(xpath_buffer, sizeof(xpath_buffer), "%s[name='%s']/ia-na[ia-id='%s']/ia-pd-prefix", "/ietf-dhcpv6-client:dhcpv6-client/client-if", oper_data->if_data[i].if_name,  oper_data->if_data[i].ia_pd[j].ia_id);

			lyd_new_path(*parent, ly_ctx, xpath_buffer, oper_data->if_data[i].ia_pd[j].iaprefix, LYD_ANYDATA_STRING, 0);

			// preferred-lifetime
			snprintf(xpath_buffer, sizeof(xpath_buffer), "%s[name='%s']/ia-na[ia-id='%s']/preferred-lifetime", "/ietf-dhcpv6-client:dhcpv6-client/client-if", oper_data->if_data[i].if_name,  oper_data->if_data[i].ia_pd[j].ia_id);

			lyd_new_path(*parent, ly_ctx, xpath_buffer, oper_data->if_data[i].ia_pd[j].preferred_lifetime, LYD_ANYDATA_STRING, 0);

			// valid-lifetime
			snprintf(xpath_buffer, sizeof(xpath_buffer), "%s[name='%s']/ia-na[ia-id='%s']/valid-lifetime", "/ietf-dhcpv6-client:dhcpv6-client/client-if", oper_data->if_data[i].if_name,  oper_data->if_data[i].ia_pd[j].ia_id);

			lyd_new_path(*parent, ly_ctx, xpath_buffer, oper_data->if_data[i].ia_pd[j].valid_lifetime, LYD_ANYDATA_STRING, 0);

			// allocation-time
			snprintf(xpath_buffer, sizeof(xpath_buffer), "%s[name='%s']/ia-na[ia-id='%s']/allocation-time", "/ietf-dhcpv6-client:dhcpv6-client/client-if", oper_data->if_data[i].if_name,  oper_data->if_data[i].ia_pd[j].ia_id);

			lyd_new_path(*parent, ly_ctx, xpath_buffer, oper_data->if_data[i].ia_pd[j].allocation_time, LYD_ANYDATA_STRING, 0);

			// last-renew-rebind
			snprintf(xpath_buffer, sizeof(xpath_buffer), "%s[name='%s']/ia-na[ia-id='%s']/last-renew-rebind", "/ietf-dhcpv6-client:dhcpv6-client/client-if", oper_data->if_data[i].if_name,  oper_data->if_data[i].ia_pd[j].ia_id);

			lyd_new_path(*parent, ly_ctx, xpath_buffer, oper_data->if_data[i].ia_pd[j].last_renew_rebind, LYD_ANYDATA_STRING, 0);

			// server-duid
			snprintf(xpath_buffer, sizeof(xpath_buffer), "%s[name='%s']/ia-na[ia-id='%s']/server-duid", "/ietf-dhcpv6-client:dhcpv6-client/client-if", oper_data->if_data[i].if_name,  oper_data->if_data[i].ia_pd[j].ia_id);

			lyd_new_path(*parent, ly_ctx, xpath_buffer, oper_data->if_data[i].ia_pd[j].server_duid, LYD_ANYDATA_STRING, 0);
		}
	}

	return 0;
}

void dhcpv6_client_oper_cleanup(oper_data_t *oper_data)
{
	int j = 0;

	for (int i = 0; i < MAX_IFS; i++) {

		if (oper_data->if_data[i].if_name != NULL) {
			FREE_SAFE(oper_data->if_data[i].if_name);
		}

		// IA_NA
		for (j = 0; j < MAX_IA_NA; j++) {
			if (oper_data->if_data[i].ia_na[j].ia_id != NULL) {
				FREE_SAFE(oper_data->if_data[i].ia_na[j].ia_id);
			}

			if (oper_data->if_data[i].ia_na[j].ia_na_address != NULL) {
				FREE_SAFE(oper_data->if_data[i].ia_na[j].ia_na_address);
			}

			if (oper_data->if_data[i].ia_na[j].last_renew_rebind != NULL) {
				FREE_SAFE(oper_data->if_data[i].ia_na[j].last_renew_rebind);
			}

			if (oper_data->if_data[i].ia_na[j].preferred_lifetime != NULL) {
				FREE_SAFE(oper_data->if_data[i].ia_na[j].preferred_lifetime);
			}

			if (oper_data->if_data[i].ia_na[j].valid_lifetime != NULL) {
				FREE_SAFE(oper_data->if_data[i].ia_na[j].valid_lifetime);
			}
			if (oper_data->if_data[i].ia_na[j].allocation_time != NULL) {
				FREE_SAFE(oper_data->if_data[i].ia_na[j].allocation_time);
			}

			if (oper_data->if_data[i].ia_na[j].server_duid != NULL) {
				FREE_SAFE(oper_data->if_data[i].ia_na[j].server_duid);
			}
		}

		// IA_PD
		for (j = 0; j < MAX_IA_PD; j++) {
			if (oper_data->if_data[i].ia_pd[j].ia_id != NULL) {
				FREE_SAFE(oper_data->if_data[i].ia_pd[j].ia_id);
			}

			if (oper_data->if_data[i].ia_pd[j].iaprefix != NULL) {
				FREE_SAFE(oper_data->if_data[i].ia_pd[j].iaprefix);
			}

			if (oper_data->if_data[i].ia_pd[j].last_renew_rebind != NULL) {
				FREE_SAFE(oper_data->if_data[i].ia_pd[j].last_renew_rebind);
			}

			if (oper_data->if_data[i].ia_pd[j].preferred_lifetime != NULL) {
				FREE_SAFE(oper_data->if_data[i].ia_pd[j].preferred_lifetime);
			}

			if (oper_data->if_data[i].ia_pd[j].valid_lifetime != NULL) {
				FREE_SAFE(oper_data->if_data[i].ia_pd[j].valid_lifetime);
			}
			if (oper_data->if_data[i].ia_pd[j].allocation_time != NULL) {
				FREE_SAFE(oper_data->if_data[i].ia_pd[j].allocation_time);
			}

			if (oper_data->if_data[i].ia_pd[j].server_duid != NULL) {
				FREE_SAFE(oper_data->if_data[i].ia_pd[j].server_duid);
			}
		}

		oper_data->if_data[i].na_count = 0;
		oper_data->if_data[i].pd_count = 0;
	}

	oper_data->count = 0;
}