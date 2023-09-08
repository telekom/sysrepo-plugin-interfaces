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

#include <netlink/addr.h>
#include <netlink/route/nexthop.h>
#include <sysrepo.h>

#include "route/next_hop.h"
#include "utils/memory.h"

#include <utlist.h>

void route_next_hop_init(struct route_next_hop* nh)
{
    nh->kind = route_next_hop_kind_none;
}

void route_next_hop_set_simple_gateway(struct route_next_hop* nh, struct nl_addr* gw)
{
    nh->kind = route_next_hop_kind_simple;
    if (gw) {
        nh->value.simple.addr = nl_addr_clone(gw);
    } else {
        nh->value.simple.addr = NULL;
    }
}

void route_next_hop_set_simple_interface(struct route_next_hop* nh, int ifindex, const char* if_name)
{
    nh->value.simple.ifindex = ifindex;
    nh->value.simple.if_name = xstrdup(if_name);
}

void route_next_hop_set_simple(struct route_next_hop* nh, int ifindex, const char* if_name, struct nl_addr* gw)
{
    route_next_hop_set_simple_gateway(nh, gw);
    route_next_hop_set_simple_interface(nh, ifindex, if_name);
}

void route_next_hop_set_special(struct route_next_hop* nh, char* value)
{
    nh->kind = route_next_hop_kind_special;
    if (value) {
        nh->value.special.value = xstrdup(value);
    }
}

void route_next_hop_add_list(struct route_next_hop* nh, int ifindex, const char* if_name, struct nl_addr* gw)
{
    struct route_next_hop_list_element* new_element = NULL;

    if (nh->kind == route_next_hop_kind_none) {
        nh->value.list_head = NULL;
        nh->kind = route_next_hop_kind_list;
    }

    new_element = xmalloc(sizeof(*new_element));
    new_element->next = NULL;
    new_element->simple.ifindex = ifindex;
    new_element->simple.if_name = xstrdup(if_name);
    if (gw) {
        new_element->simple.addr = nl_addr_clone(gw);
    } else {
        new_element->simple.addr = NULL;
    }

    LL_APPEND(nh->value.list_head, new_element);
}

struct route_next_hop route_next_hop_clone(struct route_next_hop* nh)
{
    struct route_next_hop out = { 0 };
    struct route_next_hop_list_element* list_iter = NULL;

    switch (nh->kind) {
    case route_next_hop_kind_none:
        break;
    case route_next_hop_kind_simple:
        route_next_hop_set_simple(&out, nh->value.simple.ifindex, nh->value.simple.if_name, nh->value.simple.addr);
        break;
    case route_next_hop_kind_special:
        route_next_hop_set_special(&out, nh->value.special.value);
        break;
    case route_next_hop_kind_list:
        LL_FOREACH(nh->value.list_head, list_iter)
        {
            route_next_hop_add_list(&out, list_iter->simple.ifindex, list_iter->simple.if_name, list_iter->simple.addr);
        }
        break;
    }

    return out;
}

void route_next_hop_free(struct route_next_hop* nh)
{
    struct route_next_hop_list_element *list_iter = NULL, *tmp = NULL;

    switch (nh->kind) {
    case route_next_hop_kind_none:
        break;
    case route_next_hop_kind_simple:
        if (nh->value.simple.addr) {
            nl_addr_put(nh->value.simple.addr);
        }

        if (nh->value.simple.if_name) {
            FREE_SAFE(nh->value.simple.if_name);
        }
        break;
    case route_next_hop_kind_special:
        if (nh->value.special.value != NULL) {
            FREE_SAFE(nh->value.special.value);
        }
        break;
    case route_next_hop_kind_list:
        LL_FOREACH_SAFE(nh->value.list_head, list_iter, tmp)
        {
            LL_DELETE(nh->value.list_head, list_iter);

            if (list_iter->simple.addr) {
                nl_addr_put(list_iter->simple.addr);
            }

            if (list_iter->simple.if_name) {
                FREE_SAFE(list_iter->simple.if_name);
            }

            free(list_iter);
        }
        break;
    }
    route_next_hop_init(nh);
}
