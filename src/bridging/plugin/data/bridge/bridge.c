#include "bridge.h"
#include "plugin/data/component/list.h"
#include "memory.h"

#include <string.h>

void bridge_init(bridge_t* br)
{
    memset(br, 0, sizeof(*br));
}

void bridge_free(bridge_t* br)
{
    if (br->name) {
        FREE_SAFE(br->name);
    }

    if (br->type) {
        FREE_SAFE(br->type);
    }

    bridge_component_list_free(&br->component_list);

    bridge_init(br);
}
