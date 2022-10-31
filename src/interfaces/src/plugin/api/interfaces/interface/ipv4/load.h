#ifndef INTERFACES_PLUGIN_API_INTERFACES_INTERFACE_IPV4_LOAD_H
#define INTERFACES_PLUGIN_API_INTERFACES_INTERFACE_IPV4_LOAD_H

#include "plugin/context.h"
#include "plugin/data/interfaces/interface.h"
#include "plugin/data/interfaces/interface/linked_list.h"
#include "plugin/types.h"

int interfaces_interface_ipv4_load_enabled(interfaces_ctx_t* ctx, interfaces_interface_hash_element_t** element, struct rtnl_link* link);
int interfaces_interface_ipv4_load_forwarding(interfaces_ctx_t* ctx, interfaces_interface_hash_element_t** element, struct rtnl_link* link);
int interfaces_interface_ipv4_load_mtu(interfaces_ctx_t* ctx, interfaces_interface_hash_element_t** element, struct rtnl_link* link);

#endif // INTERFACES_PLUGIN_API_INTERFACES_INTERFACE_IPV4_LOAD_H
