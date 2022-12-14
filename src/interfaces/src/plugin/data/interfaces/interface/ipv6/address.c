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
#include "address.h"
#include "plugin/types.h"
#include "src/utlist.h"
#include <stdlib.h>

interfaces_interface_ipv6_address_element_t* interfaces_interface_ipv6_address_new(void)
{
    return NULL;
}

int interfaces_interface_ipv6_address_add_element(interfaces_interface_ipv6_address_element_t** address, interfaces_interface_ipv6_address_element_t* new_element)
{
    LL_PREPEND(*address, new_element);
    return 0;
}

void interfaces_interface_ipv6_address_free(interfaces_interface_ipv6_address_element_t** address)
{
    interfaces_interface_ipv6_address_element_t *iter = NULL, *tmp = NULL;

    LL_FOREACH_SAFE(*address, iter, tmp)
    {
        // remove from list
        LL_DELETE(*address, iter);

        // free element data
        interfaces_interface_ipv6_address_element_free(&iter);
    }
}

interfaces_interface_ipv6_address_element_t* interfaces_interface_ipv6_address_element_new(void)
{
    interfaces_interface_ipv6_address_element_t* new_element = NULL;

    new_element = malloc(sizeof(interfaces_interface_ipv6_address_element_t));
    new_element->address = (interfaces_interface_ipv6_address_t) { 0 };

    return new_element;
}

int interfaces_interface_ipv6_address_element_set(interfaces_interface_ipv6_address_element_t** src,
    interfaces_interface_ipv6_address_element_t** dst)
{
    interfaces_interface_ipv6_address_element_t* src_iter = NULL;
    interfaces_interface_ipv6_address_element_t* new_elem = NULL;

    LL_FOREACH(*src, src_iter)
    {
        new_elem = interfaces_interface_ipv6_address_element_new();
        interfaces_interface_ipv6_address_element_set_ip(&new_elem, src_iter->address.ip);
        interfaces_interface_ipv6_address_element_set_prefix_length(&new_elem,
            src_iter->address.prefix_length);

        interfaces_interface_ipv6_address_add_element(dst, new_elem);
    }

    return 0;
}

void interfaces_interface_ipv6_address_element_free(interfaces_interface_ipv6_address_element_t** el)
{
    if (*el) {
        // IP
        if ((*el)->address.ip) {
            free((*el)->address.ip);
        }

        // address data
        free(*el);
        *el = NULL;
    }
}

int interfaces_interface_ipv6_address_element_set_ip(interfaces_interface_ipv6_address_element_t** el, const char* ip)
{
    if ((*el)->address.ip) {
        free((*el)->address.ip);
    }

    if (ip) {
        (*el)->address.ip = strdup(ip);
        return (*el)->address.ip == NULL;
    }

    return 0;
}

int interfaces_interface_ipv6_address_element_set_prefix_length(interfaces_interface_ipv6_address_element_t** el, uint8_t prefix_length)
{
    (*el)->address.prefix_length = prefix_length;

    return 0;
}
