#include "list.h"
#include "memory.h"
#include "plugin/data/component/component.h"

#include <stdlib.h>

#include <uthash/include/utlist.h>

void bridge_component_list_init(bridge_component_list_element_t** head)
{
    *head = NULL;
}

void bridge_component_list_free(bridge_component_list_element_t** head)
{
    bridge_component_list_element_t *iter = NULL, *tmp = NULL;

    LL_FOREACH_SAFE(*head, iter, tmp)
    {
        bridge_component_free(&iter->component);
        FREE_SAFE(iter);
    }
}
