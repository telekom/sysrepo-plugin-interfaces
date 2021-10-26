#include <stdlib.h>

#include "route/list.h"
#include "route.h"
#include "utils/memory.h"

void route_list_init(struct route_list *ls)
{
	ls->list = NULL;
	ls->size = 0;
}

void route_list_add(struct route_list *ls, struct route *route)
{
	ls->list = xrealloc(ls->list, sizeof(struct route) * (unsigned long) (ls->size + 1));
	ls->list[ls->size] = route_clone(route);
	ls->size += 1;
}

struct route *route_list_get_last(struct route_list *ls)
{
	return &ls->list[ls->size - 1];
}

void route_list_free(struct route_list *ls)
{
	if (ls->list) {
		for (int i = 0; i < ls->size; i++) {
			route_free(&ls->list[i]);
		}
		FREE_SAFE(ls->list);
	}
	route_list_init(ls);
}
