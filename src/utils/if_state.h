#ifndef IF_STATE_H_ONCE
#define IF_STATE_H_ONCE

#include <stdint.h>
#include <time.h>

typedef struct if_state_s if_state_t;
typedef struct if_state_list_s if_state_list_t;
typedef unsigned int uint;

struct if_state_s {
	int if_idx;
	uint8_t state;
	time_t last_change;
};

void if_state_init(if_state_t *st);
void if_state_free(if_state_t *st);

struct if_state_list_s {
	if_state_t *data;
	uint count;
};

void if_state_list_init(if_state_list_t *ls);
if_state_t *if_state_list_get(if_state_list_t *ls, uint idx);
if_state_t *if_state_list_get_by_if_idx(if_state_list_t *ls, int if_idx);
void if_state_list_alloc(if_state_list_t *ls, uint count);
void if_state_list_free(if_state_list_t *ls);

#endif /* IF_STATE_H_ONCE */