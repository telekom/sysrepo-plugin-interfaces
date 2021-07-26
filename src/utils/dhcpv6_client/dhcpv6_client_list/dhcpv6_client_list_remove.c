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

int dhcpv6_client_list_set_del_if(config_data_list_t *ccl, char *if_name)
{
	dhcpv6_client_config_t *client_config = NULL;

	client_config = dhcpv6_client_list_get_client_config(ccl, if_name);

	if (client_config == NULL) {
		goto error_out;
	}

	client_config->del = true;

	return 0;
error_out:
	return -1;
}

int dhcpv6_client_list_check_remove_interface(config_data_list_t *ccl)
{
	int error = 0;

	// iterate through all interfaces in list
	for (uint32_t i = 0; i < ccl->count; i++) {
		if (ccl->configs[i].del == true) {
			error = dhcpv6_client_list_remove_interface(ccl, ccl->configs[i].if_name);
			if (error != 0) {
				goto error_out;
			}
		}
	}

	return 0;
error_out:
	return -1;
}

int dhcpv6_client_list_remove_interface(config_data_list_t *ccl, char *if_name)
{

	dhcpv6_client_config_t *client_config = NULL;

	client_config = dhcpv6_client_list_get_client_config(ccl, if_name);

	if (client_config == NULL) {
		goto error_out;
	}

	// if_name should always be != NULL when this function is called, but check it anyway
	if (client_config->if_name != NULL) {
		FREE_SAFE(client_config->if_name);
	}

	ccl->count--;
	client_config->del = false;

	return 0;
error_out:
	return -1;
}

int dhcpv6_client_list_remove_duid(config_data_list_t *ccl, char *if_name, char *duid)
{
	dhcpv6_client_config_t *client_config = NULL;

	client_config = dhcpv6_client_list_get_client_config(ccl, if_name);

	if (client_config == NULL) {
		goto error_out;
	}

	//  // duid should always be != NULL when this function is called, but check it anyway
	if (client_config->duid != NULL) {
		FREE_SAFE(client_config->duid);
	}

	return 0;
error_out:
	return -1;
}

int dhcpv6_client_list_remove_oro(config_data_list_t *ccl, char *if_name, char *oro_opt)
{
	dhcpv6_client_config_t *client_config = NULL;

	client_config = dhcpv6_client_list_get_client_config(ccl, if_name);

	if (client_config == NULL) {
		goto error_out;
	}

	/* oro can not be sent by the client (dhclient implementation problem)
	// iterate through all oro options for current interface
	for (uint32_t i = 0; i < MAX_ORO_OPTS; i++) {
		if (client_config->config_opts.oro.oro_options[i] != NULL) {
			if (strcmp(client_config->config_opts.oro.oro_options[i], oro_opt) == 0) {
				FREE_SAFE(client_config->config_opts.oro.oro_options[i]);
				--client_config->config_opts.oro.count;
				break;
			}
		}
	}
	*/

	// just decrease the count
	client_config->config_opts.oro.count--;

	return 0;

error_out:
	return -1;
}

int dhcpv6_client_list_remove_rpd_cmt(config_data_list_t *ccl, char *if_name)
{
	dhcpv6_client_config_t *client_config = NULL;

	client_config = dhcpv6_client_list_get_client_config(ccl, if_name);

	if (client_config == NULL) {
		goto error_out;
	}

	client_config->config_opts.rapid_commit_opt = false;

	return 0;

error_out:
	return -1;
}

int dhcpv6_client_list_remove_usr_cls(config_data_list_t *ccl, char *if_name, char *usr_cls)
{
	dhcpv6_client_config_t *client_config = NULL;

	client_config = dhcpv6_client_list_get_client_config(ccl, if_name);

	if (client_config == NULL) {
		goto error_out;
	}

	// iterate through all user class data for current interface
	for (uint32_t i = 0; i < MAX_USR_CLASS_INSTANCES; i++) {
		user_class_data_instance_t usr_cls_inst = client_config->config_opts.user_class_opt.user_class_data_instances[i];

		if (usr_cls_inst.user_class_data != NULL) {
			if (strcmp(usr_cls_inst.user_class_data, usr_cls) == 0) {
				FREE_SAFE(client_config->config_opts.user_class_opt.user_class_data_instances[i].user_class_data);
				--client_config->config_opts.user_class_opt.count;
				break;
			}
		}
	}

	return 0;

error_out:
	return -1;
}

int dhcpv6_client_list_remove_rcn_acpt(config_data_list_t *ccl, char *if_name)
{
	dhcpv6_client_config_t *client_config = NULL;

	client_config = dhcpv6_client_list_get_client_config(ccl, if_name);

	if (client_config == NULL) {
		goto error_out;
	}

	client_config->config_opts.reconfigure_accept_opt = false;

	return 0;

error_out:
	return -1;
}

/* vendor class is currently not implemented in dhclient

int dhcpv6_client_list_remove_vnd_cls_opt(config_data_list_t *ccl, char *if_name, char *ent_num)
{
	dhcpv6_client_config_t *client_config = NULL;
	uint32_t enterprise_number = 0;

	// TODO: create functions: dhcpv6_client_list_get_vendor_class_instance by ent num
	// and dhcpv6_client_list_get_vendor_class by vnd cls id
	client_config = dhcpv6_client_list_get_client_config(ccl, if_name);

	if (client_config == NULL) {
		goto error_out;
	}

	// cast strings to appropriate integers
	enterprise_number = (uint32_t) atoi(ent_num);

	// iterate through all vendor class instances for current interface
	for (uint32_t i = 0; i < MAX_VENDOR_CLASS_INSTANCES; i++) {
		vendor_class_option_instance_t vnd_cls_inst = client_config->config_opts.vendor_class_opt.vendor_class_option_instances[i];

		if (vnd_cls_inst.enterprise_number != 0) { // in case we deleted a vendor class instance, it's enterprise number will be 0
			if (vnd_cls_inst.enterprise_number == enterprise_number) {
				dhcpv6_client_list_remove_vnd_cls(client_config, i);

				client_config->config_opts.vendor_class_opt.vendor_class_option_instances[i].enterprise_number = 0;
				--client_config->config_opts.vendor_class_opt.count;

				break;
			}
		}
	}

	return 0;

error_out:
	return -1;
}

int dhcpv6_client_list_remove_vnd_cls_data(config_data_list_t *ccl, char *if_name, char *ent_num, char *vnd_cls_id, char *vnd_cls_data)
{
	dhcpv6_client_config_t *client_config = NULL;
	uint32_t enterprise_number = 0;
	uint8_t vendor_class_data_id = 0;

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
	for (uint32_t i = 0; i < MAX_VENDOR_CLASS_INSTANCES; i++) {
		vendor_class_option_instance_t vnd_cls_inst = client_config->config_opts.vendor_class_opt.vendor_class_option_instances[i];

		if (vnd_cls_inst.enterprise_number != 0) {
			if (vnd_cls_inst.enterprise_number == enterprise_number) {
				for (uint32_t j = 0; j < MAX_VENDOR_CLASSES; j++) {
					vendor_class_t vnd_cls = client_config->config_opts.vendor_class_opt.vendor_class_option_instances[i].vendor_class[j];

					if (vnd_cls.vendor_class_data_id != 0) {
						if (vnd_cls.vendor_class_data_id == vendor_class_data_id) {
							FREE_SAFE(client_config->config_opts.vendor_class_opt.vendor_class_option_instances[i].vendor_class[j].vendor_class_data);
							break;
						}
					}
				}
			}
		}
	}

	return 0;

error_out:
	return -1;
}

int dhcpv6_client_list_remove_vnd_cls_id(config_data_list_t *ccl, char *if_name, char *ent_num, char *vnd_cls_id, char *vnd_cls_data)
{
	dhcpv6_client_config_t *client_config = NULL;
	uint32_t enterprise_number = 0;
	uint8_t vendor_class_data_id = 0;

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
	for (uint32_t i = 0; i < MAX_VENDOR_CLASS_INSTANCES; i++) {
		vendor_class_option_instance_t vnd_cls_inst = client_config->config_opts.vendor_class_opt.vendor_class_option_instances[i];

		if (vnd_cls_inst.enterprise_number != 0) {
			if (vnd_cls_inst.enterprise_number == enterprise_number) {
				for (uint32_t j = 0; j < MAX_VENDOR_CLASSES; j++) {
					vendor_class_t vnd_cls = client_config->config_opts.vendor_class_opt.vendor_class_option_instances[i].vendor_class[j];

					if (vnd_cls.vendor_class_data_id != 0) {
						if (vnd_cls.vendor_class_data_id == vendor_class_data_id) {
							FREE_SAFE(client_config->config_opts.vendor_class_opt.vendor_class_option_instances[i].vendor_class[j].vendor_class_data);

							client_config->config_opts.vendor_class_opt.vendor_class_option_instances[i].vendor_class[j].vendor_class_data_id = 0;

							--client_config->config_opts.vendor_class_opt.vendor_class_option_instances[i].count;

							break;
						}
					}
				}
			}
		}
	}

	return 0;

error_out:
	return -1;
}

void dhcpv6_client_list_remove_vnd_cls(dhcpv6_client_config_t *client_config, uint32_t inst_pos)
{
	// iterate through all vendor classes in a vendor class instance for current interface
	for (uint32_t i = 0; i < MAX_VENDOR_CLASSES; i++) {
		vendor_class_t vnd_cls = client_config->config_opts.vendor_class_opt.vendor_class_option_instances[inst_pos].vendor_class[i];

		if (vnd_cls.vendor_class_data_id != 0) {
			// remove all vendor classes
			client_config->config_opts.vendor_class_opt.vendor_class_option_instances[inst_pos].vendor_class[i].vendor_class_data_id = 0;
			--client_config->config_opts.vendor_class_opt.vendor_class_option_instances[inst_pos].count;
		}
	}
}
*/

int dhcpv6_client_list_remove_vnd_spec_opt(config_data_list_t *ccl, char *if_name)
{
	dhcpv6_client_config_t *client_config = NULL;

	client_config = dhcpv6_client_list_get_client_config(ccl, if_name);

	if (client_config == NULL) {
		goto error_out;
	}

	--client_config->config_opts.vendor_specific_info_opts.count;

	return 0;

error_out:
	return -1;
}
