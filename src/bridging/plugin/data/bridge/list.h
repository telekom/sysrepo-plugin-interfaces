#ifndef BRIDGING_PLUGIN_DATA_LIST_H
#define BRIDGING_PLUGIN_DATA_LIST_H

#include "plugin/common.h"
#include "plugin/types.h"

void bridge_list_init(bridge_list_element_t** head);
void bridge_list_free(bridge_list_element_t** head);

#endif // BRIDGING_PLUGIN_DATA_LIST_H
