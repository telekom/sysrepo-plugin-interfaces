#ifndef ROUTING_ROUTE_LIST_H
#define ROUTING_ROUTE_LIST_H

#include "route.h"

struct route_list {
	struct route *list;
	bool delete;
	size_t size;
};

void route_list_init(struct route_list *ls);
bool route_list_is_empty(struct route_list *ls);
void route_list_add(struct route_list *ls, struct route *route);
struct route *route_list_get_last(struct route_list *ls);
void route_list_free(struct route_list *ls);

#endif // ROUTING_ROUTE_LIST_H
