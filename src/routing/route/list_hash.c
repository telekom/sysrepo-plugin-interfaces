#include <sysrepo.h>

#include "netlink/addr.h"
#include "route/list.h"
#include "route/list_hash.h"
#include "utils/memory.h"

#include <utlist.h>

static int route_list_hash_element_cmp(struct route_list_hash_element *e1, struct route_list_hash_element *e2);

void route_list_hash_init(struct route_list_hash_element **head)
{
	*head = NULL;
}

void route_list_hash_add(struct route_list_hash_element **head, struct nl_addr *addr, struct route *route)
{
	struct route_list_element **routes_head = NULL;
	struct route_list_hash_element *new_hash = NULL;

	routes_head = route_list_hash_get(head, addr);
	if (routes_head) {
		route_list_add(routes_head, route);
	} else {
		// create new hash
		new_hash = xmalloc(sizeof(*new_hash));
		new_hash->prefix = nl_addr_clone(addr);
		new_hash->next = NULL;
		new_hash->routes_head = NULL;

		route_list_add(&new_hash->routes_head, route);
		LL_APPEND(*head, new_hash);
	}
}

struct route_list_element **route_list_hash_get(struct route_list_hash_element **head, struct nl_addr *addr)
{
	struct route_list_hash_element *found = NULL;
	struct route_list_hash_element find_element = {
		.prefix = addr,
	};

	LL_SEARCH(*head, found, &find_element, route_list_hash_element_cmp);
	if (found) {
		return &found->routes_head;
	}

	return NULL;
}

void route_list_hash_free(struct route_list_hash_element **head)
{
	struct route_list_hash_element *iter = NULL, *tmp = NULL;

	LL_FOREACH_SAFE(*head, iter, tmp)
	{
		LL_DELETE(*head, iter);
		nl_addr_put(iter->prefix);
		route_list_free(&iter->routes_head);
		free(iter);
	}
}

static int route_list_hash_element_cmp(struct route_list_hash_element *e1, struct route_list_hash_element *e2)
{
	return nl_addr_cmp(e1->prefix, e2->prefix);
}