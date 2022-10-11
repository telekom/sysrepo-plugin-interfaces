#include "address.h"
#include "plugin/types.h"
#include "srpc/common.h"
#include "src/utlist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

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

int interfaces_interface_netmask_to_prefix_length(const char *netmask)
{
        int error = 0;

        struct sockaddr_in sa = { 0 };
        struct sockaddr_in6 sa6 = { 0 };

        // IPv6 if a ':' is found
        if (strchr(netmask, ':')) {
                SRPC_SAFE_CALL(inet_pton(AF_INET6, netmask, &(sa6.sin6_addr)), error_out);

                // s6_addr is a uint8_t array of length 16, all the byte popcounts need to be summarized
                // avoid branching, use popcountll's 64 bits minimum
                uint64_t *s6_addr64 = (uint64_t *) sa6.sin6_addr.s6_addr;

                return __builtin_popcountll(s6_addr64[0]) + __builtin_popcountll(s6_addr64[1]);

        }

        // IPv4 otherwise
        SRPC_SAFE_CALL(inet_pton(AF_INET, netmask, &(sa.sin_addr)), error_out);

        return __builtin_popcountl(sa.sin_addr.s_addr);

error_out:
        return -1;
}

int interfaces_interface_ipv4_address_element_set_subnet(interfaces_interface_ipv4_address_element_t** el, char *netmask, enum interfaces_interface_ipv4_address_subnet subtype)
{
    int error = 0;
    int prefix_length = 0;

    switch (subtype) {
        case interfaces_interface_ipv4_address_subnet_netmask:
            interfaces_interface_ipv4_address_element_set_netmask(el, netmask);
            break;
        case interfaces_interface_ipv4_address_subnet_prefix_length:
            prefix_length = interfaces_interface_netmask_to_prefix_length(netmask);
            interfaces_interface_ipv4_address_element_set_prefix_length(el, prefix_length);
            break;
        case interfaces_interface_ipv4_address_subnet_none:
            break;
        default:
            error = -1;            
    }

    return error;
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
