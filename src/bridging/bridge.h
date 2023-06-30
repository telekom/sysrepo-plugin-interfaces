#ifndef BRIDGING_PLUGIN_BRIDGE_H
#define BRIDGING_PLUGIN_BRIDGE_H

// local
#include "bridge/mac_address.h"
#include "bridge/component/list.h"

// stdlib
#include <stdint.h>

// uthash
#include <utlist.h>

typedef struct bridge_s bridge_t;

struct bridge_s {
	char *name;
	mac_address_t address;
	char *type;
	uint16_t ports;
	uint32_t up_time;
	uint32_t components;
	bridge_component_list_element_t *component_list;
};

void bridge_init(bridge_t *br);
void bridge_free(bridge_t *br);

#endif // BRIDGING_PLUGIN_BRIDGE_H