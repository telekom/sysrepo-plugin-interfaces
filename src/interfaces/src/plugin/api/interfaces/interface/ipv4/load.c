#include "load.h"
#include "netlink/route/link.h"
#include "plugin/data/interfaces/interface.h"
#include "plugin/data/interfaces/interface/ipv4.h"

int interfaces_interface_ipv4_load_enabled(interfaces_ctx_t* ctx, interfaces_interface_ipv4_t* ipv4, struct rtnl_link* link)
{
    int error = 0;

    // enabled by default
    SRPC_SAFE_CALL_ERR(error, interfaces_interface_hash_element_ipv4_set_enabled(ipv4, 1), error_out);

    goto out;

error_out:
    error = -1;

out:

    return error;
}

int interfaces_interface_ipv4_load_forwarding(interfaces_ctx_t* ctx, interfaces_interface_ipv4_t* ipv4, struct rtnl_link* link)
{
    int error = 0;

    // TODO: implement

    return error;
}

int interfaces_interface_ipv4_load_mtu(interfaces_ctx_t* ctx, interfaces_interface_ipv4_t* ipv4, struct rtnl_link* link)
{
    int error = 0;

    const unsigned int mtu = rtnl_link_get_mtu(link);

    SRPC_SAFE_CALL_ERR(error, interfaces_interface_hash_element_ipv4_set_mtu(ipv4, (uint16_t)mtu), error_out);

    goto out;

error_out:
    error = -1;

out:

    return error;
}
