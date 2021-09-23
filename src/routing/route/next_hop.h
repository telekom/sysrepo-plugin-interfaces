#ifndef ROUTING_ROUTE_NEXT_HOP_H
#define ROUTING_ROUTE_NEXT_HOP_H

enum route_next_hop_kind {
	route_next_hop_kind_none = 0,
	route_next_hop_kind_simple,
	route_next_hop_kind_special,
	route_next_hop_kind_list
};

// single next hop - interface index
struct route_next_hop_simple {
	struct nl_addr *addr;
	int ifindex;
};

// enum string value
struct route_next_hop_special {
	char *value;
};

// list of interface indexes
struct route_next_hop_list {
	struct route_next_hop_simple *list;
	int size;
};

union route_next_hop_value {
	struct route_next_hop_simple simple;
	struct route_next_hop_special special;
	struct route_next_hop_list list;
};

struct route_next_hop {
	enum route_next_hop_kind kind;
	union route_next_hop_value value;
};

void route_next_hop_init(struct route_next_hop *nh);
void route_next_hop_set_simple(struct route_next_hop *nh, int ifindex, struct nl_addr *gw);
void route_next_hop_set_special(struct route_next_hop *nh, char *value);
void route_next_hop_add_list(struct route_next_hop *nh, int ifindex, struct nl_addr *gw);
struct route_next_hop route_next_hop_clone(struct route_next_hop *nh);
void route_next_hop_free(struct route_next_hop *nh);

#endif // ROUTING_ROUTE_NEXT_HOP_H
