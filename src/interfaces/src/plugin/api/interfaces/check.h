#ifndef INTERFACES_PLUGIN_API_INTERFACES_CHECK_H
#define INTERFACES_PLUGIN_API_INTERFACES_CHECK_H

#include "plugin/context.h"
#include <utarray.h>

#include <srpc.h>

srpc_check_status_t interfaces_check_interface(interfaces_ctx_t* ctx, const interfaces_interface_hash_element_t* if_hash);

#endif // INTERFACES_PLUGIN_API_INTERFACES_CHECK_H