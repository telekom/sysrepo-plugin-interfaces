#ifndef INTERFACES_PLUGIN_API_INTERFACES_INTERFACE_IPV6_NEIGHBOR_LOAD_H
#define INTERFACES_PLUGIN_API_INTERFACES_INTERFACE_IPV6_NEIGHBOR_LOAD_H

#include "netlink/route/neighbour.h"
#include "plugin/context.h"
#include "plugin/types.h"

int interfaces_interface_ipv6_neighbor_load_ip(interfaces_ctx_t* ctx, interfaces_interface_ipv6_neighbor_element_t** element, struct rtnl_neigh* neigh);
int interfaces_interface_ipv6_neighbor_load_link_layer_address(interfaces_ctx_t* ctx, interfaces_interface_ipv6_neighbor_element_t** element, struct rtnl_neigh* neigh);

#endif // INTERFACES_PLUGIN_API_INTERFACES_INTERFACE_IPV6_NEIGHBOR_LOAD_H
