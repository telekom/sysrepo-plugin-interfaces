#ifndef ROUTING_ROUTE_LIST_HASH_H
#define ROUTING_ROUTE_LIST_HASH_H

#include <netlink/addr.h>

#include "route.h"
#include "route/list.h"

// "hash" like struct - simpler implementation rather than adding one more linked list struct for real hash implementation
// struct maps lists of routes by the destionation prefix
struct route_list_hash {
	struct nl_addr **list_addr;
	struct route_list *list_route;
	int size;
};

void route_list_hash_init(struct route_list_hash *hash);
void route_list_hash_add(struct route_list_hash *hash, struct nl_addr *addr, struct route *route);
void route_list_hash_free(struct route_list_hash *hash);

#endif // ROUTING_ROUTE_LIST_HASH_H
