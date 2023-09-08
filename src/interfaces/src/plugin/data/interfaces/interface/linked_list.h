/*
 * telekom / sysrepo-plugin-system
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
#ifndef INTERFACES_PLUGIN_DATA_INTERFACES_INTERFACE_LIST_H
#define INTERFACES_PLUGIN_DATA_INTERFACES_INTERFACE_LIST_H

#include "ipv4/address.h"
#include "ipv4/neighbor.h"
#include "ipv6/address.h"
#include "ipv6/neighbor.h"
#include "plugin/types.h"

#include <string.h>
#include <utlist.h>

/*
    Linked list operations
*/

#define INTERFACES_INTERFACE_LIST_NEW(ll_ptr) \
    do {                                      \
        (ll_ptr) = NULL;                      \
    } while (0)

#define INTERFACES_INTERFACE_LIST_FREE(ll_ptr)                                                 \
    _Generic((ll_ptr),                                                                         \
        interfaces_interface_ipv4_address_element_t *: interfaces_interface_ipv4_address_free, \
        interfaces_interface_ipv4_neighbor_element_t                                           \
        *: interfaces_interface_ipv4_neighbor_free,                                            \
        interfaces_interface_ipv6_address_element_t                                            \
        *: interfaces_interface_ipv6_address_free,                                             \
        interfaces_interface_ipv6_neighbor_element_t                                           \
        *: interfaces_interface_ipv6_neighbor_free)(&ll_ptr)

/* prepend since ordering doesn't matter - O(1) */
#define INTERFACES_INTERFACE_LIST_ADD_ELEMENT(ll_ptr, new_element_ptr) \
    do {                                                               \
        LL_PREPEND(ll_ptr, new_element_ptr);                           \
    } while (0)

#define INTERFACES_INTERFACE_LIST_GET_ELEMENT_STRING(ll_ptr, element_ptr, member, value) \
    do {                                                                                 \
        LL_FOREACH(ll_ptr, element_ptr)                                                  \
        {                                                                                \
            if (strcmp(element_ptr->member, value) == 0) {                               \
                break;                                                                   \
            }                                                                            \
        }                                                                                \
    } while (0)

#define INTERFACES_INTERFACE_LIST_GET_ELEMENT_SCALAR(ll_ptr, element_ptr, member, value) \
    do {                                                                                 \
        LL_SEARCH_SCALAR(ll_ptr, element_ptr, member, value);                            \
    } while (0)

#endif // INTERFACES_PLUGIN_DATA_INTERFACES_INTERFACE_LIST_H
