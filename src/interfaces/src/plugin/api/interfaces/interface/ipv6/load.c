#include "load.h"

int interfaces_add_address_ipv6(interfaces_interface_ipv6_address_element_t **address, char *ip, char *netmask)
{
    int error = 0;
    uint8_t prefix_length = 0;

    interfaces_interface_ipv6_address_element_t* new_element = NULL;

    new_element = interfaces_interface_ipv6_address_element_new(); 

    interfaces_interface_ipv6_address_element_set_ip(&new_element, ip);
    SRPC_SAFE_CALL_ERR(error, interfaces_interface_ipv4_address_netmask2prefix(netmask, prefix_length), out);
    interfaces_interface_ipv6_address_element_set_prefix_length(&new_element, prefix_length);
    interfaces_interface_ipv6_address_add_element(address, new_element);

out:
    return error;
}

int interfaces_add_neighbor_ipv6(interfaces_interface_ipv6_neighbor_element_t** neighbor, char *dst_addr, char *ll_addr)
{
    interfaces_interface_ipv6_neighbor_element_t* new_element = NULL;

    new_element = interfaces_interface_ipv6_neighbor_element_new(); 

    interfaces_interface_ipv6_neighbor_element_set_ip(&new_element, dst_addr);
    interfaces_interface_ipv6_neighbor_element_set_link_layer_address(&new_element, ll_addr);
    interfaces_interface_ipv6_neighbor_add_element(neighbor, new_element);

    return 0;
}

unsigned int interfaces_get_ipv6_mtu(struct rtnl_link* link, interfaces_interface_t* interface)
{
    unsigned int mtu = 0;

    mtu = rtnl_link_get_mtu(link);

    interface->ipv6.mtu = mtu;

    return 0;
    
}

unsigned int interfaces_get_ipv6_enabled(interfaces_interface_t* interface)
{
    int error = 0;
    int enabled = 0;

    const char *ipv6_base = "/proc/sys/net/ipv6/conf";
    
    SRPC_SAFE_CALL_ERR(error, read_from_proc_file(ipv6_base, interface->name, "disable_ipv6", &enabled), out);

    interface->ipv6.enabled = enabled;

out:
    return error;
}

unsigned int interfaces_get_ipv6_forwarding(interfaces_interface_t* interface)
{
    int error = 0;
    int forwarding = 0;

    const char *ipv6_base = "/proc/sys/net/ipv6/conf";
    
    SRPC_SAFE_CALL_ERR(error, read_from_proc_file(ipv6_base, interface->name, "forwarding", &forwarding), out);

    interface->ipv6.forwarding = forwarding;

out:
    return error;
}

