/*
 * telekom / sysrepo-plugin-interfaces
 *
 * This program is made available under the terms of the
 * BSD 3-Clause license which is available at
 * https://opensource.org/licenses/BSD-3-Clause
 *
 * SPDX-FileCopyrightText: 2022 Deutsche Telekom AG
 * SPDX-FileContributor: Sartura Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef ROUTING_ROUTE_LIST_H
#define ROUTING_ROUTE_LIST_H

#include "route.h"

struct route_list_element {
	struct route route;
	struct route_list_element *next;
};

void route_list_init(struct route_list_element **head);
bool route_list_is_empty(struct route_list_element **head);
void route_list_add(struct route_list_element **head, struct route *route);
void route_list_free(struct route_list_element **head);

#endif // ROUTING_ROUTE_LIST_H
