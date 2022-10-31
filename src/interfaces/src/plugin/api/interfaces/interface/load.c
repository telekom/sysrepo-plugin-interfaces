#include "load.h"
#include "plugin/common.h"
#include "plugin/data/interfaces/interface.h"
#include "sysrepo.h"

#include <linux/if.h>
#include <srpc.h>

int interfaces_interface_load_name(interfaces_ctx_t* ctx, interfaces_interface_hash_element_t** element, struct rtnl_link* link)
{
    int error = 0;

    const char* nl_if_name = NULL;

    nl_if_name = rtnl_link_get_name(link);

    // set element values
    SRPC_SAFE_CALL_ERR(error, interfaces_interface_hash_element_set_name(element, nl_if_name), error_out);

    goto out;

error_out:
    error = -1;

out:
    return error;
}

int interfaces_interface_load_type(interfaces_ctx_t* ctx, interfaces_interface_hash_element_t** element, struct rtnl_link* link)
{
    int error = 0;

    const char* nl_if_type = NULL;
    const char* ly_if_type = NULL;

    // interface type - nl version -> convert to libyang version
    nl_if_type = rtnl_link_get_type(link);

    SRPC_SAFE_CALL_ERR(error, interfaces_interface_type_nl2ly(nl_if_type, &ly_if_type), error_out);
    SRPC_SAFE_CALL_ERR(error, interfaces_interface_hash_element_set_type(element, ly_if_type), error_out);

    goto out;

error_out:
    error = -1;

out:

    return error;
}

int interfaces_interface_load_enabled(interfaces_ctx_t* ctx, interfaces_interface_hash_element_t** element, struct rtnl_link* link)
{
    int error = 0;

    uint8_t nl_enabled = (rtnl_link_get_operstate(link) == IF_OPER_UP || rtnl_link_get_operstate(link) == IF_OPER_UNKNOWN);

    SRPC_SAFE_CALL_ERR(error, interfaces_interface_hash_element_set_enabled(element, nl_enabled), error_out);

    goto out;

error_out:
    error = -1;

out:

    return error;
}
