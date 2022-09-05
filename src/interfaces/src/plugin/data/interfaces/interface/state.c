#include "state.h"
#include "src/uthash.h"

interfaces_interface_state_t* interfaces_interface_state_hash_new(void)
{
    return NULL;
}

int interfaces_interface_state_hash_add(interfaces_interface_state_t** state_hash, const char* name, const uint8_t state, const time_t time)
{
    interfaces_interface_state_t* new_state = malloc(sizeof(interfaces_interface_state_t));

    if (!new_state) {
        return -1;
    }

    new_state->name = strdup(name);
    if (!new_state->name) {
        return -2;
    }

    new_state->state = state;
    new_state->last_change = time;

    HASH_ADD_STR(*state_hash, name, new_state);

    return 0;
}

interfaces_interface_state_t* interfaces_interface_state_hash_get(const interfaces_interface_state_t* state_hash, const char* name)
{
    interfaces_interface_state_t* state = NULL;

    HASH_FIND_STR(state_hash, name, state);

    return state;
}

void interfaces_interface_state_hash_free(interfaces_interface_state_t** state_hash)
{
    interfaces_interface_state_t *current = NULL, *tmp = NULL;

    HASH_ITER(hh, *state_hash, current, tmp)
    {
        HASH_DEL(*state_hash, current);

        // free data
        free((char*)current->name);

        // free allocated struct
        free(current);
    }
}
