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

#include "dhcpv6_client_config.h"

int dhcpv6_client_config_load_data(sr_session_ctx_t *session)
{
	return 0;
}

int dhcpv6_client_config_set_value(sr_change_oper_t operation, const char *xpath, const char *value, const char *prev_value, config_data_list_t *ccl)
{
	/* if operation is SR_OP_CREATED, business as usual,
	 * if operation is SR_OP_MODIFIED, call deletion for prev_value and creation for value
	 * if operation is SR_OP_DELETED, delete
	 */
	int error = 0;
	char *leaf_node = NULL;
	char *interface_node_name = NULL;
	//char *vendor_class_opt_enterprise_num = NULL;
	//char *vendor_class_data_id = NULL;
	//char *vendor_specific_info_opts_enterprise_num = NULL;
	//char *xpath_cpy_vendor_class_id = NULL;
	//char *xpath_cpy_vendor_specific_sub_opt_code = NULL;
	//char *vendor_specific_sub_opt_code = NULL;
	sr_xpath_ctx_t state = {0};
	//char *xpath_cpy_vendor_class = NULL;
	//char *xpath_cpy_vendor_specific = NULL;
	//bool del_ent_num = false;

	// we have to copy the xpath value because sr_xpath_key_value modifies it (trims it)
	//xpath_cpy_vendor_class = xstrdup(xpath);
	//xpath_cpy_vendor_specific = xstrdup(xpath);

	//xpath_cpy_vendor_class_id = xstrdup(xpath);
	//xpath_cpy_vendor_specific_sub_opt_code = xstrdup(xpath);

	leaf_node = sr_xpath_node_name((char *) xpath);
	if (leaf_node == NULL) {
		SRP_LOG_ERR("sr_xpath_node_name error");
		goto error_out;
	}

	interface_node_name = sr_xpath_key_value((char *) xpath, "client-if", "if-name", &state);
	//vendor_class_opt_enterprise_num = sr_xpath_key_value(xpath_cpy_vendor_class, "vendor-class-option-instances", "enterprise-number", &state);
	//vendor_specific_info_opts_enterprise_num = sr_xpath_key_value(xpath_cpy_vendor_specific, "vendor-specific-information-option", "enterprise-number", &state);

	if (interface_node_name != NULL) {
		if (strcmp("if-name", leaf_node) == 0) {
			if (operation == SR_OP_CREATED) {
				error = dhcpv6_client_list_add_interface(ccl, interface_node_name);
				if (error != 0) {
					SRP_LOG_ERR("dhcpv6_client_list_add_interface error");
					goto error_out;
				}
			} else if (operation == SR_OP_DELETED) {
				// the interface name has to be deleted last, since all other removal operation depend on if_name to
				// find the specific entry in question
				// just set the del flag of this specific interface to true
				error = dhcpv6_client_list_set_del_if(ccl, interface_node_name);
				if (error != 0) {
					SRP_LOG_ERR("dhcpv6_client_list_set_del_if error");
					goto error_out;
				}
			}
		} else if (strcmp("enabled", leaf_node) == 0) {
			// TODO: implement

		} else if (strcmp("duid", leaf_node) == 0) {
			if (operation == SR_OP_CREATED) {
				error = dhcpv6_client_list_add_duid(ccl, interface_node_name, (char *)value);
				if (error != 0) {
					SRP_LOG_ERR("dhcpv6_client_list_add_duid error");
					goto error_out;
				}
			} else if (operation == SR_OP_MODIFIED) {
				// first remove the previous value from the list
				error = dhcpv6_client_list_remove_duid(ccl, interface_node_name, (char *)prev_value);
				if (error != 0) {
					SRP_LOG_ERR("dhcpv6_client_list_remove_duid error");
					goto error_out;
				}
				// then add the new value to the list
				error = dhcpv6_client_list_add_duid(ccl, interface_node_name, (char *)value);
				if (error != 0) {
					SRP_LOG_ERR("dhcpv6_client_list_add_duid error");
					goto error_out;
				}
			} else if (operation == SR_OP_DELETED) {
				error = dhcpv6_client_list_remove_duid(ccl, interface_node_name, (char *)value);
				if (error != 0) {
					SRP_LOG_ERR("dhcpv6_client_list_remove_duid error");
					goto error_out;
				}
			}
		} else if (strcmp("option-request-option", leaf_node) == 0) {
			/* only request is supported in dhclient:
			 *	'send dhcp6.oro' syntax is deprecated, please use the 'request' syntax ("man dhclient.conf")
			 * so just add: "request dhcp6.oro"
			 */
			if (operation == SR_OP_CREATED) {
				error = dhcpv6_client_list_add_oro(ccl, interface_node_name, (char *)value);
				if (error != 0) {
					SRP_LOG_ERR("dhcpv6_client_list_add_oro error");
					goto error_out;
				}
			} else if (operation == SR_OP_DELETED) {
				error = dhcpv6_client_list_remove_oro(ccl, interface_node_name, (char *)value);
				if (error != 0) {
					SRP_LOG_ERR("dhcpv6_client_list_remove_oro error");
					goto error_out;
				}
			}
		} else if (strstr(leaf_node, "oro-option") != NULL) { // strstr because e.g.: oro-option[.='5']
			if (operation == SR_OP_CREATED) {
				SRP_LOG_ERR("adding (sending) a list of ORO is not supported by dhclient");
				goto error_out;
				/*error = dhcpv6_client_list_add_oro(ccl, interface_node_name, (char *)value);
				if (error != 0) {
					SRP_LOG_ERR("dhcpv6_client_list_add_oro error");
					goto error_out;
				}*/
			} else if (operation == SR_OP_MODIFIED) {
				SRP_LOG_ERR("modification of ORO is not supported");
				goto error_out;
				/*
				error = dhcpv6_client_list_modify_oro(ccl, interface_node_name, (char *)prev_value, (char *)value);
				if (error != 0) {
					SRP_LOG_ERR("dhcpv6_client_list_modify_oro error");
					goto error_out;
				}*/
			} else if (operation == SR_OP_DELETED) {
				SRP_LOG_ERR("deleting list of ORO is not supported by dhclient");
				goto error_out;
				/*error = dhcpv6_client_list_remove_oro(ccl, interface_node_name, (char *)value);
				if (error != 0) {
					SRP_LOG_ERR("dhcpv6_client_list_remove_oro error");
					goto error_out;
				}*/
			}
		} else if (strcmp("rapid-commit-option", leaf_node) == 0) {
			if (operation == SR_OP_CREATED) {
				error = dhcpv6_client_list_add_rpd_cmt(ccl, interface_node_name);
				if (error != 0) {
					SRP_LOG_ERR("dhcpv6_client_list_add_rpd_cmt error");
					goto error_out;
				}
			} else if (operation == SR_OP_DELETED) {
				error = dhcpv6_client_list_remove_rpd_cmt(ccl, interface_node_name);
				if (error != 0) {
					SRP_LOG_ERR("dhcpv6_client_list_remove_rpd_cmt error");
					goto error_out;
				}
			}
		} else if (strcmp("user-class-data-id", leaf_node) == 0) {
			// dhclient.conf only supports option user-class "string_value", so just use user-class-data and not user-class-data-id
		} else if (strcmp("user-class-data", leaf_node) == 0) {
			if (operation == SR_OP_CREATED) {
				error = dhcpv6_client_list_add_usr_cls(ccl, interface_node_name, (char *)value);
				if (error != 0) {
					SRP_LOG_ERR("dhcpv6_client_list_add_usr_cls error");
					goto error_out;
				}
			} else if (operation == SR_OP_MODIFIED) {
				error = dhcpv6_client_list_modify_usr_cls(ccl, interface_node_name, (char *)prev_value, (char *)value);
				if (error != 0) {
					SRP_LOG_ERR("dhcpv6_client_list_modify_usr_cls error");
					goto error_out;
				}
			} else if (operation == SR_OP_DELETED) {
				error = dhcpv6_client_list_remove_usr_cls(ccl, interface_node_name, (char *)value);
				if (error != 0) {
					SRP_LOG_ERR("dhcpv6_client_list_remove_usr_cls error");
					goto error_out;
				}
			}
		} else if (strcmp("vendor-class-data-id", leaf_node) == 0) {
			SRP_LOG_ERR("vendor class option is not currently supported in dhclient");
			goto error_out;
		} else if (strcmp("vendor-specific-information-options", leaf_node) == 0) {
			if (operation == SR_OP_CREATED) {
				// currently only increments the counter
				error = dhcpv6_client_list_add_vnd_spec_opt(ccl, interface_node_name);
				if (error != 0) {
					SRP_LOG_ERR("dhcpv6_client_list_add_vnd_spec_opt error");
					goto error_out;
				}
			} else if (operation == SR_OP_DELETED) {
				// currently only decrements the counter
				error = dhcpv6_client_list_remove_vnd_spec_opt(ccl, interface_node_name);
				if (error != 0) {
					SRP_LOG_ERR("dhcpv6_client_list_remove_vnd_spec_opt error");
					goto error_out;
				}
			}
		}
		else if (strcmp("enterprise-number", leaf_node) == 0) {
			SRP_LOG_ERR("sending enterprise-number by client is currently not supported by dhclient");
			goto error_out;
		} else if (strcmp("sub-option-code", leaf_node) == 0) {
			SRP_LOG_ERR("sending sub-option-code by client is currently not supported by dhclient");
			goto error_out;
		}  else if (strcmp("sub-option-data", leaf_node) == 0) {
			SRP_LOG_ERR("sending sub-option-code by client is currently not supported by dhclient");
			goto error_out;
		}  /* vendor class is not implemented in dhclient
		 else if (strcmp("vendor-class-data-id", leaf_node) == 0) {
			// set when vendor-class-data leaf is processed
			// when vendor class data id is changed it is always followed with vendor class data
			// only when operation == SR_OP_DELETED is encountered remove the vendor class id from the list
			if (operation == SR_OP_DELETED) {
				vendor_class_data_id = sr_xpath_key_value(xpath_cpy_vendor_class_id, "vendor-class-data-element", "vendor-class-data-id", &state);
				if (vendor_class_data_id == NULL) {
					SRP_LOG_ERR("couldn't get vendor-class-data-id");
					goto error_out;
				}

				error = dhcpv6_client_list_remove_vnd_cls_id(ccl, interface_node_name, vendor_class_opt_enterprise_num, vendor_class_data_id, (char *)value);
				if (error != 0) {
					SRP_LOG_ERR("dhcpv6_client_list_remove_vnd_cls_id error");
					goto error_out;
				}
			}

		} else if (strcmp("vendor-class-data", leaf_node) == 0) {
			vendor_class_data_id = sr_xpath_key_value(xpath_cpy_vendor_class_id, "vendor-class-data-element", "vendor-class-data-id", &state);
			if (vendor_class_data_id == NULL) {
				SRP_LOG_ERR("couldn't get vendor-class-data-id");
				goto error_out;
			}

			if (operation == SR_OP_CREATED) {
				error = dhcpv6_client_list_add_vnd_cls_opt(ccl, interface_node_name, vendor_class_opt_enterprise_num, vendor_class_data_id, (char *)value);
				if (error != 0) {
					SRP_LOG_ERR("dhcpv6_client_list_add_vnd_cls_opt error");
					goto error_out;
				}
			} else if (operation == SR_OP_MODIFIED) {
				error = dhcpv6_client_list_modify_vnd_cls_data(ccl, interface_node_name, vendor_class_opt_enterprise_num, vendor_class_data_id, (char *)prev_value, (char *)value);
				if (error != 0) {
					SRP_LOG_ERR("dhcpv6_client_list_modify_vnd_cls_data error");
					goto error_out;
				}
			} else if (operation == SR_OP_DELETED) {
				error = dhcpv6_client_list_remove_vnd_cls_data(ccl, interface_node_name, vendor_class_opt_enterprise_num, vendor_class_data_id, (char *)value);
				if (error != 0) {
					SRP_LOG_ERR("dhcpv6_client_list_remove_vnd_cls_data error");
					goto error_out;
				}

				// the enterprise number has to be deleted last
				if (del_ent_num == true) {
					error = dhcpv6_client_list_remove_vnd_cls_opt(ccl, interface_node_name, vendor_class_opt_enterprise_num);
					if (error != 0) {
						SRP_LOG_ERR("dhcpv6_client_list_remove_vnd_cls_opt error");
						goto error_out;
					}
				}
			}
		}*/ 
		else if (strcmp("reconfigure-accept-option", leaf_node) == 0) {
			if (operation == SR_OP_CREATED) {
				error = dhcpv6_client_list_add_rcn_acpt(ccl, interface_node_name);
				if (error != 0) {
					SRP_LOG_ERR("dhcpv6_client_list_add_rcn_acpt error");
					goto error_out;
				}
			} else if (operation == SR_OP_DELETED) {
				error = dhcpv6_client_list_remove_rcn_acpt(ccl, interface_node_name);
				if (error != 0) {
					SRP_LOG_ERR("dhcpv6_client_list_remove_rcn_acpt error");
					goto error_out;
				}
			}
		}

		// TODO: add ia-na and ia-pd
	}

	/*FREE_SAFE(xpath_cpy_vendor_class);
	FREE_SAFE(xpath_cpy_vendor_specific);
	FREE_SAFE(xpath_cpy_vendor_class_id);
	FREE_SAFE(xpath_cpy_vendor_specific_sub_opt_code);
	*/

	return SR_ERR_OK;

error_out:
	/*if (xpath_cpy_vendor_class != NULL) {
		FREE_SAFE(xpath_cpy_vendor_class);
	}

	if (xpath_cpy_vendor_specific != NULL) {
		FREE_SAFE(xpath_cpy_vendor_specific);
	}

	if (xpath_cpy_vendor_class_id != NULL) {
		FREE_SAFE(xpath_cpy_vendor_class_id);
	}

	if (xpath_cpy_vendor_specific_sub_opt_code != NULL) {
		FREE_SAFE(xpath_cpy_vendor_specific_sub_opt_code);
	}*/

	return SR_ERR_CALLBACK_FAILED;
}
