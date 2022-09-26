#ifndef INTERFACES_PLUGIN_DATA_INTERFACES_INTERFACE_H
#define INTERFACES_PLUGIN_DATA_INTERFACES_INTERFACE_H

#include "plugin/types.h"
#include <libyang/libyang.h>

/*
    Hash table operations
*/

interfaces_interface_hash_element_t* interfaces_interface_hash_new(void);
void interfaces_interface_hash_print_debug(const interfaces_interface_hash_element_t* if_hash);
int interfaces_interface_hash_from_ly(interfaces_interface_hash_element_t** if_hash, const struct lyd_node* interface_list_node);
int interfaces_interface_hash_add_element(interfaces_interface_hash_element_t** hash, interfaces_interface_hash_element_t* new_element);
interfaces_interface_hash_element_t* interfaces_interface_hash_get_element(interfaces_interface_hash_element_t** hash, const char* name);
void interfaces_interface_hash_free(interfaces_interface_hash_element_t** hash);

/*
    Element operations
*/

interfaces_interface_hash_element_t* interfaces_interface_hash_element_new(void);
void interfaces_interface_hash_element_free(interfaces_interface_hash_element_t** el);
int interfaces_interface_hash_element_set_name(interfaces_interface_hash_element_t** el, const char* name);
int interfaces_interface_hash_element_set_description(interfaces_interface_hash_element_t** el, const char* description);
int interfaces_interface_hash_element_set_type(interfaces_interface_hash_element_t** el, const char* type);
int interfaces_interface_hash_element_set_enabled(interfaces_interface_hash_element_t** el, uint8_t enabled);
int interfaces_interface_hash_element_set_link_up_down_trap_enable(interfaces_interface_hash_element_t** el, interfaces_interface_link_up_down_trap_enable_t link_up_down_trap_enable);
int interfaces_interface_hash_element_set_carrier_delay(interfaces_interface_hash_element_t** el, interfaces_interface_carrier_delay_t carrier_delay);
int interfaces_interface_hash_element_set_dampening(interfaces_interface_hash_element_t** el, interfaces_interface_dampening_t dampening);
int interfaces_interface_hash_element_set_encapsulation(interfaces_interface_hash_element_t** el, interfaces_interface_encapsulation_t encapsulation);
int interfaces_interface_hash_element_set_loopback(interfaces_interface_hash_element_t** el, const char* loopback);
int interfaces_interface_hash_element_set_max_frame_size(interfaces_interface_hash_element_t** el, uint32_t max_frame_size);
int interfaces_interface_hash_element_set_parent_interface(interfaces_interface_hash_element_t** el, const char* parent_interface);

/*
    IPv4
*/

int interfaces_interface_hash_element_set_ipv4(interfaces_interface_hash_element_t** el, interfaces_interface_ipv4_t ipv4);
int interfaces_interface_hash_element_set_ipv4_enabled(interfaces_interface_hash_element_t** el, uint8_t enabled);
int interfaces_interface_hash_element_set_ipv4_forwarding(interfaces_interface_hash_element_t** el, uint8_t forwarding);
int interfaces_interface_hash_element_set_ipv4_mtu(interfaces_interface_hash_element_t** el, uint16_t mtu);
int interfaces_interface_hash_element_set_ipv4_address(interfaces_interface_hash_element_t** el, interfaces_interface_ipv4_address_element_t* head);

/*
    IPv6
*/

int interfaces_interface_hash_element_set_ipv6(interfaces_interface_hash_element_t** el, interfaces_interface_ipv6_t ipv6);
int interfaces_interface_hash_element_set_ipv6_enabled(interfaces_interface_hash_element_t** el, uint8_t enabled);
int interfaces_interface_hash_element_set_ipv6_forwarding(interfaces_interface_hash_element_t** el, uint8_t forwarding);
int interfaces_interface_hash_element_set_ipv6_mtu(interfaces_interface_hash_element_t** el, uint16_t mtu);

#endif // INTERFACES_PLUGIN_DATA_INTERFACES_INTERFACE_H