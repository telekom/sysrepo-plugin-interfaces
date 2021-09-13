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

#include "dhcpv6_client.h"

#define BASE_YANG_MODEL "ietf-dhcpv6-client"
#define DHCPV6_CLIENT_YANG_MODEL "/" BASE_YANG_MODEL ":dhcpv6-client"

#define SYSREPOCFG_EMPTY_CHECK_COMMAND "sysrepocfg -X -d running -m " BASE_YANG_MODEL

#define DHCLIENT_RELEASE "dhclient -r"
#define RELEASE_CMD_LEN 12 + IFNAMSIZ // 11 is the length of "dhclient -r" string and +1 for the space

#define DHCLIENT_ENABLE "dhclient -6 -nw"
#define ENABLE_CMD_LEN 16 + IFNAMSIZ // 15 is the length of "dhclient -6 -nw" string and +1 for the space

static int dhcpv6_client_module_change_cb(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data);
int dhcpv6_client_state_data_cb(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *path, const char *request_xpath, uint32_t request_id, struct lyd_node **parent, void *private_data);

// utils
static bool is_running_datastore_empty(void);
static int dhcpv6_client_restart(void);

int dhcpv6_client_init(sr_session_ctx_t *session, sr_session_ctx_t *startup_session)
{
	int error = SR_ERR_OK;

	error = dhcpv6_client_config_file_read(&client_config_list);

	if (is_running_datastore_empty() == true) {
		SRP_LOG_INF("running datastore [%s] is empty, loading data", BASE_YANG_MODEL);

		error = dhcpv6_client_config_load_data(session);
		if (error) {
			SRP_LOG_ERR("dhcpv6_client_config_load_data error");
			return error;
		}

		error = sr_copy_config(startup_session, BASE_YANG_MODEL, SR_DS_RUNNING, 0);
		if (error) {
			SRP_LOG_ERR("sr_copy_config error (%d): %s", error, sr_strerror(error));
			return error;
		}
	}

	return error;
}

static bool is_running_datastore_empty(void)
{
	FILE *sysrepocfg_DS_empty_check = NULL;
	bool is_empty = false;

	sysrepocfg_DS_empty_check = popen(SYSREPOCFG_EMPTY_CHECK_COMMAND, "r");
	if (sysrepocfg_DS_empty_check == NULL) {
		SRP_LOG_WRN("could not execute %s", SYSREPOCFG_EMPTY_CHECK_COMMAND);
		is_empty = true;
		goto out;
	}

	if (fgetc(sysrepocfg_DS_empty_check) == EOF) {
		is_empty = true;
	}

out:
	if (sysrepocfg_DS_empty_check) {
		pclose(sysrepocfg_DS_empty_check);
	}

	return is_empty;
}

int dhcpv6_client_subscribe(sr_session_ctx_t *session, void **private_data, sr_subscription_ctx_t **subscription)
{
	int error = SR_ERR_OK;

	error = sr_module_change_subscribe(session, BASE_YANG_MODEL, "/" BASE_YANG_MODEL ":*//.", dhcpv6_client_module_change_cb, *private_data, 0, SR_SUBSCR_DEFAULT, subscription);
	if (error) {
		SRP_LOG_ERR("sr_module_change_subscribe error (%d): %s", error, sr_strerror(error));
		return error;
	}

	error = sr_oper_get_items_subscribe(session, BASE_YANG_MODEL, DHCPV6_CLIENT_YANG_MODEL "/*", dhcpv6_client_state_data_cb, NULL, SR_SUBSCR_DEFAULT, subscription);
	if (error) {
		SRP_LOG_ERR("sr_oper_get_items_subscribe error (%d): %s", error, sr_strerror(error));
		return error;
	}

	return error;
}

static int dhcpv6_client_module_change_cb(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data)
{
	int error = 0;
	sr_session_ctx_t *startup_session = (sr_session_ctx_t *) private_data;
	sr_change_iter_t *dhcpv6_client_change_iter = NULL;
	sr_change_oper_t operation = SR_OP_CREATED;
	const struct lyd_node *node = NULL;
	const char *prev_value = NULL;
	const char *prev_list = NULL;
	int prev_default = false;
	char *node_xpath = NULL;
	char *node_value = NULL;
	bool container_opts = false;

	SRP_LOG_INF("module_name: %s, xpath: %s, event: %d, request_id: %u", module_name, xpath, event, request_id);

	if (event == SR_EV_ABORT) {
		SRP_LOG_ERR("aborting changes for: %s", xpath);
		error = SR_ERR_CALLBACK_FAILED;
		goto out;
	}

	if (event == SR_EV_DONE) {
		error = sr_copy_config(startup_session, BASE_YANG_MODEL, SR_DS_RUNNING, 0);
		if (error) {
			SRP_LOG_ERR("sr_copy_config error (%d): %s", error, sr_strerror(error));
			goto out;
		}
	}

	if (event == SR_EV_CHANGE) {
		error = sr_get_changes_iter(session, xpath, &dhcpv6_client_change_iter);
		if (error) {
			SRP_LOG_ERR("sr_get_changes_iter error (%d): %s", error, sr_strerror(error));
			goto out;
		}

		while (sr_get_change_tree_next(session, dhcpv6_client_change_iter, &operation, &node, &prev_value, &prev_list, &prev_default) == SR_ERR_OK) {
			container_opts = false;

			node_xpath = lyd_path(node, LYD_PATH_STD, NULL, 0);

			/* rapid-commit-option and reconfigure-accept-option are just containers so check if they're in the xpath */
			if (strstr(node_xpath, "rapid-commit-option") != NULL || strstr(node_xpath, "reconfigure-accept-option") != NULL
				|| strstr(node_xpath, "option-request-option") != NULL || strstr(node_xpath, "vendor-specific-information-options") != NULL) {

				container_opts = true;
			}

			if (node->schema->nodetype == LYS_LEAF || node->schema->nodetype == LYS_LEAFLIST || container_opts) {
				if (container_opts == false) { // containers don't have node values
					node_value = xstrdup(lyd_get_value(node));
				}

				SRP_LOG_DBG("node_xpath: %s; prev_val: %s; node_val: %s; operation: %d", node_xpath, prev_value, node_value, operation);

				error = dhcpv6_client_config_set_value(session, operation, node_xpath, node_value, prev_value, &client_config_list);
				if (error != 0) {
					SRP_LOG_ERR("set_config_value error (%d)", error);
					goto out;
				}
			}

			FREE_SAFE(node_xpath);

			// node_value is only set when nodetype is: LYS_LEAF or LYS_LEAFLIST; or when xpath contains: rapid-commit-option or reconfigure-accept-option
			if (node_value != NULL) {
				FREE_SAFE(node_value);
			}
		}

		SRP_LOG_DBG("write client_config_list to file");

		error = dhcpv6_client_config_file_write_list(client_config_list);
		if (error != 0) {
			SRP_LOG_ERR("dhcpv6_client_config_file_write_list error (%d)", error);
			goto out;
		}

		// check if any if_name has to be deleted and
		// delete the interface name from list
		error = dhcpv6_client_list_check_remove_interface(&client_config_list);
		if (error != 0) {
			SRP_LOG_ERR("dhcpv6_client_list_check_remove_interface error");
			goto out;
		}

		// restart the dhclient service in order to update the new options
		error = dhcpv6_client_restart();
		if (error != 0) {
			SRP_LOG_ERR("dhcpv6_client_restart error");
			goto out;
		}
	}

out:
	if (node_xpath != NULL) {
		FREE_SAFE(node_xpath);
	}

	if (node_value != NULL) {
		FREE_SAFE(node_value);
	}

	if (dhcpv6_client_change_iter != NULL) {
		sr_free_change_iter(dhcpv6_client_change_iter);
	}

	return error ? SR_ERR_CALLBACK_FAILED : SR_ERR_OK;
}

static int dhcpv6_client_restart(void)
{
	int error = 0;

	error = system(DHCLIENT_RELEASE);
	if (error != 0) {
		SRP_LOG_ERR("\"%s\" failed with return value: %d", DHCLIENT_RELEASE, error);
		return -1;
	}

	error = system(DHCLIENT_ENABLE);
	if (error != 0) {
		SRP_LOG_ERR("\"%s\" failed with return value: %d", DHCLIENT_ENABLE, error);
		return -1;
	}

	return error;
}

int dhcpv6_client_enable(char *if_name)
{
	int error = 0;
	char cmd[ENABLE_CMD_LEN] = {0};

	error = snprintf(cmd, ENABLE_CMD_LEN, "dhclient -6 -nw %s", if_name);
	if (error < 0) {
		SRP_LOG_ERR("snprintf error");
		return -1;
	}

	error = system(cmd);
	if (error != 0) {
		SRP_LOG_ERR("\"%s\" failed with return value: %d", cmd, error);
		return -1;
	}

	return error;
}

int dhcpv6_client_release(char *if_name)
{
	int error = 0;
	char cmd[RELEASE_CMD_LEN] = {0};

	error = snprintf(cmd, RELEASE_CMD_LEN, "dhclient -r %s", if_name);
	if (error < 0) {
		SRP_LOG_ERR("snprintf error");
		return -1;
	}

	error = system(cmd);
	if (error != 0) {
		SRP_LOG_ERR("\"%s\" failed with return value: %d", cmd, error);
		return -1;
	}

	return error;
}

int dhcpv6_client_state_data_cb(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *path, const char *request_xpath, uint32_t request_id, struct lyd_node **parent, void *private_data)
{
	int error = SR_ERR_OK;
	const struct ly_ctx *ly_ctx = NULL;
	oper_data_t *oper_data = {0};

	if (*parent == NULL) {
		ly_ctx = sr_get_context(sr_session_get_connection(session));
		if (ly_ctx == NULL) {
			error = SR_ERR_CALLBACK_FAILED;
			goto error_out;
		}
		lyd_new_path(*parent, ly_ctx, request_xpath, NULL, 0, NULL);
	}

	// parse the dhclient6.leases file to gather operational data
	error = dhcpv6_client_leases_file_parse(oper_data);
	if (error != 0) {
		SRP_LOG_ERR("dhcpv6_client_leases_file_parse error");
		goto error_out;
	}

	// iterate through the gathered operational data and
	// create new nodes in the data tree based on corresponding xpaths
	error = dhcpv6_client_oper_create(oper_data, parent, ly_ctx);
	if (error != 0) {
		SRP_LOG_ERR("dhcpv6_client_oper_create error");
		goto error_out;
	}

	// cleanup oper_data
	dhcpv6_client_oper_cleanup(oper_data);

	goto out;

error_out:
	error = SR_ERR_CALLBACK_FAILED;

out:
	return error ? SR_ERR_CALLBACK_FAILED : SR_ERR_OK;
}
