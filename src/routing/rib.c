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

#include <net/if.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sysrepo.h>

#include "route/list_hash.h"
#include "rib.h"
#include "utils/memory.h"

void rib_init(struct rib* rib)
{
    rib->address_family = AF_UNSPEC;
    rib->default_rib = 0;

    memset(rib->name, 0, sizeof(rib->name));
    memset(rib->description, 0, sizeof(rib->description));

    route_list_hash_init(&rib->routes_head);
}

void rib_set_address_family(struct rib* rib, int af)
{
    rib->address_family = af;
}

void rib_set_description(struct rib* rib, const char* desc)
{
    memcpy(rib->description, desc, strlen(desc) < ROUTING_RIB_DESCRIPTION_SIZE ? strlen(desc) : ROUTING_RIB_DESCRIPTION_SIZE);
}

void rib_set_default(struct rib* rib, int def)
{
    rib->default_rib = def;
}

void rib_set_name(struct rib* rib, char* buff)
{
    memcpy(rib->name, buff, strlen(buff));
}

void rib_free(struct rib* rib)
{
    route_list_hash_free(&rib->routes_head);
}
