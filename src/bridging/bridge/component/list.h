#ifndef BRIDGING_PLUGIN_BRIDGE_COMPONENT_LIST_H
#define BRIDGING_PLUGIN_BRIDGE_COMPONENT_LIST_H

#include <bridging/bridge/component.h>

typedef struct bridge_component_list_element_s bridge_component_list_element_t;

struct bridge_component_list_element_s {
	bridge_component_t component;
	bridge_component_list_element_t *next;
};

void bridge_component_list_init(bridge_component_list_element_t **head);
void bridge_component_list_free(bridge_component_list_element_t **head);

#endif // BRIDGING_PLUGIN_BRIDGE_COMPONENT_LIST_H