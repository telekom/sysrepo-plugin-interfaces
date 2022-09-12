#ifndef INTERFACES_PLUGIN_DATA_INTERFACES_INTERFACE_STATE_H
#define INTERFACES_PLUGIN_DATA_INTERFACES_INTERFACE_STATE_H

#include "plugin/types.h"

/*
    Hash table operations
*/

interfaces_interface_state_hash_element_t* interfaces_interface_state_hash_new(void);
int interfaces_interface_state_hash_add(interfaces_interface_state_hash_element_t** state_hash, interfaces_interface_state_hash_element_t* new_element);
interfaces_interface_state_hash_element_t* interfaces_interface_state_hash_get(const interfaces_interface_state_hash_element_t* state_hash, const char* name);
void interfaces_interface_state_hash_free(interfaces_interface_state_hash_element_t** state_hash);

/*
    Element operations
*/

interfaces_interface_state_hash_element_t* interfaces_interface_state_hash_element_new(void);
int interfaces_interface_state_hash_element_set_name(interfaces_interface_state_hash_element_t** el, const char* name);
void interfaces_interface_state_hash_element_set_state(interfaces_interface_state_hash_element_t** el, const uint8_t state);
void interfaces_interface_state_hash_element_set_last_change(interfaces_interface_state_hash_element_t** el, const time_t last_change);
void interfaces_interface_state_hash_element_free(interfaces_interface_state_hash_element_t** el);

#endif // INTERFACES_PLUGIN_DATA_INTERFACES_INTERFACE_STATE_H