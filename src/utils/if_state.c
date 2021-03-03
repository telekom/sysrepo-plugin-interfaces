#include "if_state.h"
#include "netlink/cache.h"
#include "netlink/socket.h"

void if_state_init(if_state_t *st)
{
	st->last_change = 0;
	st->if_idx = 0;
	st->state = 0;
}

void if_state_free(if_state_t *st)
{
	if_state_init(st);
}

void if_state_list_init(if_state_list_t *ls)
{
	ls->data = 0;
	ls->count = 0;
}

if_state_t *if_state_list_get(if_state_list_t *ls, uint idx)
{
	if (idx < ls->count) {
		return &ls->data[idx];
	}
	return NULL;
}

if_state_t *if_state_list_get_by_if_idx(if_state_list_t *ls, int if_idx)
{
	for (uint i = 0; i < ls->count; i++) {
		if (ls->data[i].if_idx == if_idx) {
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

void if_state_list_free(if_state_list_t *ls)
{
	if (ls->count) {
		for (uint i = 0; i < ls->count; i++) {
			if_state_free(ls->data + i);
		}
		free(ls->data);
		if_state_list_init(ls);
	}
}