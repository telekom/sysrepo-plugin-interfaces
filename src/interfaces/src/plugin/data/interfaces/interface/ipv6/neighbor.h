#ifndef INTERFACES_PLUGIN_DATA_INTERFACES_INTERFACE_IPV6_NEIGHBOR_H
#define INTERFACES_PLUGIN_DATA_INTERFACES_INTERFACE_IPV6_NEIGHBOR_H

#include "plugin/types.h"

#include <string.h>
#include <utlist.h>

/*
    Linked list operations.
*/

interfaces_interface_ipv6_neighbor_element_t* interfaces_interface_ipv6_neighbor_new(void);
int interfaces_interface_ipv6_neighbor_add_element(interfaces_interface_ipv6_neighbor_element_t** address, interfaces_interface_ipv6_neighbor_element_t* new_element);
void interfaces_interface_ipv6_neighbor_free(interfaces_interface_ipv6_neighbor_element_t** address);

/*
    Element operations.
*/

interfaces_interface_ipv6_neighbor_element_t* interfaces_interface_ipv6_neighbor_element_new(void);
void interfaces_interface_ipv6_neighbor_element_free(interfaces_interface_ipv6_neighbor_element_t** el);
int interfaces_interface_ipv6_neighbor_element_set_ip(interfaces_interface_ipv6_neighbor_element_t** el, const char* ip);
int interfaces_interface_ipv6_neighbor_element_set_link_layer_address(interfaces_interface_ipv6_neighbor_element_t** el, const char* link_layer_address);
int interfaces_interface_ipv6_neighbor_element_set(interfaces_interface_ipv6_neighbor_element_t** src, interfaces_interface_ipv6_neighbor_element_t **dst);

#endif // INTERFACES_PLUGIN_DATA_INTERFACES_INTERFACE_IPV6_NEIGHBOR_H
