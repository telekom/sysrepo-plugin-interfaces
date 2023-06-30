#ifndef BRIDGING_PLUGIN_BRIDGE_COMPONENT_H
#define BRIDGING_PLUGIN_BRIDGE_COMPONENT_H

// local
#include "mac_address.h"

// stdlib
#include <stdint.h>

typedef struct bridge_component_s bridge_component_t;

struct bridge_component_s {
	char *name;
	uint32_t id;
	char *type;
	mac_address_t address;
};

void bridge_component_init(bridge_component_t *c);
void bridge_component_free(bridge_component_t *c);

#endif // BRIDGING_PLUGIN_BRIDGE_COMPONENT_H