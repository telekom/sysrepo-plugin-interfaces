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

/*
int dhcpv6_client_list_modify_oro(config_data_list_t *ccl, char *if_name, char *prev_oro_opt, char *oro_opt)
{
	int error = 0;
	dhcpv6_client_config_t *client_config = NULL;

	error = dhcpv6_client_list_remove_oro(ccl, if_name, prev_oro_opt);
	if (error != 0) {
		goto error_out;
	}

	//	since remove decreases the count and this is a modify operation
	// 	which consists of first removing the entry and then adding it
	client_config = dhcpv6_client_list_get_client_config(ccl, if_name);

	if (client_config == NULL) {
		goto error_out;
	}

	// 	increase the counter again
	++client_config->config_opts.oro.count;

	error = dhcpv6_client_list_add_oro(ccl, if_name, oro_opt);
	if (error != 0) {
		goto error_out;
	}
	return 0;

error_out:
	return -1;
}*/

int dhcpv6_client_list_modify_usr_cls(config_data_list_t *ccl, char *if_name, char *prev_usr_cls, char *new_usr_cls)
{
	int error = 0;
	dhcpv6_client_config_t *client_config = NULL;

	error = dhcpv6_client_list_remove_usr_cls(ccl, if_name, prev_usr_cls);
	if (error != 0) {
		goto error_out;
	}

	// workaround:
	//	since remove decreases the count and this is a modify operation
	// 	which consists of first removing the entry and then adding it
	client_config = dhcpv6_client_list_get_client_config(ccl, if_name);

	if (client_config == NULL) {
		goto error_out;
	}

	// 	increase the counter again
	++client_config->config_opts.user_class_opt.count;

	error = dhcpv6_client_list_add_usr_cls(ccl, if_name, new_usr_cls);
	if (error != 0) {
		goto error_out;
	}

	return 0;

error_out:
	return -1;
}

/* vendor class is currently not implemented in dhclient
int dhcpv6_client_list_modify_vnd_cls_data(config_data_list_t *ccl, char *if_name, char *ent_num, char *vnd_cls_id, char *prev_vnd_cls_data, char *vnd_cls_data)
{
	int error = 0;
	dhcpv6_client_config_t *client_config = NULL;
	uint32_t enterprise_number = 0;
	uint8_t vendor_class_data_id = 0;

	error = dhcpv6_client_list_remove_vnd_cls_data(ccl, if_name, ent_num, vnd_cls_id, prev_vnd_cls_data);
	if (error != 0) {
		goto error_out;
	}

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
							client_config->config_opts.vendor_class_opt.vendor_class_option_instances[i].vendor_class[j].vendor_class_data = xstrdup(vnd_cls_data);
							goto out;
						}
					}
				}
			}
		}
	}

out:
	return 0;

error_out:
	return -1;
}
*/