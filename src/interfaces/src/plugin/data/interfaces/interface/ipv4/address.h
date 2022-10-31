#ifndef INTERFACES_PLUGIN_DATA_INTERFACES_INTERFACE_IPV4_ADDRESS_H
#define INTERFACES_PLUGIN_DATA_INTERFACES_INTERFACE_IPV4_ADDRESS_H

#include "plugin/types.h"

#include <string.h>
#include <utlist.h>

/*
    Linked list operations.
*/

interfaces_interface_ipv4_address_element_t* interfaces_interface_ipv4_address_new(void);
int interfaces_interface_ipv4_address_add_element(interfaces_interface_ipv4_address_element_t** address, interfaces_interface_ipv4_address_element_t* new_element);
void interfaces_interface_ipv4_address_free(interfaces_interface_ipv4_address_element_t** address);

/*
    Element operations.
*/

interfaces_interface_ipv4_address_element_t* interfaces_interface_ipv4_address_element_new(void);
void interfaces_interface_ipv4_address_element_free(interfaces_interface_ipv4_address_element_t** el);
int interfaces_interface_ipv4_address_element_set_ip(interfaces_interface_ipv4_address_element_t** el, const char* ip);
int interfaces_interface_ipv4_address_element_set_prefix_length(interfaces_interface_ipv4_address_element_t** el, uint8_t prefix_length);
int interfaces_interface_ipv4_address_element_set_netmask(interfaces_interface_ipv4_address_element_t** el, const char* netmask);
int interfaces_interface_ipv4_address_element_set(interfaces_interface_ipv4_address_element_t** src, interfaces_interface_ipv4_address_element_t **dst);

/*
    Helper functions
*/

int interfaces_interface_ipv4_address_netmask2prefix(const char* netmask, uint8_t* prefix_length);

#endif // INTERFACES_PLUGIN_DATA_INTERFACES_INTERFACE_IPV4_ADDRESS_H
