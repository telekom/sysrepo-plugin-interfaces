/*
 * telekom / sysrepo-plugin-interfaces
 *
 * This program is made available under the terms of the
 * BSD 3-Clause license which is available at
 * https://opensource.org/licenses/BSD-3-Clause
 *
 * SPDX-FileCopyrightText: 2021 Deutsche Telekom AG
 * SPDX-FileContributor: Sartura Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef ROUTING_RIB_H
#define ROUTING_RIB_H

#include "route/list_hash.h"
#include "rib/description_pair.h"

struct rib {
	char name[32];
	char description[ROUTING_RIB_DESCRIPTION_SIZE];
	int address_family;
	int default_rib;
	struct route_list_hash_element *routes_head;
};

void rib_init(struct rib *rib);
void rib_set_address_family(struct rib *rib, int af);
void rib_set_description(struct rib *rib, const char *desc);
void rib_set_default(struct rib *rib, int def);
void rib_set_name(struct rib *rib, char *buff);
void rib_free(struct rib *rib);

#endif // ROUTING_RIB_H
