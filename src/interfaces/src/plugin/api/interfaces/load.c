#include "load.h"
#include "read.h"
#include "plugin/common.h"
#include "utils/memory.h"
#include "utlist.h"

#include <errno.h>
#include <linux/limits.h>

#include <linux/if_arp.h>

#include <netlink/addr.h>
#include <netlink/cache.h>
#include <netlink/errno.h>
#include <netlink/netlink.h>
#include <netlink/route/addr.h>
#include <netlink/route/link.h>
#include <netlink/route/link/inet.h>
#include <netlink/route/link/inet6.h>
#include <netlink/route/link/vlan.h>
#include <netlink/route/neighbour.h>
#include <netlink/route/qdisc.h>
#include <netlink/route/tc.h>
#include <netlink/socket.h>

#include <sysrepo.h>

enum interfaces_load_exit_status {
    interfaces_load_failure  = -1,
    interfaces_load_success  =  0,
    interfaces_load_continue =  1,
};

static int interfaces_add_address_ipv4(interfaces_interface_ipv4_address_element_t **address, char *ip, char *netmask)
{
    interfaces_interface_ipv4_address_element_t* new_element = NULL;

    new_element = interfaces_interface_ipv4_address_element_new(); 

    interfaces_interface_ipv4_address_element_set_ip(&new_element, ip);
    interfaces_interface_ipv4_address_element_set_subnet(&new_element, netmask, interfaces_interface_ipv4_address_subnet_netmask);
    interfaces_interface_ipv4_address_add_element(address, new_element);

}

static int interfaces_add_address_ipv6(interfaces_interface_ipv6_address_element_t **address, char *ip, char *netmask)
{
    int prefix_length = 0;

    interfaces_interface_ipv6_address_element_t* new_element = NULL;

    new_element = interfaces_interface_ipv6_address_element_new(); 

    interfaces_interface_ipv6_address_element_set_ip(&new_element, ip);
    prefix_length = interfaces_interface_netmask_to_prefix_length(netmask);
    interfaces_interface_ipv6_address_element_set_prefix_length(&new_element, prefix_length);
    interfaces_interface_ipv6_address_add_element(address, new_element);

}

static int interfaces_add_ips(interfaces_interface_t* interface, char *ip, int netmask, int addr_family)
{
    switch (addr_family) {
        case AF_INET:
            interfaces_add_address_ipv4(&interface->ipv4.address, ip, netmask);
            break;
        case  AF_INET6:
            interfaces_add_address_ipv6(&interface->ipv6.address, ip, netmask);
            break;
        default:
            SRPLG_LOG_ERR(PLUGIN_NAME, "%s: invalid address family", __func__);
            return -1;
    }

    return 0;
}

static int interfaces_get_interface_ips(struct nl_sock* socket, struct rtnl_link* link, interfaces_interface_t* interface)
{
    struct nl_object *nl_object = NULL;
	struct nl_cache *addr_cache = NULL;
	struct nl_addr *nl_addr_local = NULL;
	struct rtnl_addr *addr = { 0 };
    char *address = NULL;
    char *subnet = NULL;
    char *addr_s = NULL;
    char *token = NULL;
    char *str = NULL;
	char addr_str[ADDR_STR_BUF_SIZE] = { 0 };

    int addr_count = 0;
    int addr_family = 0;
    int if_index = 0;
    int cur_if_index = 0;
    int error = 0;

    if_index = rtnl_link_get_ifindex(link);

    SRPC_SAFE_CALL(rtnl_neigh_alloc_cache(socket, &addr_cache), error_out);

    /* get ipv4 and ipv6 addresses */
    addr_count = nl_cache_nitems(addr_cache);

    nl_object = nl_cache_get_first(addr_cache);
    addr = (struct rtnl_addr *) nl_object;

    for (int i = 0; i < addr_count; ++i) {
        SRPC_SAFE_CALL_PTR(nl_addr_local, rtnl_addr_get_local(addr), error_out);
        
        cur_if_index = rtnl_addr_get_ifindex(addr);

        if (if_index != cur_if_index) {
            goto next_address;
        }

        SRPC_SAFE_CALL_PTR(addr_s, nl_addr2str(nl_addr_local, addr_str, sizeof(addr_str)), error_out);

        str = xstrdup(addr_s);
        SRPC_SAFE_CALL_PTR(token, strtok(str, "/"), error_out);
        
        address = xstrdup(token);

        /* get subnet */
        token = strtok(NULL, "/");
        if (token == NULL) {
            /*
               the address exists
               skip it
               we didn't add this address
               e.g.: ::1 
            */
            FREE_SAFE(str);
            FREE_SAFE(address);
            continue;
        }

        subnet = xstrdup(token);

        /* check if ipv4 or ipv6 */
		addr_family = rtnl_addr_get_family(addr);

        interfaces_add_ips(interface, address, subnet, addr_family);

    next_address:
        nl_object = nl_cache_get_next(nl_object);
        addr = (struct rtnl_addr *) nl_object;

        FREE_SAFE(subnet);
        FREE_SAFE(str);
        FREE_SAFE(address);
}

    goto out;
error_out:
out:
    if (addr_cache) {
        nl_cache_free(addr_cache);
    }   
    if (str) {
        FREE_SAFE(str);
    }   
    if (address) {
        FREE_SAFE(address);
    }   

    return interfaces_load_success;
}

static int interfaces_add_neighbor_ipv4(interfaces_interface_ipv4_neighbor_element_t** neighbor, char *dst_addr, char *ll_addr)
{
    interfaces_interface_ipv4_neighbor_element_t* new_element = NULL;

    new_element = interfaces_interface_ipv4_neighbor_element_new(); 

    interfaces_interface_ipv4_neighbor_element_set_ip(&new_element, dst_addr);
    interfaces_interface_ipv4_neighbor_element_set_link_layer_address(&new_element, ll_addr);
    interfaces_interface_ipv4_neighbor_add_element(neighbor, new_element);

    return 0;
}

static int interfaces_add_neighbor_ipv6(interfaces_interface_ipv6_neighbor_element_t** neighbor, char *dst_addr, char *ll_addr)
{
    interfaces_interface_ipv6_neighbor_element_t* new_element = NULL;

    new_element = interfaces_interface_ipv6_neighbor_element_new(); 

    interfaces_interface_ipv6_neighbor_element_set_ip(&new_element, dst_addr);
    interfaces_interface_ipv6_neighbor_element_set_link_layer_address(&new_element, ll_addr);
    interfaces_interface_ipv6_neighbor_add_element(neighbor, new_element);

    return 0;
}

static int interfaces_add_neighbor(interfaces_interface_t* interface, char *dst_addr, char *ll_addr, int addr_family)
{
    switch (addr_family) {
        case AF_INET:
            interfaces_add_neighbor_ipv4(&interface->ipv4.neighbor, dst_addr, ll_addr);
            break;
        case  AF_INET6:
            interfaces_add_neighbor_ipv6(&interface->ipv6.neighbor, dst_addr, ll_addr);
            break;
        default:
            SRPLG_LOG_ERR(PLUGIN_NAME, "%s: invalid address family", __func__);
            return -1;
    }

    return 0;
}

static int interfaces_get_interface_ip_neighbors(struct nl_sock* socket, struct rtnl_link* link, interfaces_interface_t* interface) 
{
    struct nl_cache *neigh_cache = NULL;
    struct nl_object *nl_neigh_object = NULL;
    struct nl_addr *nl_dst_addr = NULL;
    struct nl_addr *ll_addr = NULL;
	struct rtnl_neigh *neigh = NULL;

    int error = 0;
    int if_index = 0;
    int addr_family = 0;
    int neigh_state = 0;
    int neigh_count = 0;
    char *dst_addr = NULL;
    char *ll_addr_s = NULL;
    char dst_addr_str[ADDR_STR_BUF_SIZE] = { 0 };
	char ll_addr_str[ADDR_STR_BUF_SIZE] = { 0 };

    INTERFACES_INTERFACE_LIST_NEW(interface->ipv4.neighbor);
    INTERFACES_INTERFACE_LIST_NEW(interface->ipv6.neighbor);
    
    if_index = rtnl_link_get_ifindex(link);

    SRPC_SAFE_CALL(rtnl_neigh_alloc_cache(socket, &neigh_cache), error_out);

    neigh_count = nl_cache_nitems(neigh_cache);

    nl_neigh_object = nl_cache_get_first(neigh_cache);

    for (int i = 0; i < neigh_count; ++i) {
        nl_dst_addr = rtnl_neigh_get_dst((struct rtnl_neigh *) nl_neigh_object);

        SRPC_SAFE_CALL_PTR(dst_addr, nl_addr2str(nl_dst_addr, dst_addr_str, sizeof(dst_addr_str)), error_out);

        neigh = rtnl_neigh_get(neigh_cache, if_index, nl_dst_addr);

        if (neigh != NULL) {
				// get neigh state
				neigh_state = rtnl_neigh_get_state(neigh);

				// skip neighs with no arp state
				if (NUD_NOARP == neigh_state) {
					nl_neigh_object = nl_cache_get_next(nl_neigh_object);
					continue;
				}

				int cur_neigh_index = rtnl_neigh_get_ifindex(neigh);

				if (if_index != cur_neigh_index) {
					nl_neigh_object = nl_cache_get_next(nl_neigh_object);
					continue;
				}

				ll_addr = rtnl_neigh_get_lladdr(neigh);

				SRPC_SAFE_CALL_PTR(ll_addr_s, nl_addr2str(ll_addr, ll_addr_str, sizeof(ll_addr_str)), error_out);

				// check if ipv4 or ipv6
				addr_family = rtnl_neigh_get_family(neigh);

                // switch based on address family, add to the neighbor linked list
                SRPC_SAFE_CALL(interfaces_add_neighbor(interface, dst_addr, ll_addr_s, addr_family), error_out);
                
                rtnl_neigh_put(neigh);
        }
         
        nl_neigh_object = nl_cache_get_next(nl_neigh_object);
    }
    

    goto out;

error_out:
out:
    if (neigh_cache) {
        nl_cache_free(neigh_cache);
    }   
    if (neigh) {
        rtnl_neigh_put(neigh);
    }   

    return interfaces_load_success;
}

static unsigned int interfaces_get_ipv4_mtu(struct rtnl_link* link, interfaces_interface_t* interface)
{
    unsigned int mtu = 0;

    mtu = rtnl_link_get_mtu(link);

    interface->ipv4.mtu = mtu;

    return 0;
}

static unsigned int interfaces_get_ipv6_mtu(struct rtnl_link* link, interfaces_interface_t* interface)
{
    unsigned int mtu = 0;

    mtu = rtnl_link_get_mtu(link);

    interface->ipv6.mtu = mtu;

    return 0;
    
}

static unsigned int interfaces_get_interface_ip_mtu(struct rtnl_link* link, interfaces_interface_t* interface)
{
    interfaces_get_ipv4_mtu(link, interface);

    interfaces_get_ipv6_mtu(link, interface);
}

static unsigned int interfaces_get_ipv4_enabled(interfaces_interface_t* interface)
{
    const char *ipv4_base = "/proc/sys/net/ipv4/conf";

    /* TODO: figure out how to enable/disable ipv4                                      */
    /*		 since disable_ipv4 doesn't exist in /proc/sys/net/ipv6/conf/interface_name */

}

static unsigned int interfaces_get_ipv6_enabled(interfaces_interface_t* interface)
{
    int error = 0;
    int enabled = 0;

    const char *ipv6_base = "/proc/sys/net/ipv6/conf";
    
    SRPC_SAFE_CALL(read_from_proc_file(ipv6_base, interface->name, "disable_ipv6", &enabled), out);

    interface->ipv6.enabled = enabled;

out:
    return error;
}

static unsigned int interfaces_get_interface_ip_enabled(interfaces_interface_t* interface)
{
    interfaces_get_ipv4_enabled(interface);

    interfaces_get_ipv6_enabled(interface);
}

static unsigned int interfaces_get_ipv4_forwarding(interfaces_interface_t* interface)
{
    int error = 0;
    int forwarding = 0;

    const char *ipv4_base = "/proc/sys/net/ipv4/conf";
    
    SRPC_SAFE_CALL(read_from_proc_file(ipv4_base, interface->name, "forwarding", &forwarding), out);

    interface->ipv4.forwarding = forwarding;

out:
    return error;
}

static unsigned int interfaces_get_ipv6_forwarding(interfaces_interface_t* interface)
{
    int error = 0;
    int forwarding = 0;

    const char *ipv6_base = "/proc/sys/net/ipv6/conf";
    
    SRPC_SAFE_CALL(read_from_proc_file(ipv6_base, interface->name, "forwarding", &forwarding), out);

    interface->ipv6.forwarding = forwarding;

out:
    return error;
}


static unsigned int interfaces_get_interface_ip_forwarding(interfaces_interface_t* interface)
{
    interfaces_get_ipv4_forwarding(interface);

    interfaces_get_ipv6_forwarding(interface);
}

static char* interfaces_get_interface_name(struct rtnl_link* link)
{
    char* name = NULL;

    SRPC_SAFE_CALL_PTR(name, rtnl_link_get_name(link), error_out);

error_out:
    return xstrdup(name);
}

static char* interfaces_get_interface_description(interfaces_ctx_t* ctx, char* name)
{
    int error = SR_ERR_OK;
    char path_buffer[PATH_MAX] = { 0 };
    sr_val_t* val = { 0 };
    char* description = NULL;

    /* conjure description path for this interface: /ietf-interfaces:interfaces/interface[name='test_interface']/description */
    error = snprintf(path_buffer, sizeof(path_buffer) / sizeof(char), "%s[name=\"%s\"]/description", INTERFACES_INTERFACES_LIST_YANG_PATH, name);
    if (error < 0) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf error");
        goto error_out;
    }

    // get the interface description value
    error = sr_get_item(ctx->startup_session, path_buffer, 0, &val);
    if (error != SR_ERR_OK) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "sr_get_item error (%d): %s", error, sr_strerror(error));
        goto error_out;
    }

    if (strlen(val->data.string_val) > 0) {
        description = xstrdup(val->data.string_val);
    }

error_out:
    return description;
}

static char* interfaces_get_interface_type(struct rtnl_link* link, char* name)
{
    int error = 0;
    char* type = NULL;

    type = rtnl_link_get_type(link);
    if (type == NULL) {
        /* rtnl_link_get_type() will return NULL for interfaces that were not
         * set with rtnl_link_set_type()
         *
         * get the type from: /sys/class/net/<interface_name>/type
         */
        const char* path_to_sys = "/sys/class/net/";
        int type_id = 0;

        SRPC_SAFE_CALL(read_from_sys_file(path_to_sys, name, &type_id), error_out);

        switch (type_id) {
        case ARPHRD_ETHER:
            type = "eth";
            break;
        case ARPHRD_LOOPBACK:
            type = "lo";
            break;
        default:
            SRPLG_LOG_ERR(PLUGIN_NAME, "%s: unkown type_id: %d", __func__, type_id);
            return NULL;
        }
    }

error_out:
    return xstrdup(type);
}

static int interfaces_get_interface_enabled(struct rtnl_link* link, interfaces_interface_t *interface)
{
    uint8_t enabled = rtnl_link_get_operstate(link);

    /*
     * if the lo interface state is unknown, treat it as enabled
     * otherwise it will be set to down, and dns resolution won't work
     */
    if (IF_OPER_UP == enabled || IF_OPER_UNKNOWN == enabled) {
        enabled = interfaces_interface_enable_enabled;
    } else if (IF_OPER_DOWN == enabled) {
        enabled = interfaces_interface_enable_disabled;
    }

    interface->enabled = enabled;

    return 0;
}

static int interfaces_get_interface_parent_interface(struct nl_cache* cache, struct rtnl_link* link, interfaces_interface_t* interface)
{
    int parent_index = 0;
    char parent_buffer[IFNAMSIZ] = { 0 };
    char* parent_interface = NULL;

    if (rtnl_link_is_vlan(link)) {
        parent_index = rtnl_link_get_link(link);
        parent_interface = rtnl_link_i2name(cache, parent_index, parent_buffer, IFNAMSIZ);
        interface->parent_interface = xstrdup(parent_interface);

        return 0;
    }

    return -1;
}

/* TODO: outer tag, second id, tag - maybe refactor all to pass by reference, return error */
static int interfaces_get_interface_vlan_id(struct rtnl_link* link, interfaces_interface_t* interface)
{
    uint16_t* outer_vlan_id = &interface->encapsulation.dot1q_vlan.outer_tag.vlan_id;
    char* first = NULL;
    char* second = NULL;

    if (rtnl_link_is_vlan(link)) {
        *outer_vlan_id = (uint16_t)rtnl_link_vlan_get_id(link);
        if (*outer_vlan_id <= 0) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "%s: couldn't get vlan ID", __func__);
            return interfaces_load_failure;
        }

        /* check if vlan_id in name, if it is this is the QinQ interface, skip it */
        first = strchr(interface->name, '.');
        second = strchr(first + 1, '.');

        if (second != 0) {
            // continue to the next interface
            return interfaces_load_continue;
        }
    }

    return interfaces_load_success;
}

/*
    Parses the link for interface data
*/
static int interfaces_parse_link(interfaces_ctx_t* ctx, struct nl_sock* socket, struct nl_cache* cache, struct rtnl_link* link, interfaces_interface_t* interface)
{
    int error = interfaces_load_success;

    // required, fail if NULL
    SRPC_SAFE_CALL_PTR(interface->name, interfaces_get_interface_name(link), error_out);

    /* interfaces_get_interface_description(ctx, interface->name); */

    interfaces_get_interface_type(link, interface->name);

    interfaces_get_interface_parent_interface(cache, link, interface);

    /* interface can be skipped - interface_load_continue*/
    error = interfaces_get_interface_vlan_id(link, interface);
    if (error != interfaces_load_success) {
        goto out; // error_out would possibly change the error
    }

    interfaces_get_interface_enabled(link, interface);

    interfaces_get_interface_ips(socket, link, interface);

    interfaces_get_interface_ip_neighbors(socket, link, interface);

    interfaces_get_interface_ip_mtu(link, interface);

    //interfaces_get_interface_ip_enabled(interface);

    interfaces_get_interface_ip_forwarding(interface);

    goto out;
error_out:
    error = interfaces_load_failure;
out:
    /* do not free the data, the interface data needs to be added in the interfaces hash table */

    return error;
}

static int interfaces_add_link(interfaces_interface_hash_element_t** if_hash, interfaces_interface_t* interface)
{
    int error = 0;

    interfaces_interface_hash_element_t* new_if_hash_elem = interfaces_interface_hash_element_new();
    interfaces_interface_hash_element_set_name(&new_if_hash_elem, interface->name);

    SRPC_SAFE_CALL(interfaces_interface_hash_add_element(if_hash, new_if_hash_elem), error_out);

    if (interface->description != NULL) {
        SRPC_SAFE_CALL(interfaces_interface_hash_element_set_description(&new_if_hash_elem, interface->description), error_out);
    }

    if (interface->type != NULL) {
        SRPC_SAFE_CALL(interfaces_interface_hash_element_set_type(&new_if_hash_elem, interface->type), error_out);
    }

    if (interface->parent_interface != NULL) {
        SRPC_SAFE_CALL(interfaces_interface_hash_element_set_parent_interface(&new_if_hash_elem, interface->parent_interface), error_out);
    }

    interfaces_interface_hash_element_set_enabled(&new_if_hash_elem, interface->enabled);

    interfaces_interface_hash_element_set_ipv4(&new_if_hash_elem, interface->ipv4);
    interfaces_interface_hash_element_set_ipv4_mtu(&new_if_hash_elem, interface->ipv4.mtu);
    interfaces_interface_hash_element_set_ipv4_enabled(&new_if_hash_elem, interface->ipv4.enabled);
    interfaces_interface_hash_element_set_ipv4_forwarding(&new_if_hash_elem, interface->ipv4.forwarding);

    interfaces_interface_hash_element_set_ipv6(&new_if_hash_elem, interface->ipv6);
    interfaces_interface_hash_element_set_ipv6_mtu(&new_if_hash_elem, interface->ipv6.mtu);
    interfaces_interface_hash_element_set_ipv6_enabled(&new_if_hash_elem, interface->ipv6.enabled);
    interfaces_interface_hash_element_set_ipv6_forwarding(&new_if_hash_elem, interface->ipv6.forwarding);

    interfaces_interface_hash_element_set_enabled(&new_if_hash_elem, interface->enabled);

    goto out;
error_out:
out:
    if (interface->name != NULL) {
        FREE_SAFE(interface->name);
    }
    if (interface->description != NULL) {
        FREE_SAFE(interface->description);
    }
    if (interface->type != NULL) {
        FREE_SAFE(interface->type);
    }
    if (interface->parent_interface != NULL) {
        FREE_SAFE(interface->parent_interface);
    }

    return error;
}

static struct rtnl_link* interfaces_get_next_link(struct rtnl_link* link)
{
    return (struct rtnl_link*)nl_cache_get_next((struct nl_object*)link);
}

static int interfaces_interfaces_worker(interfaces_ctx_t* ctx, struct nl_sock* socket, struct nl_cache* cache, interfaces_interface_hash_element_t** if_hash)
{
    int error = 0;
    struct rtnl_link* link = NULL;
    interfaces_interface_t interface = { 0 };

    link = (struct rtnl_link*)nl_cache_get_first(cache);

    while (link != NULL) {
        error = interfaces_parse_link(ctx, socket, cache, link, &interface);
        switch (error) {
        case interfaces_load_success:
            SRPC_SAFE_CALL(interfaces_add_link(if_hash, &interface), error_out);
            break;
        case interfaces_load_continue:
            break;
        default:
            SRPLG_LOG_ERR(PLUGIN_NAME, "%s: error parsing link (%d)", __func__, error);
            goto error_out;
        }

        link = interfaces_get_next_link(link);
    }

    goto out;
error_out:
    error = -1;
out:
    if (link != NULL) {
        rtnl_link_put(link);
    }

    return error;
}

int interfaces_load_interface(interfaces_ctx_t* ctx, interfaces_interface_hash_element_t** if_hash)
{
    int error = 0;
    struct nl_sock* socket = NULL;
    struct nl_cache* cache = NULL;

    *if_hash = interfaces_interface_hash_new();

    socket = nl_socket_alloc();
    if (socket == NULL) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "nl_socket_alloc error: invalid socket");
        goto error_out;
    }

    SRPC_SAFE_CALL(nl_connect(socket, NETLINK_ROUTE), error_out);

    SRPC_SAFE_CALL(rtnl_link_alloc_cache(socket, AF_UNSPEC, &cache), error_out);

    SRPC_SAFE_CALL(interfaces_interfaces_worker(ctx, socket, cache, if_hash), error_out);

    goto out;
error_out:
    error = -1;
out:
    if (socket != NULL) {
        nl_socket_free(socket);
    }

    if (cache != NULL) {
        nl_cache_free(cache);
    }

    return error;
}
