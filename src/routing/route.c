/*
 * telekom / sysrepo-plugin-interfaces
 *
 * This program is made available under the terms of the
 * BSD 3-Clause license which is available at
 * https://opensource.org/licenses/BSD-3-Clause
 *
 * SPDX-FileCopyrightText: 2022 Deutsche Telekom AG
 * SPDX-FileContributor: Sartura Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "route.h"
#include "utils/memory.h"

void route_init(struct route *route)
{
	route->preference = 0;
	route->metadata.active = 0;
	route->metadata.source_protocol = NULL;
	route->metadata.last_updated = NULL;
	route->metadata.description = NULL;
	route_next_hop_init(&route->next_hop);
}

void route_set_preference(struct route *route, uint32_t pref)
{
	route->preference = pref;
}

void route_set_active(struct route *route, bool active)
{
	route->metadata.active = active;
}

void route_set_source_protocol(struct route *route, char *proto)
{
	if (proto) {
		route->metadata.source_protocol = xstrdup(proto);
	}
}

void route_set_last_updated(struct route *route, char *last_up)
{
	if (last_up) {
		route->metadata.source_protocol = xstrdup(last_up);
	}
}

void route_set_description(struct route *route, const char *description)
{
	if (description) {
		route->metadata.description = xstrdup(description);
	}
}

struct route route_clone(struct route *route)
{
	struct route out;

	route_init(&out);

	route_set_preference(&out, route->preference);
	route_set_active(&out, route->metadata.active);
	route_set_source_protocol(&out, route->metadata.source_protocol);
	route_set_last_updated(&out, route->metadata.last_updated);
	route_set_description(&out, route->metadata.description);
	out.next_hop = route_next_hop_clone(&route->next_hop);

	return out;
}

void route_free(struct route *route)
{
	if (route->metadata.source_protocol) {
		FREE_SAFE(route->metadata.source_protocol);
	}

	if (route->metadata.last_updated) {
		FREE_SAFE(route->metadata.last_updated);
	}

	if (route->metadata.description) {
		FREE_SAFE(route->metadata.description);
	}

	route_next_hop_free(&route->next_hop);
	route_init(route);
}
