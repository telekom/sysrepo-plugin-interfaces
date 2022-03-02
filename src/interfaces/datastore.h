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

#define BASE_YANG_MODEL "ietf-interfaces"
#define BASE_IP_YANG_MODEL "ietf-ip"
// config data
#define INTERFACES_YANG_MODEL "/" BASE_YANG_MODEL ":interfaces"
#define INTERFACE_LIST_YANG_PATH INTERFACES_YANG_MODEL "/interface"

enum interface_general_leaf {
	IF_NAME = 0,
	IF_TYPE,
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
	IF_IPV4_ADDR_IP = 0,
	IF_IPV4_ADDR_SUBNET,
	IF_IPV4_ADDR_PREFIX_LENGTH,
	IF_IPV4_ADDR_ORIGIN,
	IF_IPV4_ADDR_LEAF_COUNT
};

int ds_add_interface_ipv4_address(sr_session_ctx_t *session, char *interface_name, char *leaf_values[IF_IPV4_ADDR_LEAF_COUNT]);

enum interface_ipv4_neighbour_leaf {
	IF_IPV4_NEIGH_IP = 0,
	IF_IPV4_NEIGH_LINK_LAYER_ADDR,
	IF_IPV4_NEIGH_ORIGIN,
	IF_IPV4_NEIGH_LEAF_COUNT
};

int ds_add_interface_ipv4_neighbor(sr_session_ctx_t *session, char *interface_name, char *leaf_values[IF_IPV4_NEIGH_LEAF_COUNT]);

enum interface_ipv6_addr_leaf {
	IF_IPV6_ADDR_IP = 0,
	IF_IPV6_ADDR_PREFIX_LENGTH,
	IF_IPV6_ADDR_ORIGIN,
	IF_IPV6_ADDR_STATUS,
	IF_IPV6_ADDR_LEAF_COUNT
};

int ds_add_interface_ipv6_address(sr_session_ctx_t *session, char *interface_name, char *leaf_values[IF_IPV6_ADDR_LEAF_COUNT]);

enum interface_ipv6_neighbour_leaf {
	IF_IPV6_NEIGH_IP = 0,
	IF_IPV6_NEIGH_LINK_LAYER_ADDR,
	IF_IPV6_NEIGH_ORIGIN,
	IF_IPV6_NEIGH_IS_ROUTER,
	IF_IPV6_NEIGH_STATE,
	IF_IPV6_NEIGH_LEAF_COUNT
};

int ds_add_interface_ipv6_neighbor(sr_session_ctx_t *session, char *interface_name, char *leaf_values[IF_IPV6_NEIGH_LEAF_COUNT]);

enum interface_oper_info_leaf {
	IF_ADMIN_STATUS,
	IF_OPER_STATUS,
	IF_LAST_CHANGE,
	IF_IF_INDEX,
	IF_PHYS_ADDRESS,
	IF_SPEED,
	IF_OPER_LEAF_COUNT
};

int ds_oper_set_interface_info(struct lyd_node *parent, const struct ly_ctx *ly_ctx, char *interface_name, char *leaf_values[IF_OPER_LEAF_COUNT]);

int ds_oper_add_interface_higher_layer_if(struct lyd_node *parent, const struct ly_ctx *ly_ctx, char *interface_name, char *higher_layer_if);
int ds_oper_add_interface_lower_layer_if(struct lyd_node *parent, const struct ly_ctx *ly_ctx, char *interface_name, char *lower_layer_if);

enum interface_statistics_leaf {
	IF_STATS_DISCONTINUITY_TIME,
	IF_STATS_IN_OCTETS,
	IF_STATS_IN_UNICAST_PKTS,
	IF_STATS_IN_BROADCAST_PKTS,
	IF_STATS_IN_MULTICAST_PKTS,
	IF_STATS_IN_DISCARDS,
	IF_STATS_IN_ERRORS,
	IF_STATS_IN_UNKNOWN_PROTOS,
	IF_STATS_OUT_OCTETS,
	IF_STATS_OUT_UNICAST_PKTS,
	IF_STATS_OUT_BROADCAST_PKTS,
	IF_STATS_OUT_MULTICAST_PKTS,
	IF_STATS_OUT_DISCARDS,
	IF_STATS_OUT_ERRORS,
	IF_STATS_LEAF_COUNT
};

int ds_oper_set_interface_statistics(struct lyd_node *parent, const struct ly_ctx *ly_ctx, char *interface_name, char *leaf_values[IF_STATS_LEAF_COUNT]);

#endif // DATASTORE_H_ONCE