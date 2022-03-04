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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <net/if.h>

#include <sysrepo.h>

#include "utils/memory.h"
#include "rib.h"
#include "rib/list.h"

#include <utlist.h>

static int rib_list_element_cmp(struct rib_list_element *e1, struct rib_list_element *e2);

void rib_list_init(struct rib_list_element **head)
{
	*head = NULL;
}

void rib_list_add(struct rib_list_element **head, char *name, int af)
{
	struct rib *found = NULL;
	struct rib_list_element *new_rib = NULL;

	found = rib_list_get(head, name, af);
	if (!found) {
		new_rib = xmalloc(sizeof(*new_rib));
		new_rib->next = NULL;

		rib_init(&new_rib->rib);
		rib_set_name(&new_rib->rib, name);
		rib_set_address_family(&new_rib->rib, af);

		LL_APPEND(*head, new_rib);
	}
}

int rib_list_set_description(struct rib_list_element **head, char *name, int af, const char *desc)
{
	int error = 0;
	struct rib *rib = NULL;

	rib = rib_list_get(head, name, af);
	if (rib) {
		rib_set_description(rib, desc);
	} else {
		error = -1;
	}

	return error;
}

int rib_list_set_default(struct rib_list_element **head, char *name, int af, int def)
{
	int error = 0;
	struct rib *rib = NULL;

	rib = rib_list_get(head, name, af);
	if (rib) {
		rib_set_default(rib, def);
	} else {
		error = -1;
	}

	return error;
}

struct rib *rib_list_get(struct rib_list_element **head, char *name, int af)
{
	struct rib_list_element *found = NULL;
	struct rib_list_element find_element = {0};
	struct rib *rib_ptr = NULL;

	// setup find element
	rib_set_name(&find_element.rib, name);
	rib_set_address_family(&find_element.rib, af);

	LL_SEARCH(*head, found, &find_element, rib_list_element_cmp);
	if (found) {
		rib_ptr = &found->rib;
	}

	return rib_ptr;
}

void rib_list_free(struct rib_list_element **head)
{
	struct rib_list_element *iter = NULL, *tmp = NULL;

	LL_FOREACH_SAFE(*head, iter, tmp)
	{
		LL_DELETE(*head, iter);
		rib_free(&iter->rib);
		free(iter);
	}
}

static int rib_list_element_cmp(struct rib_list_element *e1, struct rib_list_element *e2)
{
	// return only eq/neq
	return !(strcmp(e1->rib.name, e2->rib.name) == 0 && e1->rib.address_family == e2->rib.address_family);
}