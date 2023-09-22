#ifndef BRIDGING_PLUGIN_DATA_COMPONENT_LIST_H
#define BRIDGING_PLUGIN_DATA_COMPONENT_LIST_H

#include "plugin/common.h"
#include "plugin/types.h"

void bridge_component_list_init(bridge_component_list_element_t **head);
void bridge_component_list_free(bridge_component_list_element_t **head);

#endif // BRIDGING_PLUGIN_DATA_COMPONENT_LIST_H
