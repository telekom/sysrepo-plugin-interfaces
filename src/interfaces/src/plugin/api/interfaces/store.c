#include "store.h"
#include "netlink/addr.h"
#include "netlink/cache.h"
#include "netlink/object.h"
#include "netlink/route/addr.h"
#include "netlink/socket.h"
#include "plugin/data/interfaces/interface/ipv4/address.h"
#include "src/utlist.h"
#include "srpc/common.h"
#include "utlist.h"
#include "uthash.h"
#include "plugin/types.h"
#include <sysrepo.h>
#include "plugin/common.h"
#include "plugin/context.h"
#include <errno.h>
#include "sysrepo/xpath.h"
#include "netlink/errno.h"
#include "netlink/route/link.h"
#include "/usr/include/linux/if.h"

int interfaces_store_interface(interfaces_ctx_t* ctx, const interfaces_interface_hash_element_t* if_hash)
{
    int error = 0;
    uint8_t if_status, prefix_length;
    uint32_t mtu;
    char *if_type = NULL;
    char *if_type_libnl = NULL;
    const interfaces_interface_hash_element_t *i = NULL, *tmp = NULL;
    interfaces_interface_ipv4_address_element_t *ipv4_iter = NULL;
    interfaces_interface_ipv6_address_element_t *ipv6_iter = NULL;
    struct nl_sock *sk = NULL;
    struct nl_addr* local_addr = NULL;
    struct nl_cache* link_cache = NULL;
    struct rtnl_link* new_link = NULL;
    struct rtnl_addr *link_ipv4_addr = NULL;
    struct rtnl_addr *link_ipv6_addr = NULL;

    // setup nl socket
    SRPC_SAFE_CALL_PTR(sk, nl_socket_alloc(), error_out);

    // connect
    SRPC_SAFE_CALL_ERR(error, nl_connect(sk, NETLINK_ROUTE), error_out);

    // allocate link cache
    SRPC_SAFE_CALL_ERR(error, rtnl_link_alloc_cache(sk, AF_UNSPEC, &link_cache), error_out);

    HASH_ITER (hh, if_hash, i, tmp) {
        if_type = i->interface.type;
        if_status = i->interface.enabled;
        mtu = i->interface.ipv4.mtu < i->interface.ipv6.mtu ? i->interface.ipv4.mtu : i->interface.ipv6.mtu;
        
        // create a new link
        SRPC_SAFE_CALL_PTR(new_link, rtnl_link_alloc(), error_out);
        
        //determine link type
        if (strcmp(if_type, "iana-if-type:ethernetCsmacd") == 0) {
                if_type_libnl = "veth";
                // !! ADDS 2 INTERFACES (one without ip addresses)
        }   else if (strcmp(if_type, "iana-if-type:softwareLoopback") == 0) {
                if_type_libnl = "vcan";
                //!! vcan INSTEAD OF lo
        }   else if (strcmp(if_type, "iana-if-type:l2vlan") == 0) {
                if_type_libnl = "vlan";
                //!! doesn't work
        }   else if (strcmp(if_type, "iana-if-type:other") == 0) {
                if_type_libnl = "dummy";
        }

        // set link type
        SRPC_SAFE_CALL_ERR(error, rtnl_link_set_type(new_link, if_type_libnl), error_out);

        // setup link name 
        rtnl_link_set_name(new_link, i->interface.name);

        // initialize desired link status
        if (if_status) {
            rtnl_link_set_flags(new_link, IFF_UP);
        } else {
            rtnl_link_unset_flags(new_link, IFF_UP);
        }
        rtnl_link_set_operstate(new_link, if_status ? IF_OPER_UP : IF_OPER_DOWN);

        // set MTU
        if (mtu) {
            rtnl_link_set_mtu(new_link, mtu);
        }

        // add link
        SRPC_SAFE_CALL_ERR(error, rtnl_link_add(sk, new_link, NLM_F_CREATE), error_out);

        // update link cache
        SRPC_SAFE_CALL_ERR(error, nl_cache_refill(sk, link_cache), error_out);

        // get updated new_link
        SRPC_SAFE_CALL_PTR(new_link, rtnl_link_get_by_name(link_cache, i->interface.name), error_out);

        LL_FOREACH (i->interface.ipv4.address, ipv4_iter) {
            link_ipv4_addr = rtnl_addr_alloc();

            //parse local ipv4 address
            SRPC_SAFE_CALL_ERR(error, nl_addr_parse(ipv4_iter->address.ip, AF_INET, &local_addr), error_out);

            // get ipv4 prefix length by using prefix-length or netmask
            switch (ipv4_iter->address.subnet_type) {
                case interfaces_interface_ipv4_address_subnet_none:
                    SRPLG_LOG_ERR(PLUGIN_NAME, "Unable to get prefix-length/netmask for address %s", ipv4_iter->address.ip);
                    break;
                case interfaces_interface_ipv4_address_subnet_prefix_length:
                    prefix_length = ipv4_iter->address.subnet.prefix_length;
                    break;
                case interfaces_interface_ipv4_address_subnet_netmask:
                    interfaces_interface_ipv4_address_netmask2prefix(ipv4_iter->address.subnet.netmask, &prefix_length);
                    break;
                default:
                    SRPLG_LOG_ERR(PLUGIN_NAME, "Unable to get prefix-length/netmask for address %s", ipv4_iter->address.ip);
            }
        
            // set ipv4 prefix length
            nl_addr_set_prefixlen(local_addr, prefix_length);

            //set to route address
            SRPC_SAFE_CALL_ERR(error, rtnl_addr_set_local(link_ipv4_addr, local_addr), error_out);

            // set interface
            rtnl_addr_set_ifindex(link_ipv4_addr, rtnl_link_get_ifindex(new_link));

            // add ipv4 address
            SRPC_SAFE_CALL_ERR(error, rtnl_addr_add(sk, link_ipv4_addr, 0), error_out);

            nl_addr_put(local_addr);
            rtnl_addr_put(link_ipv4_addr);
        }

        
        LL_FOREACH (i->interface.ipv6.address, ipv6_iter) {
            link_ipv6_addr = rtnl_addr_alloc();

            // parse local ipv6 address
            SRPC_SAFE_CALL_ERR(error, nl_addr_parse(ipv6_iter->address.ip, AF_INET6, &local_addr), error_out);

            // set ipv6 prefix length
            nl_addr_set_prefixlen(local_addr, ipv6_iter->address.prefix_length);

            //set to route address
            SRPC_SAFE_CALL_ERR(error, rtnl_addr_set_local(link_ipv6_addr, local_addr), error_out);

            // set interface
            rtnl_addr_set_ifindex(link_ipv6_addr, rtnl_link_get_ifindex(new_link));

            //add ipv6 address
            SRPC_SAFE_CALL_ERR(error, rtnl_addr_add(sk, link_ipv6_addr, 0), error_out);

            nl_addr_put(local_addr);
            rtnl_addr_put(link_ipv6_addr);
        }
        
    }
    
    goto out;

error_out:
    if (error < 0) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "nl_geterror(): %d = %s", error, nl_geterror(error));
    }
    error = -1;

out:
    nl_socket_free(sk);
    return error;
}
