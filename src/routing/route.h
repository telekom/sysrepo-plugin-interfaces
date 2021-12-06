#ifndef ROUTING_ROUTE_H
#define ROUTING_ROUTE_H

#include <stdint.h>
#include <stdbool.h>
#include <netlink/addr.h>

#include "route/next_hop.h"

struct route_metadata {
	char *source_protocol;
	char *last_updated;
	char *description; // used only in control_plane_protocol struct
	bool active;
};

struct route {
	uint32_t preference;
	struct route_metadata metadata;
	struct route_next_hop next_hop;
};

void route_init(struct route *route);
void route_set_preference(struct route *route, uint32_t pref);
void route_set_active(struct route *route, bool active);
void route_set_source_protocol(struct route *route, char *proto);
void route_set_last_updated(struct route *route, char *last_up);
struct route route_clone(struct route *route);
void route_free(struct route *route);

#endif // ROUTING_ROUTE_H
