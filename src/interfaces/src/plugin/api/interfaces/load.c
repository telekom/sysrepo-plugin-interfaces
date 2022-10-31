#include "load.h"
#include "interface/ipv4/load.h"
#include "interface/ipv6/load.h"
#include "plugin/common.h"
#include "plugin/data/interfaces/interface.h"
#include "plugin/types.h"
#include "read.h"
#include "utils/memory.h"
#include "utlist.h"

// load APIs
#include "interface/load.h"

#include <errno.h>
#include <linux/if.h>
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

int interfaces_load_interface(interfaces_ctx_t* ctx, interfaces_interface_hash_element_t** if_hash)
{
    int error = 0;
    struct nl_sock* socket = NULL;
    struct nl_cache* link_cache = NULL;
    struct rtnl_link* link_iter = NULL;

    // temp data
    interfaces_interface_hash_element_t* new_element = NULL;

    // init hash
    *if_hash = interfaces_interface_hash_new();

    // socket + cache
    SRPC_SAFE_CALL_PTR(socket, nl_socket_alloc(), error_out);
    SRPC_SAFE_CALL_ERR(error, nl_connect(socket, NETLINK_ROUTE), error_out);
    SRPC_SAFE_CALL_ERR(error, rtnl_link_alloc_cache(socket, AF_UNSPEC, &link_cache), error_out);

    // get link iterator
    SRPC_SAFE_CALL_PTR(link_iter, (struct rtnl_link*)nl_cache_get_first(link_cache), error_out);

    // iterate links
    while (link_iter) {
        // allocate new element
        SRPC_SAFE_CALL_PTR(new_element, interfaces_interface_hash_element_new(), error_out);

        // load interface data
        SRPC_SAFE_CALL_ERR(error, interfaces_interface_load_name(ctx, &new_element, link_iter), error_out);
        SRPC_SAFE_CALL_ERR(error, interfaces_interface_load_type(ctx, &new_element, link_iter), error_out);
        SRPC_SAFE_CALL_ERR(error, interfaces_interface_load_enabled(ctx, &new_element, link_iter), error_out);

        // load interface IPv4 data
    }

    goto out;

error_out:
    error = -1;

out:
    if (socket != NULL) {
        nl_socket_free(socket);
    }

    if (link_cache != NULL) {
        nl_cache_free(link_cache);
    }

    return error;
}