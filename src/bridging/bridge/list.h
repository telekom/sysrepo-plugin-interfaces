#ifndef BRIDGING_PLUGIN_BRIDGE_LIST_H
#define BRIDGING_PLUGIN_BRIDGE_LIST_H

#include <bridging/bridge.h>

typedef struct bridge_list_element_s bridge_list_element_t;

struct bridge_list_element_s {
	bridge_t bridge;
	bridge_list_element_t *next;
};

void bridge_list_init(bridge_list_element_t **head);
void bridge_list_free(bridge_list_element_t **head);

#endif // BRIDGING_PLUGIN_BRIDGE_LIST_H