#include "route/list_hash.h"
#include "control_plane_protocol.h"
#include "utils/memory.h"

void control_plane_protocol_init(struct control_plane_protocol *cpp)
{
	cpp->type = NULL;
	cpp->description = NULL;
	memset(cpp->name, 0, sizeof(cpp->name));
	cpp->initialized = 0;
	// init static routes hashes
	route_list_hash_init(&cpp->ipv4_static.routes);
	route_list_hash_init(&cpp->ipv6_static.routes);
}

void control_plane_protocol_free(struct control_plane_protocol *cpp)
{
	if (cpp->type) {
		FREE_SAFE(cpp->type);
	}
	if (cpp->description) {
		FREE_SAFE(cpp->description);
	}
	route_list_hash_free(&cpp->ipv4_static.routes);
	route_list_hash_free(&cpp->ipv6_static.routes);
	control_plane_protocol_init(cpp);
}
