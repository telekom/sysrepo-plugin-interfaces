#ifndef INTERFACES_PLUGIN_API_INTERFACES_IPV4_LOAD_H
#define INTERFACES_PLUGIN_API_INTERFACES_IPV4_LOAD_H

#include "plugin/context.h"
#include "plugin/data/interfaces/interface.h"
#include "plugin/data/interfaces/interface/linked_list.h"
#include "plugin/types.h"

int interfaces_add_address_ipv4(interfaces_interface_ipv4_address_element_t **address, char *ip, char *netmask);

int interfaces_add_neighbor_ipv4(interfaces_interface_ipv4_neighbor_element_t** neighbor, char *dst_addr, char *ll_addr);

unsigned int interfaces_get_ipv4_mtu(struct rtnl_link* link, interfaces_interface_t* interface);

unsigned int interfaces_get_ipv4_enabled(interfaces_interface_t* interface);

unsigned int interfaces_get_ipv4_forwarding(interfaces_interface_t* interface);


#endif /* INTERFACES_PLUGIN_API_INTERFACES_IPV4_LOAD_H */

