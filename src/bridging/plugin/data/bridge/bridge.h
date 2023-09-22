#ifndef BRIDGING_PLUGIN_DATA_BRIDGE_H
#define BRIDGING_PLUGIN_DATA_BRIDGE_H

// local
#include "plugin/common.h"
#include "plugin/types.h"
#include "plugin/data/component/list.h"

// stdlib
#include <stdint.h>

// uthash
#include <uthash/include/utlist.h>

void bridge_init(bridge_t* br);
void bridge_free(bridge_t* br);

#endif // BRIDGING_PLUGIN_DATA_BRIDGE_H
