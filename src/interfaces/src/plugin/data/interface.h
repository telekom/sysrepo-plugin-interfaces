#ifndef IF_DATA_H
#define IF_DATA_H

#include <uthash.h>

#include "plugin/types.h"

void interfaces_data_ht_root_init(interface_ht_element_t** if_root);

static void
interfaces_data_init(interfaces_interface_t* interface);

interface_ht_element_t*
interfaces_data_ht_get_by_name(interface_ht_element_t* if_root, char* name);

void interfaces_data_ht_set_name(interfaces_interface_t* interface, char* name);

int interfaces_data_ht_add(interface_ht_element_t* if_root, char* name);

int interfaces_data_ht_set_description(interface_ht_element_t* if_root, char* name, char* description);

int interfaces_data_ht_set_type(interface_ht_element_t* if_root, char* name, char* type);

int interfaces_data_ht_set_loopback(interface_ht_element_t* if_root, char* name, char* loopback);

int interfaces_data_ht_set_parent_interface(interface_ht_element_t* if_root, char* name, char* parent_interface);

void interfaces_data_ht_if_free(interfaces_interface_t* interface);

void interfaces_data_ht_free(interface_ht_element_t* if_root);

#endif /* IF_DATA_H */
