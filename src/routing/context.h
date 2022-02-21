#ifndef ROUTING_PLUGIN_CONTEXT_H
#define ROUTING_PLUGIN_CONTEXT_H

#include <sysrepo_types.h>

#include "route/list_hash.h"

struct routing_ctx {
	struct route_list_hash_element *ipv4_static_routes_head;
	struct route_list_hash_element *ipv6_static_routes_head;
	sr_session_ctx_t *startup_session;
};

#endif // ROUTING_PLUGIN_CONTEXT_H