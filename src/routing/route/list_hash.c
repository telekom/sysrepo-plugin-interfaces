#include <sysrepo.h>

#include "route/list_hash.h"
#include "utils/memory.h"

struct route_list *get_list_by_addr(struct route_list_hash *hash, struct nl_addr *addr);

void route_list_hash_init(struct route_list_hash *hash)
{
	hash->list_addr = NULL;
	hash->list_route = NULL;
	hash->size = 0;
}

void route_list_hash_add(struct route_list_hash *hash, struct nl_addr *addr, struct route *route)
{
	struct route_list *exists = NULL;

	exists = get_list_by_addr(hash, addr);
	if (exists != NULL) {
		route_list_add(exists, route);
	} else {
		hash->list_addr = xrealloc(hash->list_addr, sizeof(struct nl_addr *) * (unsigned long) (hash->size + 1));
		hash->list_route = xrealloc(hash->list_route, sizeof(struct route_list) * (unsigned long) (hash->size + 1));
		hash->list_addr[hash->size] = nl_addr_clone(addr);
		route_list_init(&hash->list_route[hash->size]);
		route_list_add(&hash->list_route[hash->size], route);
		hash->size += 1;
	}
}

void route_list_hash_free(struct route_list_hash *hash)
{
	if (hash->size) {
		for (int i = 0; i < hash->size; i++) {
			nl_addr_put(hash->list_addr[i]);
			route_list_free(&hash->list_route[i]);
		}
		FREE_SAFE(hash->list_addr);
		FREE_SAFE(hash->list_route);
	}
	route_list_hash_init(hash);
}

struct route_list *get_list_by_addr(struct route_list_hash *hash, struct nl_addr *addr)
{
	for (int i = 0; i < hash->size; i++) {
		if (nl_addr_cmp(addr, hash->list_addr[i]) == 0) {
			return &hash->list_route[i];
		}
	}
	return NULL;
}
