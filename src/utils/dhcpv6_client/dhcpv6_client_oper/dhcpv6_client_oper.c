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

	//lyd_new_path(*parent, ly_ctx, "/ietf-dhcpv6-client:dhcpv6-client/client-if[if-name='enp0s3']/ia-na[ia-id='27:17:c7:f9']/ia-id", oper_data->if_data[0].ia_na[0].ia_id, LYD_ANYDATA_STRING, 0);

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