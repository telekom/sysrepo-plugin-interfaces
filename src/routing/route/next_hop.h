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

#ifndef ROUTING_ROUTE_NEXT_HOP_H
#define ROUTING_ROUTE_NEXT_HOP_H

#include <stddef.h>

enum route_next_hop_kind {
	route_next_hop_kind_none = 0,
	route_next_hop_kind_simple,
	route_next_hop_kind_special,
	route_next_hop_kind_list
};

// single next hop - interface index
struct route_next_hop_simple {
	struct nl_addr *addr;
	int ifindex;
	char *if_name;
};

// enum string value
struct route_next_hop_special {
	char *value;
};

struct route_next_hop_list_element {
	struct route_next_hop_simple simple;
	struct route_next_hop_list_element *next;
};

union route_next_hop_value {
	struct route_next_hop_simple simple;
	struct route_next_hop_special special;
	struct route_next_hop_list_element *list_head;
};

struct route_next_hop {
	enum route_next_hop_kind kind;
	union route_next_hop_value value;
};

void route_next_hop_init(struct route_next_hop *nh);
void route_next_hop_set_simple_gateway(struct route_next_hop *nh, struct nl_addr *gw);
void route_next_hop_set_simple_interface(struct route_next_hop *nh, int ifindex, const char *if_name);
void route_next_hop_set_simple(struct route_next_hop *nh, int ifindex, const char *if_name, struct nl_addr *gw);
void route_next_hop_set_special(struct route_next_hop *nh, char *value);
void route_next_hop_add_list(struct route_next_hop *nh, int ifindex, const char *if_name, struct nl_addr *gw);
struct route_next_hop route_next_hop_clone(struct route_next_hop *nh);
void route_next_hop_free(struct route_next_hop *nh);

#endif // ROUTING_ROUTE_NEXT_HOP_H
