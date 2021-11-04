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

#include "dhcpv6_client_list.h"


int dhcpv6_client_list_add_entries(config_data_list_t *ccl, char *if_name, char *opt, char **opt_vals)
{
	int error = 0;

	if (opt == NULL) {
		// just set the interface name to the list
		error = dhcpv6_client_list_add_interface(ccl, if_name);
		if (error != 0) {
			goto error_out;
		}
	} else if (strcmp(opt, "send dhcp6.client-id") == 0) {
		// set duid for this if_name
		error = dhcpv6_client_list_add_duid(ccl, if_name, opt_vals[0]); // the duid value will always be at 0 index
		if (error != 0) {
			goto error_out;
		}
	} else if (strcmp(opt, "request dhcp6.oro") == 0) {
		/* oro can not been sent by the client only requested (dhclient problem)
		// set oro options for this if_name
		for (int i = 0; i < MAX_ORO_OPTS; i++) {
			if (opt_vals[i] != NULL) {
				error = dhcpv6_client_list_add_oro(ccl, if_name, opt_vals[i]);
				if (error != 0) {
					goto error_out;
				}
			}
		} */
		error = dhcpv6_client_list_add_oro(ccl, if_name, opt_vals[0]);
		if (error != 0) {
			goto error_out;
		}
	}  else if (strcmp(opt, "request dhcp6.rapid-commit") == 0) {
		// set rapid commit for this if_name
				error = dhcpv6_client_list_add_rpd_cmt(ccl, if_name);
		if (error != 0) {
			goto error_out;
		}
	} else if (strcmp(opt, "send user-class") == 0) {
		// set user class for this if_name
		error = dhcpv6_client_list_add_usr_cls(ccl, if_name, opt_vals[0]); // the user class value will always be at 0 index
		if (error != 0) {
			goto error_out;
		}
	} else if (strcmp(opt, "send dhcp6.reconf-accept") == 0) {
		// set reconf accept option for this if_name
		error = dhcpv6_client_list_add_rcn_acpt(ccl, if_name);
		if (error != 0) {
			goto error_out;
		}
	} else if (strcmp(opt, "send dhcp6.vendor-class") == 0) {
		/* vendor class is not currently implemented in dhclient
		// set vendor class for this if_name
		// vendor class opt_vals will always be in following format:
		//	enterprise number at index 0, vendor class id at index 1 and
		//  vendor class data at index 2
		error = dhcpv6_client_list_add_vnd_cls_opt(ccl, if_name, opt_vals[0], opt_vals[1], opt_vals[2]);
		if (error != 0) {
			goto error_out;
		}
		*/
	} else if (strcmp(opt, "request dhcp6.vendor-opts") == 0) {
		// set vendor specific options for this if_name
		error = dhcpv6_client_list_add_vnd_spec_opt(ccl, if_name);
		if (error != 0) {
			goto error_out;
		}
	}

	return 0;
error_out:
	return -1;
}

int dhcpv6_client_list_add_interface(config_data_list_t *ccl, char *if_name)
{
	bool name_found = false;

	if (ccl->count >= MAX_IF_NUM) {
		goto error_out;
	}

	// iterate through all client_configs
	for (uint32_t i = 0; i < ccl->count; i++) {
		dhcpv6_client_config_t client_config = ccl->configs[i];

		if (client_config.if_name != NULL) { // in case we deleted a client_config entry, name will be NULL
			if (strcmp(client_config.if_name, if_name) == 0) {
				name_found = true;
				break;
			}
		}
	}

	if (!name_found) {
		// set the new client_config entry (if_name) to the first free one in the list
		// the one with if_name == NULL
		uint32_t pos = ccl->count;
		for (uint32_t i = 0; i < ccl->count; i++) {
			dhcpv6_client_config_t client_config = ccl->configs[i];

			if (client_config.if_name == NULL) {
				pos = i;
				break;
			}
		}

		dhcpv6_client_list_set_if_name(&ccl->configs[pos], if_name);
		if (pos == ccl->count) {
			// increase the number of client_configs in client_config_list
			// only if we didn't find an empty "spot" between two already existing members in the list
			++ccl->count;
		}
	}

	return 0;
error_out:
	return -1;
}

void dhcpv6_client_list_set_if_name(dhcpv6_client_config_t *client_config, char *if_name)
{
	client_config->if_name = xstrdup(if_name);
}

int dhcpv6_client_list_add_enabled(config_data_list_t *ccl, char *if_name, char *enabled)
{
	dhcpv6_client_config_t *client_config = NULL;

	client_config = dhcpv6_client_list_get_client_config(ccl, if_name);

	if (client_config == NULL) {
		goto error_out;
	}

	client_config->enabled = xstrdup(enabled);

	return 0;

error_out:
	return -1;
}

int dhcpv6_client_list_add_global_duid(config_data_list_t *ccl, char *duid)
{
	// convert duid to hex representation
	// e.g.: 000200090CC084D303000913 -> 00:02:00:09:0C:C0:84:D3:03:00:09:13
	int len = (int)strlen(duid);
	char new_duid[128] = {0}; // 128 max duid len TODO: check this
	int cnt = 0;

	for (int i = 0; i < len; i++) {
		if (i > 0 && i % 2 == 0) {
			new_duid[cnt] = ':';
			new_duid[++cnt] = duid[i];
		} else {
			new_duid[cnt] = duid[i];
		}
		cnt++;
	}

	// set same DUID to all interfaces
	for (int i = 0; i < MAX_IF_NUM; i++) {
		if (ccl->configs[i].duid != NULL) {
			FREE_SAFE(ccl->configs[i].duid);
		}

		ccl->configs[i].duid = xstrdup(new_duid);
	}

	return 0;
}

int dhcpv6_client_list_add_duid(config_data_list_t *ccl, char *if_name, char *duid)
{
	dhcpv6_client_config_t *client_config = NULL;

	client_config = dhcpv6_client_list_get_client_config(ccl, if_name);

	if (client_config == NULL) {
		goto error_out;
	}

	if (client_config->duid != NULL) {
		FREE_SAFE(client_config->duid);
	}

	// convert duid to hex representation
	// e.g.: 000200090CC084D303000913 -> 00:02:00:09:0C:C0:84:D3:03:00:09:13
	int len = (int)strlen(duid);
	char new_duid[128] = {0}; // 128 max duid len TODO: check this
	int cnt = 0;

	for (int i = 0; i < len; i++) {
		if (i > 0 && i % 2 == 0) {
			new_duid[cnt] = ':';
			new_duid[++cnt] = duid[i];
		} else {
			new_duid[cnt] = duid[i];
		}
		cnt++;
	}

	client_config->duid = xstrdup(new_duid);

	return 0;
error_out:
	return -1;
}

int dhcpv6_client_list_add_oro(config_data_list_t *ccl, char *if_name, char *oro_opt)
{
	dhcpv6_client_config_t *client_config = NULL;
	//bool oro_opt_found = false;

	client_config = dhcpv6_client_list_get_client_config(ccl, if_name);

	if (client_config == NULL) {
		goto error_out;
	}

	/* oro can not be sent by the client (dhclient problem)
	// iterate through all oro options for current interface
	// check if oro option with oro_opt value already exists in list
	for (uint32_t i = 0; i < MAX_ORO_OPTS; i++) {
		if (client_config->config_opts.oro.oro_options[i] != NULL) { // in case we deleted a oro option, it will be NULL
			if (strcmp(client_config->config_opts.oro.oro_options[i], oro_opt) == 0) {
				oro_opt_found = true;
				break;
			}
		}
	}

	if (!oro_opt_found) {
		// set the new oro option for this interface to the first free one in the list
		uint32_t pos = client_config->config_opts.oro.count;

		for (uint32_t i = 0; i < client_config->config_opts.oro.count; i++) {
			// first one where oro option is == NULL
			if (client_config->config_opts.oro.oro_options[i] == NULL) {
				pos = i;
				break;
			}
		}

		client_config->config_opts.oro.oro_options[pos] = xstrdup(oro_opt);
		if (pos == client_config->config_opts.oro.count) {
			// increase the number of oro options
			// only if we didn't find an empty "spot" between two already existing options in the array
			++client_config->config_opts.oro.count;
		}
	}*/

	// just increase the counter
	client_config->config_opts.oro.count++;

	return 0;

error_out:
	return -1;
}

int dhcpv6_client_list_add_rpd_cmt(config_data_list_t *ccl, char *if_name)
{
	dhcpv6_client_config_t *client_config = NULL;

	client_config = dhcpv6_client_list_get_client_config(ccl, if_name);

	if (client_config == NULL) {
		goto error_out;
	}

	client_config->config_opts.rapid_commit_opt = true;

	return 0;

error_out:
	return -1;
}

int dhcpv6_client_list_add_usr_cls(config_data_list_t *ccl, char *if_name, char *usr_cls)
{
	dhcpv6_client_config_t *client_config = NULL;
	bool usr_cls_found = false;

	client_config = dhcpv6_client_list_get_client_config(ccl, if_name);

	if (client_config == NULL) {
		goto error_out;
	}

	// iterate through all user class data for current interface
	for (uint32_t i = 0; i < MAX_USR_CLASS_INSTANCES; i++) {
		user_class_data_instance_t usr_cls_inst = client_config->config_opts.user_class_opt.user_class_data_instances[i];

		if (usr_cls_inst.user_class_data != NULL) { // in case we deleted a user class data, it will be NULL
			if (strcmp(usr_cls_inst.user_class_data, usr_cls) == 0) {
				usr_cls_found = true;
				break;
			}
		}
	}

	if (!usr_cls_found) {
		// set the new  user class data for this interface to the first free one in the list
		uint32_t pos = client_config->config_opts.user_class_opt.count;

		for (uint32_t i = 0; i < client_config->config_opts.user_class_opt.count; i++) {
			// first one where user class data is == NULL
			if (client_config->config_opts.user_class_opt.user_class_data_instances[i].user_class_data == NULL) {
				pos = i;
				break;
			}
		}

		client_config->config_opts.user_class_opt.user_class_data_instances[pos].user_class_data = xstrdup(usr_cls);
		if (pos == client_config->config_opts.user_class_opt.count) {
			// increase the number of user class data
			// only if we didn't find an empty "spot" between two already existing data in the list
			++client_config->config_opts.user_class_opt.count;
		}
	}

	return 0;

error_out:
	return -1;
}

int dhcpv6_client_list_add_rcn_acpt(config_data_list_t *ccl, char *if_name)
{
	dhcpv6_client_config_t *client_config = NULL;

	client_config = dhcpv6_client_list_get_client_config(ccl, if_name);

	if (client_config == NULL) {
		goto error_out;
	}

	client_config->config_opts.reconfigure_accept_opt = true;

	return 0;

error_out:
	return -1;
}
/* vendor class is currently not implemented in dhclient
int dhcpv6_client_list_add_vnd_cls_opt(config_data_list_t *ccl, char *if_name, char *ent_num, char *vnd_cls_id, char *vnd_cls_data)
{
	dhcpv6_client_config_t *client_config = NULL;
	uint32_t enterprise_number = 0;
	uint8_t vendor_class_data_id = 0;
	uint32_t i = 0;
	uint32_t inst_pos = 0;
	bool vnd_cls_inst_found = false;

	// TODO: create functions: dhcpv6_client_list_get_vendor_class_instance by ent num
	// and dhcpv6_client_list_get_vendor_class by vnd cls id
	client_config = dhcpv6_client_list_get_client_config(ccl, if_name);

	if (client_config == NULL) {
		goto error_out;
	}

	// cast strings to appropriate integers
	enterprise_number = (uint32_t) atoi(ent_num);
	vendor_class_data_id = (uint8_t) atoi(vnd_cls_id);

	// iterate through all vendor class instances for current interface
	for (i = 0; i < MAX_VENDOR_CLASS_INSTANCES; i++) {
		vendor_class_option_instance_t vnd_cls_inst = client_config->config_opts.vendor_class_opt.vendor_class_option_instances[i];

		if (vnd_cls_inst.enterprise_number != 0) { // in case we deleted a vendor class instance, it's enterprise number will be 0
			if (vnd_cls_inst.enterprise_number == enterprise_number) {
				vnd_cls_inst_found = true;
				break;
			}
		}
	}

	if (!vnd_cls_inst_found) {
		// set the new  vendor class instance for this interface to the first free one in the list
		inst_pos = client_config->config_opts.vendor_class_opt.count;

		for (i = 0; i < client_config->config_opts.vendor_class_opt.count; i++) {
			// first one where vendor class instance enterprise number is == 0
			if (client_config->config_opts.vendor_class_opt.vendor_class_option_instances[i].enterprise_number == 0) {
				inst_pos = i;
				break;
			}
		}

		client_config->config_opts.vendor_class_opt.vendor_class_option_instances[inst_pos].enterprise_number = enterprise_number;

		if (inst_pos == client_config->config_opts.vendor_class_opt.count) {
			// increase the number of vendor class instances
			// only if we didn't find an empty "spot" between two already existing data in the list
			++client_config->config_opts.vendor_class_opt.count;
		}
	}

	dhcpv6_client_list_add_vnd_cls(client_config, i, vendor_class_data_id, vnd_cls_data);

	return 0;

error_out:
	return -1;
}

void dhcpv6_client_list_add_vnd_cls(dhcpv6_client_config_t *client_config, uint32_t inst_pos, uint8_t vendor_class_data_id, char *vnd_cls_data)
{
	bool vnd_cls_found = false;
	uint32_t cls_pos = 0;

	// iterate through all vendor classes in a vendor class instance for current interface
	for (uint32_t i = 0; i < MAX_VENDOR_CLASSES; i++) {
		vendor_class_t vnd_cls = client_config->config_opts.vendor_class_opt.vendor_class_option_instances[inst_pos].vendor_class[i];

		if (vnd_cls.vendor_class_data_id != 0) { // in case we deleted a vendor class, it's data id will be 0
			if (vnd_cls.vendor_class_data_id == vendor_class_data_id) {
				vnd_cls_found = true;
				break;
			}
		}
	}

	if (!vnd_cls_found) {
		// set the new vendor class for this interface to the first free one in the list
		cls_pos = client_config->config_opts.vendor_class_opt.vendor_class_option_instances[inst_pos].count;

		for (uint32_t i = 0; i < client_config->config_opts.vendor_class_opt.vendor_class_option_instances[inst_pos].count; i++) {
			// first one where vendor class data id is == 0
			if (client_config->config_opts.vendor_class_opt.vendor_class_option_instances[inst_pos].vendor_class[i].vendor_class_data_id == 0) {
				cls_pos = i;
				break;
			}
		}

		client_config->config_opts.vendor_class_opt.vendor_class_option_instances[inst_pos].vendor_class[cls_pos].vendor_class_data_id = vendor_class_data_id;
		client_config->config_opts.vendor_class_opt.vendor_class_option_instances[inst_pos].vendor_class[cls_pos].vendor_class_data = xstrdup(vnd_cls_data);

		if (cls_pos == client_config->config_opts.vendor_class_opt.vendor_class_option_instances[inst_pos].count) {
			// increase the number of vendor classes
			// only if we didn't find an empty "spot" between two already existing data in the list
			++client_config->config_opts.vendor_class_opt.vendor_class_option_instances[inst_pos].count;
		}
	}
}
*/
int dhcpv6_client_list_add_vnd_spec_opt(config_data_list_t *ccl, char *if_name)
{
	dhcpv6_client_config_t *client_config = NULL;

	client_config = dhcpv6_client_list_get_client_config(ccl, if_name);

	if (client_config == NULL) {
		goto error_out;
	}

	// for now only send dhcp6.reconf-accept; is set once per interface
	// so just increase count for this specific interface and leave everything else with val 0

	client_config->config_opts.vendor_specific_info_opts.count++;

	return 0;

error_out:
	return -1;
}

dhcpv6_client_config_t *dhcpv6_client_list_get_client_config(config_data_list_t *ccl, char *if_name)
{
	dhcpv6_client_config_t *client_config = NULL;

	for (uint32_t i = 0; i < ccl->count; i++) {
		if (ccl->configs[i].if_name != NULL) {
			if (strcmp(ccl->configs[i].if_name, if_name) == 0) {
				client_config = &ccl->configs[i];
				break;
			}
		}
	}

	return client_config;
}

void dhcpv6_client_list_free(config_data_list_t *ccl)
{
	for (uint32_t i = 0; i < ccl->count; i++) {
		dhcpv6_client_list_free_config(&ccl->configs[i]);
	}

	// if global client-duid is used, all interfaces will have it set
	for (uint32_t i = 0; i <  MAX_IF_NUM; i++) {
		if (ccl->configs[i].duid != NULL) {
			FREE_SAFE(ccl->configs[i].duid );
		}
	}
}

void dhcpv6_client_list_free_config(dhcpv6_client_config_t *client_config)
{
	if (client_config->if_name != NULL) {
		FREE_SAFE(client_config->if_name);
	}

	if (client_config->enabled != NULL) {
		FREE_SAFE(client_config->enabled);
	}

	if (client_config->duid != NULL) {
		FREE_SAFE(client_config->duid);
	}

	// free config options

	// free ORO
	/*
	for (uint32_t i = 0; i < client_config->config_opts.oro.count; i++) {
		FREE_SAFE(client_config->config_opts.oro.oro_options[i]);
	}*/
	client_config->config_opts.oro.count = 0;

	// free user_class_opt
	for (uint32_t i = 0; i < client_config->config_opts.user_class_opt.count; i++) {
		FREE_SAFE(client_config->config_opts.user_class_opt.user_class_data_instances[i].user_class_data);
	}
	client_config->config_opts.user_class_opt.count = 0;

	/* vendor class is not currently implemented in dhclient
	// free vendor_class_opt
	for (uint32_t i = 0; i < client_config->config_opts.vendor_class_opt.count; i++) {
		client_config->config_opts.vendor_class_opt.vendor_class_option_instances[i].enterprise_number = 0;

		for (uint32_t j = 0; j < client_config->config_opts.vendor_class_opt.vendor_class_option_instances[i].count; j++) {
			FREE_SAFE(client_config->config_opts.vendor_class_opt.vendor_class_option_instances[i].vendor_class[j].vendor_class_data);
			client_config->config_opts.vendor_class_opt.vendor_class_option_instances[i].vendor_class[j].vendor_class_data_id = 0;
		}
		client_config->config_opts.vendor_class_opt.vendor_class_option_instances[i].count = 0;
	}
	client_config->config_opts.vendor_class_opt.count = 0;
	*/

	// free vendor_specific_info_opts
	/*
	for (uint32_t i = 0; i < client_config->config_opts.vendor_specific_info_opts.count; i++) {
		client_config->config_opts.vendor_specific_info_opts.vendor_specific_info_opt[i].enterprise_number = 0;

		for (uint32_t j = 0; j < client_config->config_opts.vendor_specific_info_opts.vendor_specific_info_opt[i].count; j++) {
			FREE_SAFE(client_config->config_opts.vendor_specific_info_opts.vendor_specific_info_opt[i].vendor_option_data[j].sub_opt_data);
			client_config->config_opts.vendor_specific_info_opts.vendor_specific_info_opt[i].vendor_option_data[j].sub_opt_code = 0;
		}
		client_config->config_opts.vendor_specific_info_opts.vendor_specific_info_opt[i].count = 0;
	}
	*/
	client_config->config_opts.vendor_specific_info_opts.count = 0;

	// ia-na and ia-pd not implemented yet
}