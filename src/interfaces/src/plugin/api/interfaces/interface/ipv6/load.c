#include "load.h"
#include "netlink/route/link.h"
#include "plugin/api/interfaces/interface/ipv6/address/load.h"
#include "plugin/api/interfaces/interface/ipv6/neighbor/load.h"
#include "plugin/data/interfaces/interface.h"
#include "plugin/data/interfaces/interface/ipv6.h"

int interfaces_interface_ipv6_load_enabled(interfaces_ctx_t* ctx, interfaces_interface_ipv6_t* ipv6, struct rtnl_link* link)
{
    int error = 0;

    // enabled by default
    SRPC_SAFE_CALL_ERR(error, interfaces_interface_hash_element_ipv6_set_enabled(ipv6, 1), error_out);

    goto out;

error_out:
    error = -1;

out:

    return error;
}

int interfaces_interface_ipv6_load_forwarding(interfaces_ctx_t* ctx, interfaces_interface_ipv6_t* ipv6, struct rtnl_link* link)
{
    int error = 0;

    // TODO: implement

    return error;
}

int interfaces_interface_ipv6_load_mtu(interfaces_ctx_t* ctx, interfaces_interface_ipv6_t* ipv6, struct rtnl_link* link)
{
    int error = 0;

    const unsigned int mtu = rtnl_link_get_mtu(link);

    SRPC_SAFE_CALL_ERR(error, interfaces_interface_hash_element_ipv6_set_mtu(ipv6, (uint16_t)mtu), error_out);

    goto out;

error_out:
    error = -1;

out:

    return error;
}

int interfaces_interface_ipv6_load_address(interfaces_ctx_t* ctx, interfaces_interface_ipv6_t* ipv6, struct rtnl_link* link)
{
    int error = 0;
    interfaces_nl_ctx_t* nl_ctx = &ctx->startup_ctx.nl_ctx;
    struct rtnl_addr* addr_iter = NULL;

    // created element
    interfaces_interface_ipv6_address_element_t* new_element = NULL;
    uint8_t element_added = 0;

    // allocate address list
    ipv6->address = interfaces_interface_ipv6_address_new();

    // allocate cache
    SRPC_SAFE_CALL_ERR(error, rtnl_addr_alloc_cache(nl_ctx->socket, &nl_ctx->addr_cache), error_out);

    // get link iterator
    SRPC_SAFE_CALL_PTR(addr_iter, (struct rtnl_addr*)nl_cache_get_first(nl_ctx->addr_cache), error_out);

    // iterate links
    while (addr_iter) {
        // get all addresses from the link and extract info
        if (rtnl_addr_get_ifindex(addr_iter) == rtnl_link_get_ifindex(link) && rtnl_addr_get_family(addr_iter) == AF_INET6) {
            // create new element
            new_element = interfaces_interface_ipv6_address_element_new();
            element_added = 0;

            // load IP and prefix
            SRPC_SAFE_CALL_ERR(error, interfaces_interface_ipv6_address_load_ip(ctx, &new_element, addr_iter), error_out);
            SRPC_SAFE_CALL_ERR(error, interfaces_interface_ipv6_address_load_prefix_length(ctx, &new_element, addr_iter), error_out);

            // add element to the list
            SRPC_SAFE_CALL_ERR(error, interfaces_interface_ipv6_address_add_element(&ipv6->address, new_element), error_out);
            element_added = 1;
        }

        // iterate
        addr_iter = (struct rtnl_addr*)nl_cache_get_next((struct nl_object*)addr_iter);
    }

    goto out;

error_out:
    error = -1;

    // if interrupted see if any memory is left over from the allocated element - free the element
    if (!element_added) {
        interfaces_interface_ipv6_address_element_free(&new_element);
    }

out:

    return error;
}

int interfaces_interface_ipv6_load_neighbor(interfaces_ctx_t* ctx, interfaces_interface_ipv6_t* ipv6, struct rtnl_link* link)
{
    int error = 0;
    interfaces_nl_ctx_t* nl_ctx = &ctx->startup_ctx.nl_ctx;
    struct rtnl_neigh* neigh_iter = NULL;

    // created element
    interfaces_interface_ipv6_neighbor_element_t* new_element = NULL;
    uint8_t element_added = 0;

    // allocate address list
    ipv6->neighbor = interfaces_interface_ipv6_neighbor_new();

    // allocate cache
    SRPC_SAFE_CALL_ERR(error, rtnl_neigh_alloc_cache(nl_ctx->socket, &nl_ctx->neigh_cache), error_out);

    // get link iterator
    SRPC_SAFE_CALL_PTR(neigh_iter, (struct rtnl_neigh*)nl_cache_get_first(nl_ctx->neigh_cache), error_out);

    // iterate links
    while (neigh_iter) {
        // get all neighbors from the link and extract info
        if (rtnl_neigh_get_ifindex(neigh_iter) == rtnl_link_get_ifindex(link) && rtnl_neigh_get_family(neigh_iter) == AF_INET6) {
            // create new element
            new_element = interfaces_interface_ipv6_neighbor_element_new();
            element_added = 0;

            // load IP and prefix
            SRPC_SAFE_CALL_ERR(error, interfaces_interface_ipv6_neighbor_load_ip(ctx, &new_element, neigh_iter), error_out);
            SRPC_SAFE_CALL_ERR(error, interfaces_interface_ipv6_neighbor_load_link_layer_address(ctx, &new_element, neigh_iter), error_out);

            // add element to the list
            SRPC_SAFE_CALL_ERR(error, interfaces_interface_ipv6_neighbor_add_element(&ipv6->neighbor, new_element), error_out);
            element_added = 1;
        }

        // iterate
        neigh_iter = (struct rtnl_neigh*)nl_cache_get_next((struct nl_object*)neigh_iter);
    }

    goto out;

error_out:
    error = -1;

    // if interrupted see if any memory is left over from the allocated element - free the element
    if (!element_added) {
        interfaces_interface_ipv6_neighbor_element_free(&new_element);
    }

out:

    return error;
}
