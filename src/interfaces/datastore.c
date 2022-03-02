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
#include <stdbool.h>

#include <sysrepo.h>
#include <libyang/libyang.h>
#include <libyang/tree_data.h>

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
			SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf error (%d): %s", error, strerror(error));
			goto out;
		}
		error = sr_set_item_str(session, xpath, leaf_values[i], NULL, SR_EDIT_DEFAULT);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "sr_set_item_str error (%d): %s", error, sr_strerror(error));
			goto out;
		}
		if (log_changes) {
			SRPLG_LOG_DBG(PLUGIN_NAME, "%s = %s", xpath, leaf_values[i]);
		}
	}
out:
	return error;
}

static char *interface_leaf_names[IF_LEAF_COUNT] = {
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
		SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf error (%d): %s", error, strerror(error));
		goto out;
	}
	error = ds_set_leaf_values(session, base_xpath, interface_leaf_names,
				   IF_LEAF_COUNT, leaf_values, true);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "ds_set_leaf_values error");
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
		SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf error (%d): %s", error, strerror(error));
		goto out;
	}
	error = ds_set_leaf_values(session, base_xpath, interface_ip_leaf_names,
				   IF_IP_LEAF_COUNT, leaf_values, true);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "ds_set_leaf_values error");
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
	[IF_IPV4_ADDR_SUBNET] = "subnet",
	[IF_IPV4_ADDR_PREFIX_LENGTH] = "prefix-length",
	[IF_IPV4_ADDR_ORIGIN] = "origin"
};

int ds_add_interface_ipv4_address(sr_session_ctx_t *session, char *interface_name,
				  char *ip, char *leaf_values[IF_IPV4_ADDR_LEAF_COUNT])
{
	int error = SR_ERR_OK;
	char base_xpath[PATH_MAX] = {0};
	error = snprintf(base_xpath, sizeof(base_xpath), "%s[name=\"%s\"]/ietf-ip:ipv4/address[ip='%s']",
			 INTERFACE_LIST_YANG_PATH, interface_name, ip);
	if (error < 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf error (%d): %s", error, strerror(error));
		goto out;
	}
	error = ds_set_leaf_values(session, base_xpath, interface_ipv4_addr_leaf_names,
				   IF_IPV4_ADDR_LEAF_COUNT, leaf_values, true);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "ds_set_leaf_values error");
	}
out:
	return error;
}

static char *interface_ipv4_neigh_leaf_names[IF_IPV4_NEIGH_LEAF_COUNT] = {
	[IF_IPV4_NEIGH_LINK_LAYER_ADDR] = "link-layer-address",
	[IF_IPV4_NEIGH_ORIGIN] = "origin"
};

int ds_add_interface_ipv4_neighbor(sr_session_ctx_t *session, char *interface_name,
				   char *neigh_ip, char *leaf_values[IF_IPV4_NEIGH_LEAF_COUNT])
{
	int error = SR_ERR_OK;
	char base_xpath[PATH_MAX] = {0};
	error = snprintf(base_xpath, sizeof(base_xpath), "%s[name=\"%s\"]/ietf-ip:ipv4/neighbor[ip='%s']",
			 INTERFACE_LIST_YANG_PATH, interface_name, neigh_ip);
	if (error < 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf error (%d): %s", error, strerror(error));
		goto out;
	}
	error = ds_set_leaf_values(session, base_xpath, interface_ipv4_neigh_leaf_names,
				   IF_IPV4_NEIGH_LEAF_COUNT, leaf_values, true);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "ds_set_leaf_values error");
	}
out:
	return error;
}

static char *interface_ipv6_addr_leaf_names[IF_IPV6_ADDR_LEAF_COUNT] = {
	[IF_IPV6_ADDR_PREFIX_LENGTH] = "prefix-length",
	[IF_IPV6_ADDR_ORIGIN] = "origin",
	[IF_IPV6_ADDR_STATUS] = "status"
};

int ds_add_interface_ipv6_address(sr_session_ctx_t *session, char *interface_name,
				  char *ip, char *leaf_values[IF_IPV6_ADDR_LEAF_COUNT])
{
	int error = SR_ERR_OK;
	char base_xpath[PATH_MAX] = {0};
	error = snprintf(base_xpath, sizeof(base_xpath), "%s[name=\"%s\"]/ietf-ip:ipv6/address[ip='%s']",
			 INTERFACE_LIST_YANG_PATH, interface_name, ip);
	if (error < 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf error (%d): %s", error, strerror(error));
		goto out;
	}
	error = ds_set_leaf_values(session, base_xpath, interface_ipv6_addr_leaf_names,
				   IF_IPV6_ADDR_LEAF_COUNT, leaf_values, true);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "ds_set_leaf_values error");
	}
out:
	return error;
}

static char *interface_ipv6_neigh_leaf_names[IF_IPV6_NEIGH_LEAF_COUNT] = {
	[IF_IPV6_NEIGH_LINK_LAYER_ADDR] = "link-layer-address",
	[IF_IPV6_NEIGH_ORIGIN] = "origin",
	[IF_IPV6_NEIGH_IS_ROUTER] = "is-router",
	[IF_IPV6_NEIGH_STATE] = "state"
};

int ds_add_interface_ipv6_neighbor(sr_session_ctx_t *session, char *interface_name,
				   char *neigh_ip, char *leaf_values[IF_IPV6_NEIGH_LEAF_COUNT])
{
	int error = SR_ERR_OK;
	char base_xpath[PATH_MAX] = {0};
	error = snprintf(base_xpath, sizeof(base_xpath), "%s[name=\"%s\"]/ietf-ip:ipv6/neighbor[ip='%s']",
			 INTERFACE_LIST_YANG_PATH, interface_name, neigh_ip);
	if (error < 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf error (%d): %s", error, strerror(error));
		goto out;
	}
	error = ds_set_leaf_values(session, base_xpath, interface_ipv6_neigh_leaf_names,
				   IF_IPV6_NEIGH_LEAF_COUNT, leaf_values, true);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "ds_set_leaf_values error");
	}
out:
	return error;
}

static char *interface_oper_info_leaf_names[IF_OPER_LEAF_COUNT] = {
	[IF_ADMIN_STATUS] = "admin-status",
	[IF_OPER_STATUS] = "oper-status",
	[IF_LAST_CHANGE] = "last-change",
	[IF_IF_INDEX] = "if-index",
	[IF_PHYS_ADDRESS] = "phys-address",
	[IF_SPEED] = "speed"
};

int ds_oper_set_interface_info(struct lyd_node *parent, const struct ly_ctx *ly_ctx, char *interface_name,
			       char *leaf_values[IF_OPER_LEAF_COUNT])
{
	int error = SR_ERR_OK;
	for (uint32_t i = 0; i < IF_OPER_LEAF_COUNT; i++) {
		if (leaf_values[i] == NULL) {
			continue;
		}
		char xpath[PATH_MAX] = {0};
		error = snprintf(xpath, sizeof(xpath), "%s[name=\"%s\"]/%s", INTERFACE_LIST_YANG_PATH,
				 interface_name, interface_oper_info_leaf_names[i]);
		if (error < 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf error (%d): %s", error, strerror(error));
			goto out;
		}
		error = 0;
		SRPLG_LOG_DBG(PLUGIN_NAME, "%s = %s", xpath, leaf_values[i]);
		error = lyd_new_path(parent, ly_ctx, xpath, leaf_values[i], LYD_ANYDATA_STRING, NULL);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "lyd_new_path error (%d)", error);
			goto out;
		}
	}
out:
	return error;
}

int ds_oper_add_interface_higher_layer_if(struct lyd_node *parent, const struct ly_ctx *ly_ctx,
					  char *interface_name, char *higher_layer_if)
{
	int error = SR_ERR_OK;
	char xpath[PATH_MAX] = {0};
	error = snprintf(xpath, sizeof(xpath), "%s[name=\"%s\"]/higher-layer-if",
			 INTERFACE_LIST_YANG_PATH, interface_name);
	if (error < 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf error (%d): %s", error, strerror(error));
		goto out;
	}
	error = 0;
	SRPLG_LOG_DBG(PLUGIN_NAME, "%s += %s", xpath, higher_layer_if);
	error = lyd_new_path(parent, ly_ctx, xpath, higher_layer_if, LYD_ANYDATA_STRING, NULL);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "lyd_new_path error (%d)", error);
	}
out:
	return error;
}

int ds_oper_add_interface_lower_layer_if(struct lyd_node *parent, const struct ly_ctx *ly_ctx,
					 char *interface_name, char *lower_layer_if)
{
	int error = SR_ERR_OK;
	char xpath[PATH_MAX] = {0};
	error = snprintf(xpath, sizeof(xpath), "%s[name=\"%s\"]/lower-layer-if",
			 INTERFACE_LIST_YANG_PATH, interface_name);
	if (error < 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf error (%d): %s", error, strerror(error));
		goto out;
	}
	error = 0;
	SRPLG_LOG_DBG(PLUGIN_NAME, "%s += %s", xpath, lower_layer_if);
	error = lyd_new_path(parent, ly_ctx, xpath, lower_layer_if, LYD_ANYDATA_STRING, NULL);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "lyd_new_path error (%d)", error);
	}
out:
	return error;
}

static char *interface_statistics_leaf_names[IF_STATS_LEAF_COUNT] = {
	[IF_STATS_DISCONTINUITY_TIME] = "discontinuity-time",
	[IF_STATS_IN_OCTETS] = "in-octets",
	[IF_STATS_IN_UNICAST_PKTS] = "in-unicast-pkts",
	[IF_STATS_IN_BROADCAST_PKTS] = "in-broadcast-pkts",
	[IF_STATS_IN_MULTICAST_PKTS] = "in-multicast-pkts",
	[IF_STATS_IN_DISCARDS] = "in-discards",
	[IF_STATS_IN_ERRORS] = "in-errors",
	[IF_STATS_IN_UNKNOWN_PROTOS] = "in-unknown-protos",
	[IF_STATS_OUT_OCTETS] = "out-octets",
	[IF_STATS_OUT_UNICAST_PKTS] = "out-unicast-pkts",
	[IF_STATS_OUT_BROADCAST_PKTS] = "out-broadcast-pkts",
	[IF_STATS_OUT_MULTICAST_PKTS] = "out-multicast-pkts",
	[IF_STATS_OUT_DISCARDS] = "out-discards",
	[IF_STATS_OUT_ERRORS] = "out-errors"
};

int ds_oper_set_interface_statistics(struct lyd_node *parent, const struct ly_ctx *ly_ctx,
				     char *interface_name, char *leaf_values[IF_STATS_LEAF_COUNT])
{
	int error = SR_ERR_OK;
	for (uint32_t i = 0; i < IF_STATS_LEAF_COUNT; i++) {
		if (leaf_values[i] == NULL) {
			continue;
		}
		char xpath[PATH_MAX] = {0};
		error = snprintf(xpath, sizeof(xpath), "%s[name=\"%s\"]/statistics/%s", INTERFACE_LIST_YANG_PATH,
				 interface_name, interface_statistics_leaf_names[i]);
		if (error < 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf error (%d): %s", error, strerror(error));
			goto out;
		}
		error = 0;
		SRPLG_LOG_DBG(PLUGIN_NAME, "%s = %s", xpath, leaf_values[i]);
		error = lyd_new_path(parent, ly_ctx, xpath, leaf_values[i], LYD_ANYDATA_STRING, NULL);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "lyd_new_path error (%d)", error);
			goto out;
		}
	}
out:
	return error;
}