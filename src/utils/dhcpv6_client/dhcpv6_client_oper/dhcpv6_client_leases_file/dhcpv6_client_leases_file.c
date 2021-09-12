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

#include "dhcpv6_client_leases_file.h"

int dhcpv6_client_leases_file_parse(oper_data_t *oper_data)
{
	int error = 0;
	FILE *fp = NULL;
	char *line = NULL;
	char *line_cpy = NULL;
	size_t len = 0;
	ssize_t read = 0;
	char if_name[IFNAMSIZ] = {0}; // IFNAMSIZ max length of interface name
	char ia_id[IA_ID_LEN] = {0};
	unsigned int tmp_ia_id = 0;
	char renew[RENEW_LEN] = {0};
	char iaaddr[INET6_ADDRSTRLEN] = {0};
	char starts[STARTS_LEN] = {0};
	char preferred_life[PREF_LIFE_LEN] = {0};
	char max_life[MAX_LIFE_LEN] = {0};
	char iaprefix[IA_PREFIX_LEN] = {0};
	char server_id[MAX_SERVER_ID] = {0};
	bool ia_pd = false;

	// check if file exists
	if (access(DHCLIENT6_LEASE_FILE, F_OK) != 0) {
		// if it doesn't, just return from this function like everything is normal
		SRP_LOG_INF("dhclient.conf file doesn't exist, skipping parsing at init");
		return 0;
	}

	fp = fopen(DHCLIENT6_LEASE_FILE, "r");

	if (fp == NULL) {
		SRP_LOG_ERR("fopen failed");
		goto error_out;
	}

	// set the oper_data counter to -1 because we will increment it for each new interface
	oper_data->count = -1;

	// iterate line by line through dhclient6.leases
	while ((read = getline(&line, &len, fp)) != -1) {
		line_cpy = xstrdup(line);
		line_cpy = strstrip(line_cpy);

		// check if interface keyword in line
		if (strstr(line_cpy, "interface") != NULL) {
			// find "interface" keyword and get name of interface
			error = sscanf(line_cpy, "interface \"%16[^\"]\"", if_name); // IFNAMSIZ = 16
			if (error == EOF) {
				SRP_LOG_ERR("sscanf error");
				goto error_out;
			}

			// increase the index
			oper_data->if_data[++oper_data->count].if_name = xstrdup(if_name);

			// set ia_na and ia_pd counter to -1
			--oper_data->if_data[oper_data->count].na_count;
			--oper_data->if_data[oper_data->count].pd_count;

		} else if (strstr(line_cpy, "ia-na") != NULL) {
			error = sscanf(line_cpy, "ia-na %x", &tmp_ia_id);
			if (error == EOF) {
				SRP_LOG_ERR("sscanf error");
				goto error_out;
			}

			snprintf( ia_id, IA_ID_LEN, "%d", tmp_ia_id);

			oper_data->if_data[oper_data->count].ia_na->ia_id = xstrdup(ia_id);

			// increase the counter
			++oper_data->if_data[oper_data->count].na_count;

			// set the flag to false
			ia_pd = false;

		} else if (strstr(line_cpy, "ia-pd") != NULL) {
			error = sscanf(line_cpy, "ia-pd %x", &tmp_ia_id);
			if (error == EOF) {
				SRP_LOG_ERR("sscanf error");
				goto error_out;
			}

			snprintf( ia_id, IA_ID_LEN, "%d", tmp_ia_id);

			oper_data->if_data[oper_data->count].ia_pd->ia_id = xstrdup(ia_id);

			// increase the counter
			++oper_data->if_data[oper_data->count].pd_count;

			// set the flag to true
			ia_pd = true;

		} else if (strstr(line_cpy, "renew") != NULL) {
			error = sscanf(line_cpy, "renew %s", renew);
			if (error == EOF) {
				SRP_LOG_ERR("sscanf error");
				goto error_out;
			}

			if (ia_pd) {
				oper_data->if_data[oper_data->count].ia_pd[oper_data->if_data[oper_data->count].pd_count].last_renew_rebind = xstrdup(renew);
			} else {
				oper_data->if_data[oper_data->count].ia_na[oper_data->if_data[oper_data->count].na_count].last_renew_rebind = xstrdup(renew);
			}

		} else if (strstr(line_cpy, "iaaddr") != NULL) {
			error = sscanf(line_cpy, "iaaddr %s", iaaddr);
			if (error == EOF) {
				SRP_LOG_ERR("sscanf error");
				goto error_out;
			}

			oper_data->if_data[oper_data->count].ia_na[oper_data->if_data[oper_data->count].na_count].ia_na_address = xstrdup(iaaddr);

		} else if (strstr(line_cpy, "starts") != NULL) {
			error = sscanf(line_cpy, "starts %s", starts);
			if (error == EOF) {
				SRP_LOG_ERR("sscanf error");
				goto error_out;
			}

			if (ia_pd) {
				oper_data->if_data[oper_data->count].ia_pd[oper_data->if_data[oper_data->count].pd_count].allocation_time = xstrdup(starts);
			} else {
				oper_data->if_data[oper_data->count].ia_na[oper_data->if_data[oper_data->count].na_count].allocation_time = xstrdup(starts);
			}

		} else if (strstr(line_cpy, "preferred-life") != NULL) {
			error = sscanf(line_cpy, "preferred-life %s", preferred_life);
			if (error == EOF) {
				SRP_LOG_ERR("sscanf error");
				goto error_out;
			}

			if (ia_pd) {
				oper_data->if_data[oper_data->count].ia_pd[oper_data->if_data[oper_data->count].pd_count].preferred_lifetime = xstrdup(preferred_life);
			} else {
				oper_data->if_data[oper_data->count].ia_na[oper_data->if_data[oper_data->count].na_count].preferred_lifetime = xstrdup(preferred_life);
			}

		} else if (strstr(line_cpy, "max-life") != NULL) {
			error = sscanf(line_cpy, "max-life %s", max_life);
			if (error == EOF) {
				SRP_LOG_ERR("sscanf error");
				goto error_out;
			}

			if (ia_pd) {
				oper_data->if_data[oper_data->count].ia_pd[oper_data->if_data[oper_data->count].pd_count].valid_lifetime = xstrdup(max_life);
			} else {
				oper_data->if_data[oper_data->count].ia_na[oper_data->if_data[oper_data->count].na_count].valid_lifetime = xstrdup(max_life);
			}

		} else if (strstr(line_cpy, "iaprefix") != NULL) {
			error = sscanf(line_cpy, "iaprefix %s", iaprefix);
			if (error == EOF) {
				SRP_LOG_ERR("sscanf error");
				goto error_out;
			}

			oper_data->if_data[oper_data->count].ia_pd->iaprefix = xstrdup(iaprefix);

		} else if (strstr(line_cpy, "dhcp6.server-id") != NULL) {
			// TODO: add server id to every ia_na/ia_pd entry not just the last one
			error = sscanf(line_cpy, "option dhcp6.server-id %s", server_id);
			if (error == EOF) {
				SRP_LOG_ERR("sscanf error");
				goto error_out;
			}

			// remove the colons from server_id string
			int j, n = (int)strlen(server_id);

			for (int i = j = 0; i < n; i++) {
				if (server_id[i] != ':') {
					server_id[j++] =  server_id[i];
				}
			}

			server_id[j] = '\0';

			if (ia_pd) {
				oper_data->if_data[oper_data->count].ia_pd[oper_data->if_data[oper_data->count].pd_count].server_duid = xstrdup(server_id);
			} else {
				oper_data->if_data[oper_data->count].ia_na[oper_data->if_data[oper_data->count].na_count].server_duid = xstrdup(server_id);
			}
		}
	}

	fclose(fp);

	if (line != NULL) {
		FREE_SAFE(line);
	}

	if (line_cpy != NULL) {
		FREE_SAFE(line_cpy);
	}

	return 0;

error_out:
	if (fp != NULL) {
		fclose(fp);
	}

	if (line != NULL) {
		FREE_SAFE(line);
	}

	if (line_cpy != NULL) {
		FREE_SAFE(line_cpy);
	}

	return -1;
}
