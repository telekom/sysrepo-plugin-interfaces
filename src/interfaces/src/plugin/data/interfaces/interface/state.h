#ifndef INTERFACES_PLUGIN_DATA_INTERFACES_INTERFACE_STATE_H
#define INTERFACES_PLUGIN_DATA_INTERFACES_INTERFACE_STATE_H

#include "plugin/types.h"

interfaces_interface_state_t* interfaces_interface_state_hash_new(void);
int interfaces_interface_state_hash_add(interfaces_interface_state_t** state_hash, const char* name, const uint8_t state, const time_t time);
interfaces_interface_state_t* interfaces_interface_state_hash_get(const interfaces_interface_state_t* state_hash, const char* name);
void interfaces_interface_state_hash_free(interfaces_interface_state_t** state_hash);

#endif // INTERFACES_PLUGIN_DATA_INTERFACES_INTERFACE_STATE_H