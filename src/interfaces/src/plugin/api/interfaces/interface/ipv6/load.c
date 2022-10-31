#include "load.h"
#include "netlink/route/link.h"
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
