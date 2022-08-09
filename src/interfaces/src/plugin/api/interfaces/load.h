#ifndef INTERFACES_PLUGIN_API_INTERFACES_LOAD_H
#define INTERFACES_PLUGIN_API_INTERFACES_LOAD_H

#include "plugin/context.h"
#include <utarray.h>

int interfaces_load_interface(interfaces_ctx_t* ctx, UT_array** interface);

#endif // INTERFACES_PLUGIN_API_INTERFACES_LOAD_H