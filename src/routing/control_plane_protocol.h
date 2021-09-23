#ifndef ROUTING_CONTROL_PLANE_PROTOCOL_H
#define ROUTING_CONTROL_PLANE_PROTOCOL_H

#include "route/list_hash.h"

struct static_routes_ipv4 {
	struct route_list_hash routes;
};

struct static_routes_ipv6 {
	struct route_list_hash routes;
};

struct control_plane_protocol {
	char *type;
	char *description;
	char name[32];
	int initialized;
	struct static_routes_ipv4 ipv4_static;
	struct static_routes_ipv6 ipv6_static;
};

void control_plane_protocol_init(struct control_plane_protocol *cpp);
void control_plane_protocol_free(struct control_plane_protocol *cpp);

#endif // ROUTING_CONTROL_PLANE_PROTOCOL_H
