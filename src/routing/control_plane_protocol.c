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

#include "control_plane_protocol.h"
#include "utils/memory.h"

#include <string.h>

void control_plane_protocol_init(struct control_plane_protocol *cpp)
{
	cpp->type = NULL;
	cpp->description = NULL;
	memset(cpp->name, 0, sizeof(cpp->name));
	cpp->initialized = 0;
}

void control_plane_protocol_free(struct control_plane_protocol *cpp)
{
	if (cpp->type) {
		FREE_SAFE(cpp->type);
	}
	if (cpp->description) {
		FREE_SAFE(cpp->description);
	}
	control_plane_protocol_init(cpp);
}
