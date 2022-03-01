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

#include <string.h>
#include <stdlib.h>
#include <linux/limits.h>
#include <sysrepo.h>

#include "datastore.h"

/*
 * Set the value of multiple YANG module leafs. The path for each leaf node is determined
 * by concatenating base_xpath with its name from leaf_names. All leaf nodes with a non-NULL
 * entry in leaf_values will be set.
 */
static int ds_set_leaf_values(sr_session_ctx_t *session, char *base_xpath, char **leaf_names,
			      uint32_t leaf_count, char **leaf_values, bool log_changes)
{
	int error = SR_ERR_OK;
	char xpath[PATH_MAX] = {0};

	for (uint32_t i = 0; i < leaf_count; i++) {
		if (leaf_values[i] == NULL) {
			continue;
		}
		error = snprintf(xpath, sizeof(xpath), "%s/%s", base_xpath, leaf_names[i]);
		if (error < 0) {
			SRP_LOG_ERR("snprintf error (%d): %s", error, strerror(error));
			goto out;
		}
		error = sr_set_item_str(session, xpath, leaf_values[i], NULL, SR_EDIT_DEFAULT);
		if (error) {
			SRP_LOG_ERR("sr_set_item_str error (%d): %s", error, sr_strerror(error));
			goto out;
		}
		if (log_changes) {
			SRP_LOG_DBG("%s = %s", xpath, leaf_values[i]);
		}
	}
out:
	return error;
}

static char *interface_leaf_names[IF_LEAF_COUNT] = {
	[IF_NAME] = "name",
	[IF_TYPE] = "type",
	[IF_DESCRIPTION] = "description",
	[IF_ENABLED] = "enabled",
	[IF_PARENT_INTERFACE] = "ietf-if-extensions:parent-interface"
};

int ds_set_general_interface_config(sr_session_ctx_t *session, char *interface_name,
				    char *leaf_values[IF_LEAF_COUNT])
{
	int error = SR_ERR_OK;
	char base_xpath[PATH_MAX] = {0};
	error = snprintf(base_xpath, sizeof(base_xpath), "%s[name=\"%s\"]",
			 INTERFACE_LIST_YANG_PATH, interface_name);
	if (error < 0) {
		SRP_LOG_ERR("snprintf error (%d): %s", error, strerror(error));
		goto out;
	}
	error = ds_set_leaf_values(session, base_xpath, interface_leaf_names,
				   IF_LEAF_COUNT, leaf_values, true);
	if (error) {
		SRP_LOG_ERR("ds_set_leaf_values error");
	}
out:
	return error;
}

/* Leaf nodes defined for both ipv4 and ipv6 */
static char *interface_ip_leaf_names[IF_IP_LEAF_COUNT] = {
	[IF_IP_ENABLED] = "enabled",
	[IF_IP_FORWARDING] = "forwarding",
	[IF_IP_MTU] = "mtu"
};

int ds_set_interface_ip_config(sr_session_ctx_t *session, char *interface_name, char *ip_version,
			       char *leaf_values[IF_IP_LEAF_COUNT])
{
	int error = SR_ERR_OK;
	char base_xpath[PATH_MAX] = {0};
	error = snprintf(base_xpath, sizeof(base_xpath), "%s[name=\"%s\"]/ietf-ip:%s",
			 INTERFACE_LIST_YANG_PATH, interface_name, ip_version);
	if (error < 0) {
		SRP_LOG_ERR("snprintf error (%d): %s", error, strerror(error));
		goto out;
	}
	error = ds_set_leaf_values(session, base_xpath, interface_ip_leaf_names,
				   IF_IP_LEAF_COUNT, leaf_values, true);
	if (error) {
		SRP_LOG_ERR("ds_set_leaf_values error");
	}
out:
	return error;
}

int ds_set_interface_ipv4_config(sr_session_ctx_t *session, char *interface_name,
				 char *leaf_values[IF_IP_LEAF_COUNT])
{
	return ds_set_interface_ip_config(session, interface_name, "ipv4", leaf_values);
}

int ds_set_interface_ipv6_config(sr_session_ctx_t *session, char *interface_name,
				 char *leaf_values[IF_IP_LEAF_COUNT])
{
	return ds_set_interface_ip_config(session, interface_name, "ipv6", leaf_values);
}

static char *interface_ipv4_addr_leaf_names[IF_IPV4_ADDR_LEAF_COUNT] = {
	[IF_IPV4_ADDR_IP] = "ip",
	[IF_IPV4_ADDR_SUBNET] = "subnet",
	[IF_IPV4_ADDR_PREFIX_LENGTH] = "prefix-length",
	[IF_IPV4_ADDR_ORIGIN] = "origin"
};

int ds_add_interface_ipv4_address(sr_session_ctx_t *session, char *interface_name,
				  char *leaf_values[IF_IPV4_ADDR_LEAF_COUNT])
{
	int error = SR_ERR_OK;
	char base_xpath[PATH_MAX] = {0};
	error = snprintf(base_xpath, sizeof(base_xpath), "%s[name=\"%s\"]/ietf-ip:ipv4/address[ip='%s']",
			 INTERFACE_LIST_YANG_PATH, interface_name, leaf_values[IF_IPV4_ADDR_IP]);
	if (error < 0) {
		SRP_LOG_ERR("snprintf error (%d): %s", error, strerror(error));
		goto out;
	}
	error = ds_set_leaf_values(session, base_xpath, interface_ipv4_addr_leaf_names,
				   IF_IPV4_ADDR_LEAF_COUNT, leaf_values, true);
	if (error) {
		SRP_LOG_ERR("ds_set_leaf_values error");
	}
out:
	return error;
}

static char *interface_ipv4_neigh_leaf_names[IF_IPV4_NEIGH_LEAF_COUNT] = {
	[IF_IPV4_NEIGH_IP] = "ip",
	[IF_IPV4_NEIGH_LINK_LAYER_ADDR] = "link-layer-address",
	[IF_IPV4_NEIGH_ORIGIN] = "origin"
};

int ds_add_interface_ipv4_neighbor(sr_session_ctx_t *session, char *interface_name,
				   char *leaf_values[IF_IPV4_NEIGH_LEAF_COUNT])
{
	int error = SR_ERR_OK;
	char base_xpath[PATH_MAX] = {0};
	error = snprintf(base_xpath, sizeof(base_xpath), "%s[name=\"%s\"]/ietf-ip:ipv4/neighbor[ip='%s']",
			 INTERFACE_LIST_YANG_PATH, interface_name, leaf_values[IF_IPV4_NEIGH_IP]);
	if (error < 0) {
		SRP_LOG_ERR("snprintf error (%d): %s", error, strerror(error));
		goto out;
	}
	error = ds_set_leaf_values(session, base_xpath, interface_ipv4_neigh_leaf_names,
				   IF_IPV4_NEIGH_LEAF_COUNT, leaf_values, true);
	if (error) {
		SRP_LOG_ERR("ds_set_leaf_values error");
	}
out:
	return error;
}

static char *interface_ipv6_addr_leaf_names[IF_IPV6_ADDR_LEAF_COUNT] = {
	[IF_IPV6_ADDR_IP] = "ip",
	[IF_IPV6_ADDR_PREFIX_LENGTH] = "prefix-length",
	[IF_IPV6_ADDR_ORIGIN] = "origin",
	[IF_IPV6_ADDR_STATUS] = "status"
};

int ds_add_interface_ipv6_address(sr_session_ctx_t *session, char *interface_name,
				  char *leaf_values[IF_IPV6_ADDR_LEAF_COUNT])
{
	int error = SR_ERR_OK;
	char base_xpath[PATH_MAX] = {0};
	error = snprintf(base_xpath, sizeof(base_xpath), "%s[name=\"%s\"]/ietf-ip:ipv6/address[ip='%s']",
			 INTERFACE_LIST_YANG_PATH, interface_name, leaf_values[IF_IPV6_ADDR_IP]);
	if (error < 0) {
		SRP_LOG_ERR("snprintf error (%d): %s", error, strerror(error));
		goto out;
	}
	error = ds_set_leaf_values(session, base_xpath, interface_ipv6_addr_leaf_names,
				   IF_IPV6_ADDR_LEAF_COUNT, leaf_values, true);
	if (error) {
		SRP_LOG_ERR("ds_set_leaf_values error");
	}
out:
	return error;
}

static char *interface_ipv6_neigh_leaf_names[IF_IPV6_NEIGH_LEAF_COUNT] = {
	[IF_IPV6_NEIGH_IP] = "ip",
	[IF_IPV6_NEIGH_LINK_LAYER_ADDR] = "link-layer-address",
	[IF_IPV6_NEIGH_ORIGIN] = "origin",
	[IF_IPV6_NEIGH_IS_ROUTER] = "is-router",
	[IF_IPV6_NEIGH_STATE] = "state"
};

int ds_add_interface_ipv6_neighbor(sr_session_ctx_t *session, char *interface_name,
				   char *leaf_values[IF_IPV6_NEIGH_LEAF_COUNT])
{
	int error = SR_ERR_OK;
	char base_xpath[PATH_MAX] = {0};
	error = snprintf(base_xpath, sizeof(base_xpath), "%s[name=\"%s\"]/ietf-ip:ipv6/neighbor[ip='%s']",
			 INTERFACE_LIST_YANG_PATH, interface_name, leaf_values[IF_IPV6_NEIGH_IP]);
	if (error < 0) {
		SRP_LOG_ERR("snprintf error (%d): %s", error, strerror(error));
		goto out;
	}
	error = ds_set_leaf_values(session, base_xpath, interface_ipv6_neigh_leaf_names,
				   IF_IPV6_NEIGH_LEAF_COUNT, leaf_values, true);
	if (error) {
		SRP_LOG_ERR("ds_set_leaf_values error");
	}
out:
	return error;
}