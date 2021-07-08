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

#include "if_state.h"
#include "utils/memory.h"
#include <netlink/cache.h>
#include <netlink/socket.h>

void if_state_init(if_state_t *st)
{
	st->name = NULL;
	st->last_change = 0;
	st->state = 0;
}

void if_state_free(if_state_t *st)
{
	if (st->name != NULL) {
		FREE_SAFE(st->name);
	}
}

void if_state_list_init(if_state_list_t *ls)
{
	ls->data = NULL;
	ls->count = 0;
}

if_state_t *if_state_list_get(if_state_list_t *ls, uint idx)
{
	if (idx < ls->count) {
		return &ls->data[idx];
	}
	return NULL;
}

if_state_t *if_state_list_get_by_if_name(if_state_list_t *ls, char *name)
{
	for (uint i = 0; i < ls->count; i++) {
		if (strcmp(ls->data[i].name, name) == 0) {
			return &ls->data[i];
		}
	}
	return NULL;
}

void if_state_list_alloc(if_state_list_t *ls, uint count)
{
	ls->count = count;
	ls->data = (if_state_t *) malloc(sizeof(if_state_t) * count);
	for (uint i = 0; i < count; i++) {
		if_state_init(ls->data + i);
	}
}

void if_state_list_add(if_state_list_t *ls, uint8_t state, char *name)
{
	uint count = ++ls->count;
	ls->data = (if_state_t *) realloc(ls->data, sizeof(if_state_t) * count);

	ls->data[count-1].last_change = 0;

	size_t len = strlen(name);
	ls->data[count-1].name = xcalloc(len + 1, sizeof(char));
	strncpy(ls->data[count-1].name, name, len);
	ls->data[count-1].name[len] = '\0';

	ls->data[count-1].state = state;
}

void if_state_list_free(if_state_list_t *ls)
{
	if (ls->count) {
		for (uint i = 0; i < ls->count; i++) {
			if_state_free(ls->data + i);
		}
		FREE_SAFE(ls->data);
	}
}
