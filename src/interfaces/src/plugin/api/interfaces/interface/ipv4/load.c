#include "load.h"


int interfaces_add_address_ipv4(interfaces_interface_ipv4_address_element_t **address, char *ip, char *netmask)
{
    int prefix_length = 0;
    int error = 0;

    interfaces_interface_ipv4_address_element_t* new_element = NULL;

    new_element = interfaces_interface_ipv4_address_element_new(); 

    interfaces_interface_ipv4_address_element_set_ip(&new_element, ip);
    SRPC_SAFE_CALL_ERR(error, interfaces_interface_ipv4_address_netmask2prefix(netmask, prefix_length), out);
    interfaces_interface_ipv4_address_element_set_subnet(&new_element, netmask, interfaces_interface_ipv4_address_subnet_prefix_length);
    interfaces_interface_ipv4_address_add_element(address, new_element);

out:
    return error;
}

int interfaces_add_neighbor_ipv4(interfaces_interface_ipv4_neighbor_element_t** neighbor, char *dst_addr, char *ll_addr)
{
    interfaces_interface_ipv4_neighbor_element_t* new_element = NULL;

    new_element = interfaces_interface_ipv4_neighbor_element_new(); 

    interfaces_interface_ipv4_neighbor_element_set_ip(&new_element, dst_addr);
    interfaces_interface_ipv4_neighbor_element_set_link_layer_address(&new_element, ll_addr);
    interfaces_interface_ipv4_neighbor_add_element(neighbor, new_element);

    return 0;
}

unsigned int interfaces_get_ipv4_mtu(struct rtnl_link* link, interfaces_interface_t* interface)
{
    unsigned int mtu = 0;

    mtu = rtnl_link_get_mtu(link);

    interface->ipv4.mtu = mtu;

    return 0;
}

unsigned int interfaces_get_ipv4_enabled(interfaces_interface_t* interface)
{
    const char *ipv4_base = "/proc/sys/net/ipv4/conf";

    /* TODO: figure out how to enable/disable ipv4                                      */
    /*		 since disable_ipv4 doesn't exist in /proc/sys/net/ipv6/conf/interface_name */

}

unsigned int interfaces_get_ipv4_forwarding(interfaces_interface_t* interface)
{
    int error = 0;
    int forwarding = 0;

    const char *ipv4_base = "/proc/sys/net/ipv4/conf";
    
    SRPC_SAFE_CALL_ERR(error, read_from_proc_file(ipv4_base, interface->name, "forwarding", &forwarding), out);

    interface->ipv4.forwarding = forwarding;

out:
    return error;
}
