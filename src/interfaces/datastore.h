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

#ifndef DATASTORE_H_ONCE
#define DATASTORE_H_ONCE

#define PLUGIN_NAME "interfaces-plugin"

#define BASE_YANG_MODEL "ietf-interfaces"
#define BASE_IP_YANG_MODEL "ietf-ip"
// config data
#define INTERFACES_YANG_MODEL "/" BASE_YANG_MODEL ":interfaces"
#define INTERFACE_LIST_YANG_PATH INTERFACES_YANG_MODEL "/interface"

enum interface_general_leaf {
	IF_TYPE = 0,
	IF_DESCRIPTION,
	IF_ENABLED,
	IF_PARENT_INTERFACE,
	IF_LEAF_COUNT
};

int ds_set_general_interface_config(sr_session_ctx_t *session, char *interface_name, char *leaf_values[IF_LEAF_COUNT]);

/* leaf nodes defined for both ipv4 and ipv6 */
enum interface_ip_leaf {
	IF_IP_ENABLED = 0,
	IF_IP_FORWARDING,
	IF_IP_MTU,
	IF_IP_LEAF_COUNT
};

int ds_set_interface_ipv4_config(sr_session_ctx_t *session, char *interface_name, char *leaf_values[IF_IP_LEAF_COUNT]);
int ds_set_interface_ipv6_config(sr_session_ctx_t *session, char *interface_name, char *leaf_values[IF_IP_LEAF_COUNT]);

enum interface_ipv4_addr_leaf {
	IF_IPV4_ADDR_SUBNET = 0,
	IF_IPV4_ADDR_PREFIX_LENGTH,
	IF_IPV4_ADDR_ORIGIN,
	IF_IPV4_ADDR_LEAF_COUNT
};

int ds_add_interface_ipv4_address(sr_session_ctx_t *session, char *interface_name, char *ip, char *leaf_values[IF_IPV4_ADDR_LEAF_COUNT]);

enum interface_ipv4_neighbour_leaf {
	IF_IPV4_NEIGH_LINK_LAYER_ADDR = 0,
	IF_IPV4_NEIGH_ORIGIN,
	IF_IPV4_NEIGH_LEAF_COUNT
};

int ds_add_interface_ipv4_neighbor(sr_session_ctx_t *session, char *interface_name, char *neigh_ip, char *leaf_values[IF_IPV4_NEIGH_LEAF_COUNT]);

enum interface_ipv6_addr_leaf {
	IF_IPV6_ADDR_PREFIX_LENGTH = 0,
	IF_IPV6_ADDR_ORIGIN,
	IF_IPV6_ADDR_STATUS,
	IF_IPV6_ADDR_LEAF_COUNT
};

int ds_add_interface_ipv6_address(sr_session_ctx_t *session, char *interface_name, char *ip, char *leaf_values[IF_IPV6_ADDR_LEAF_COUNT]);

enum interface_ipv6_neighbour_leaf {
	IF_IPV6_NEIGH_LINK_LAYER_ADDR = 0,
	IF_IPV6_NEIGH_ORIGIN,
	IF_IPV6_NEIGH_IS_ROUTER,
	IF_IPV6_NEIGH_STATE,
	IF_IPV6_NEIGH_LEAF_COUNT
};

int ds_add_interface_ipv6_neighbor(sr_session_ctx_t *session, char *interface_name, char *neigh_ip, char *leaf_values[IF_IPV6_NEIGH_LEAF_COUNT]);

#endif // DATASTORE_H_ONCE