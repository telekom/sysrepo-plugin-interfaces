#ifndef INTERFACES_PLUGIN_API_INTERFACES_INTERFACE_IPV6_ADDRESS_LOAD_H
#define INTERFACES_PLUGIN_API_INTERFACES_INTERFACE_IPV6_ADDRESS_LOAD_H

#include "netlink/route/addr.h"
#include "plugin/context.h"
#include "plugin/types.h"

int interfaces_interface_ipv6_address_load_ip(interfaces_ctx_t* ctx, interfaces_interface_ipv6_address_element_t** element, struct rtnl_addr* addr);
int interfaces_interface_ipv6_address_load_prefix_length(interfaces_ctx_t* ctx, interfaces_interface_ipv6_address_element_t** element, struct rtnl_addr* addr);

#endif // INTERFACES_PLUGIN_API_INTERFACES_INTERFACE_IPV6_ADDRESS_LOAD_H
