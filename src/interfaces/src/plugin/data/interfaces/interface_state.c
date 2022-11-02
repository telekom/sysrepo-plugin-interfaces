#include "interface_state.h"
#include "src/uthash.h"
#include <string.h>

interfaces_interface_state_hash_element_t* interfaces_interface_state_hash_new(void)
{
    return NULL;
}

int interfaces_interface_state_hash_add(interfaces_interface_state_hash_element_t** state_hash, interfaces_interface_state_hash_element_t* new_element)
{
    interfaces_interface_state_hash_element_t* found_element = NULL;

    HASH_FIND_STR(*state_hash, new_element->state.name, found_element);

    // element already exists
    if (found_element != NULL) {
        return -1;
    }

    // element not found - add new element to the hash
    HASH_ADD_KEYPTR(hh, *state_hash, new_element->state.name, strlen(new_element->state.name), new_element);

    return 0;
}

interfaces_interface_state_hash_element_t* interfaces_interface_state_hash_get(const interfaces_interface_state_hash_element_t* state_hash, const char* name)
{
    interfaces_interface_state_hash_element_t* state = NULL;

    HASH_FIND_STR(state_hash, name, state);

    return state;
}

void interfaces_interface_state_hash_free(interfaces_interface_state_hash_element_t** state_hash)
{
    interfaces_interface_state_hash_element_t *current = NULL, *tmp = NULL;

    HASH_ITER(hh, *state_hash, current, tmp)
    {
        HASH_DEL(*state_hash, current);

        interfaces_interface_state_hash_element_free(&current);
    }
}

interfaces_interface_state_hash_element_t* interfaces_interface_state_hash_element_new(void)
{
    interfaces_interface_state_hash_element_t* new_element = NULL;

    new_element = malloc(sizeof(interfaces_interface_state_hash_element_t));
    if (!new_element) {
        return NULL;
    }

    new_element->state = (interfaces_interface_state_t) { 0 };

    return new_element;
}

int interfaces_interface_state_hash_element_set_name(interfaces_interface_state_hash_element_t** el, const char* name)
{
    if ((*el)->state.name) {
        free((*el)->state.name);
        (*el)->state.name = NULL;
    }

    if (name) {
        (*el)->state.name = strdup(name);
        return (*el)->state.name == NULL;
    }

    return -1;
}

void interfaces_interface_state_hash_element_set_state(interfaces_interface_state_hash_element_t** el, const uint8_t state)
{
    (*el)->state.state = state;
}

void interfaces_interface_state_hash_element_set_last_change(interfaces_interface_state_hash_element_t** el, const time_t last_change)
{
    (*el)->state.last_change = last_change;
}

void interfaces_interface_state_hash_element_free(interfaces_interface_state_hash_element_t** el)
{
    if (*el) {
        free((*el)->state.name);

        free(*el);
        *el = NULL;
    }
}
