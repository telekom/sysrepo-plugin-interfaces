#include "load.h"
#include "interface/ipv4/load.h"
#include "interface/ipv6/load.h"
#include "plugin/common.h"
#include "plugin/data/interfaces/interface.h"
#include "plugin/types.h"
#include "read.h"
#include "utils/memory.h"
#include "utlist.h"

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

enum interfaces_load_exit_status {
    interfaces_load_failure = -1,
    interfaces_load_success = 0,
    interfaces_load_continue = 1,
};

/*
    Interface Data Loading (name, type, enabled etc.)
*/

static int interfaces_load_interface_info(struct rtnl_link* link, interfaces_interface_hash_element_t** new_element);

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
        // extract link info and add a new element to the interfaces hash

        // allocate new element
        SRPC_SAFE_CALL_PTR(new_element, interfaces_interface_hash_element_new(), error_out);

        // load interface data
        SRPC_SAFE_CALL_ERR(error, interfaces_load_interface_info(link_iter, &new_element), error_out);
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

static int interfaces_load_interface_info(struct rtnl_link* link, interfaces_interface_hash_element_t** new_element)
{
    int error = 0;

    const char* nl_if_name = NULL;
    const char* nl_if_type = NULL;
    const char* ly_if_type = NULL;
    uint8_t nl_enabled = 0;

    // 1. interface name
    nl_if_name = rtnl_link_get_name(link);

    // 2. interface type - nl version -> convert to libyang version
    nl_if_type = rtnl_link_get_type(link);
    SRPC_SAFE_CALL_ERR(error, interfaces_interface_type_nl2ly(nl_if_type, &ly_if_type), error_out);

    // enabled
    nl_enabled = (rtnl_link_get_operstate(link) == IF_OPER_UP || rtnl_link_get_operstate(link) == IF_OPER_UNKNOWN);

    // set element values
    SRPC_SAFE_CALL_ERR(error, interfaces_interface_hash_element_set_name(new_element, nl_if_name), error_out);
    SRPC_SAFE_CALL_ERR(error, interfaces_interface_hash_element_set_type(new_element, ly_if_type), error_out);
    SRPC_SAFE_CALL_ERR(error, interfaces_interface_hash_element_set_enabled(new_element, nl_enabled), error_out);

    goto out;

error_out:
    error = -1;

out:
    return error;
}