#include "address.h"
#include "plugin/types.h"
#include "src/utlist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

interfaces_interface_ipv4_address_element_t* interfaces_interface_ipv4_address_new(void)
{
    return NULL;
}

int interfaces_interface_ipv4_address_add_element(interfaces_interface_ipv4_address_element_t** address, interfaces_interface_ipv4_address_element_t* new_element)
{
    LL_PREPEND(*address, new_element);
    return 0;
}

void interfaces_interface_ipv4_address_free(interfaces_interface_ipv4_address_element_t** address)
{
    interfaces_interface_ipv4_address_element_t *iter = NULL, *tmp = NULL;

    LL_FOREACH_SAFE(*address, iter, tmp)
    {
        // remove from list
        LL_DELETE(*address, iter);

        // free element data
        interfaces_interface_ipv4_address_element_free(&iter);
    }
}

interfaces_interface_ipv4_address_element_t* interfaces_interface_ipv4_address_element_new(void)
{
    interfaces_interface_ipv4_address_element_t* new_element = NULL;

    new_element = malloc(sizeof(interfaces_interface_ipv4_address_element_t));
    new_element->address = (interfaces_interface_ipv4_address_t) { 0 };

    return new_element;
}

void interfaces_interface_ipv4_address_element_free(interfaces_interface_ipv4_address_element_t** el)
{
    if (*el) {
        // IP
        if ((*el)->address.ip) {
            free((*el)->address.ip);
        }

        // netmask if one exists
        if ((*el)->address.subnet_type != interfaces_interface_ipv4_address_subnet_none) {
            if ((*el)->address.subnet_type == interfaces_interface_ipv4_address_subnet_netmask && (*el)->address.subnet.netmask != NULL) {
                free((*el)->address.subnet.netmask);
            }
        }

        // address data
        free(*el);
        *el = NULL;
    }
}

int interfaces_interface_ipv4_address_element_set_ip(interfaces_interface_ipv4_address_element_t** el, const char* ip)
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

int interfaces_interface_ipv4_address_element_set_prefix_length(interfaces_interface_ipv4_address_element_t** el, uint8_t prefix_length)
{
    (*el)->address.subnet.prefix_length = prefix_length;
    (*el)->address.subnet_type = interfaces_interface_ipv4_address_subnet_prefix_length;

    return 0;
}

int interfaces_interface_ipv4_address_element_set_netmask(interfaces_interface_ipv4_address_element_t** el, const char* netmask)
{
    if ((*el)->address.subnet.netmask) {
        free((*el)->address.subnet.netmask);
        (*el)->address.subnet_type = interfaces_interface_ipv4_address_subnet_none;
    }

    if (netmask) {
        (*el)->address.subnet.netmask = strdup(netmask);

        // check for correctly set netmask
        if ((*el)->address.subnet.netmask != NULL) {
            (*el)->address.subnet_type = interfaces_interface_ipv4_address_subnet_netmask;
            return 0;
        } else {
            (*el)->address.subnet_type = interfaces_interface_ipv4_address_subnet_none;
            return 1;
        }
    }

    return 0;
}
