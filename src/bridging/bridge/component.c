#include "component.h"
#include <utils/memory.h>

#include <string.h>

void bridge_component_init(bridge_component_t *c)
{
	memset(c, 0, sizeof(*c));
}

void bridge_component_free(bridge_component_t *c)
{
	if (c->name) {
		FREE_SAFE(c->name);
	}

	if (c->type) {
		FREE_SAFE(c->type);
	}

	bridge_component_init(c);
}
