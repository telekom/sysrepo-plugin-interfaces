#ifndef ROUTING_ROUTE_LIST_H
#define ROUTING_ROUTE_LIST_H

#include "route.h"

struct route_list_element {
	struct route route;
	struct route_list_element *next;
};

void route_list_init(struct route_list_element **head);
bool route_list_is_empty(struct route_list_element **head);
void route_list_add(struct route_list_element **head, struct route *route);
void route_list_free(struct route_list_element **head);

#endif // ROUTING_ROUTE_LIST_H
