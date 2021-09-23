#include <netlink/addr.h>
#include <netlink/route/nexthop.h>

#include "route/next_hop.h"
#include "utils/memory.h"

void route_next_hop_init(struct route_next_hop *nh)
{
	nh->kind = route_next_hop_kind_none;
}

void route_next_hop_set_simple(struct route_next_hop *nh, int ifindex, struct nl_addr *gw)
{
	nh->kind = route_next_hop_kind_simple;
	nh->value.simple.ifindex = ifindex;
	if (gw) {
		nh->value.simple.addr = nl_addr_clone(gw);
	} else {
		nh->value.simple.addr = NULL;
	}
}

void route_next_hop_set_special(struct route_next_hop *nh, char *value)
{
	nh->kind = route_next_hop_kind_special;
	if (value) {
		nh->value.special.value = xstrdup(value);
	}
}

void route_next_hop_add_list(struct route_next_hop *nh, int ifindex, struct nl_addr *gw)
{
	int idx = 0;

	if (nh->kind == route_next_hop_kind_none) {
		// initialize the list
		nh->kind = route_next_hop_kind_list;
		nh->value.list.list = xmalloc(sizeof(int));
		idx = 0;
	} else {
		nh->value.list.list = xrealloc(nh->value.list.list, sizeof(int) * (unsigned long) (nh->value.list.size + 1));
		idx = nh->value.list.size;
	}
	nh->value.list.list[idx].ifindex = ifindex;
	if (gw) {
		nh->value.list.list[idx].addr = nl_addr_clone(gw);
	} else {
		nh->value.list.list[idx].addr = NULL;
	}
	++nh->value.list.size;
}

struct route_next_hop route_next_hop_clone(struct route_next_hop *nh)
{
	struct route_next_hop out = {0};

	switch (nh->kind) {
		case route_next_hop_kind_none:
			break;
		case route_next_hop_kind_simple:
			route_next_hop_set_simple(&out, nh->value.simple.ifindex, nh->value.simple.addr);
			break;
		case route_next_hop_kind_special:
			route_next_hop_set_special(&out, nh->value.special.value);
			break;
		case route_next_hop_kind_list:
			for (int i = 0; i < nh->value.list.size; i++) {
				route_next_hop_add_list(&out, nh->value.list.list[i].ifindex, nh->value.list.list[i].addr);
			}
			break;
	}

	return out;
}

void route_next_hop_free(struct route_next_hop *nh)
{
	switch (nh->kind) {
		case route_next_hop_kind_none:
			break;
		case route_next_hop_kind_simple:
			if (nh->value.simple.addr) {
				nl_addr_put(nh->value.simple.addr);
			}
			break;
		case route_next_hop_kind_special:
			if (nh->value.special.value != NULL) {
				FREE_SAFE(nh->value.special.value);
			}
			break;
		case route_next_hop_kind_list:
			if (nh->value.list.size > 0) {
				for (int i = 0; i < nh->value.list.size; i++) {
					if (nh->value.list.list[i].addr) {
						nl_addr_put(nh->value.list.list[i].addr);
					}
				}
				FREE_SAFE(nh->value.list.list);
			}
			break;
	}
	route_next_hop_init(nh);
}
