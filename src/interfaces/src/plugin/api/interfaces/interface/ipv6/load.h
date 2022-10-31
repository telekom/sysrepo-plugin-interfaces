#ifndef INTERFACES_PLUGIN_API_INTERFACES_IPV6_LOAD_H
#define INTERFACES_PLUGIN_API_INTERFACES_IPV6_LOAD_H

#include "plugin/context.h"
#include "plugin/data/interfaces/interface.h"
#include "plugin/data/interfaces/interface/linked_list.h"
#include "plugin/types.h"

int interfaces_add_address_ipv6(interfaces_interface_ipv6_address_element_t **address, char *ip, char *netmask);

int interfaces_add_neighbor_ipv6(interfaces_interface_ipv6_neighbor_element_t** neighbor, char *dst_addr, char *ll_addr);

unsigned int interfaces_get_ipv6_mtu(struct rtnl_link* link, interfaces_interface_t* interface);

unsigned int interfaces_get_ipv6_enabled(interfaces_interface_t* interface);

unsigned int interfaces_get_ipv6_forwarding(interfaces_interface_t* interface);

#endif /* INTERFACES_PLUGIN_API_INTERFACES_IPV6_LOAD_H */

