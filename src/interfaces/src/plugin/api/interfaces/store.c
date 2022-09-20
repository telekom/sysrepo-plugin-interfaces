#include "store.h"
#include "utlist.h"
#include "uthash.h"
#include "types.h"
#include <sysrepo.h>
#include "sysrepo/xpath.h"
#include "plugin/common.h"
#include "plugin/context.h"
#include <errno.h>
#include "netlink/errno.h"
#include "netlink/route/link.h"

int interfaces_store_interface(interfaces_ctx_t* ctx, const interfaces_interface_hash_element_t* if_hash)
{
    int error = 0;
    interfaces_interface_hash_element_t *i = NULL, *tmp = NULL;
    struct nl_sock *sk = NULL;
    struct rtnl_link* new_link = NULL;

    // setup nl socket

    SRPC_SAFE_CALL_PTR(sk, nl_socket_alloc(), error_out);

    // connect
    SRPC_SAFE_CALL_ERR(error, nl_connect(sk, NETLINK_ROUTE), error_out);

    HASH_ITER (hh, if_hash, i, tmp) {
        // create a new link
        SRPC_SAFE_CALL_PTR(new_link, rtnl_link_alloc(), error_out);

        // setup link and add it to the system
        rtnl_link_set_name(new_link, i->interface.name);
        SRPC_SAFE_CALL_ERR(error, rtnl_link_set_type(new_link, i->interface.type), error_out);
        SRPC_SAFE_CALL_ERR(error, rtnl_link_add(sk, new_link, NLM_F_CREATE), error_out);

    }

error_out:
    if (error < 0) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "nl_geterror(): %d = %s", error, nl_geterror(error));
    }
    error = -1;

error:
    return error;
}
