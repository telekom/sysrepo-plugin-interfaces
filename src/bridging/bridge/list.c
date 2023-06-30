#include "list.h"
#include <utils/memory.h>

void bridge_list_init(bridge_list_element_t **head)
{
	*head = NULL;
}

void bridge_list_free(bridge_list_element_t **head)
{
	bridge_list_element_t *iter = NULL, *tmp = NULL;

	LL_FOREACH_SAFE(*head, iter, tmp)
	{
		bridge_free(&iter->bridge);
		FREE_SAFE(iter);
	}
}
