#include <stdlib.h>

#include "route/list.h"
#include "route.h"
#include "utils/memory.h"

#include <utlist.h>

void route_list_init(struct route_list_element **head)
{
	*head = NULL;
}

bool route_list_is_empty(struct route_list_element **head)
{
	return *head == NULL;
}

void route_list_add(struct route_list_element **head, struct route *route)
{
	struct route_list_element *new_route = NULL;

	new_route = xmalloc(sizeof(*new_route));
	new_route->next = NULL;
	new_route->route = route_clone(route);

	LL_APPEND(*head, new_route);
}

void route_list_free(struct route_list_element **head)
{
	struct route_list_element *iter = NULL, *tmp = NULL;

	LL_FOREACH_SAFE(*head, iter, tmp)
	{
		LL_DELETE(*head, iter);
		route_free(&iter->route);
	}
}