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

#ifndef ROUTING_CONTROL_PLANE_PROTOCOL_H
#define ROUTING_CONTROL_PLANE_PROTOCOL_H

#include "route/list_hash.h"

struct control_plane_protocol {
	char *type;
	char *description;
	char name[32];
	int initialized;
};

void control_plane_protocol_init(struct control_plane_protocol *cpp);
void control_plane_protocol_free(struct control_plane_protocol *cpp);

#endif // ROUTING_CONTROL_PLANE_PROTOCOL_H
