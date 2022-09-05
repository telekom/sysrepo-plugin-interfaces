#ifndef IF_DATA_H
#define IF_DATA_H

#include <uthash.h>

#include "plugin/types.h"

void 
if_data_init(interfaces_interfaces_interface_element_t* interface);

void 
interfaces_data_ht_root_init(interface_ht_element_t **if_root);

interface_ht_element_t *
interfaces_data_ht_get_by_name(interface_ht_element_t *if_root, char *name);

void
interfaces_data_ht_set_name(interfaces_interfaces_interface_element_t *interface, char *name);

int
interfaces_data_ht_add(interface_ht_element_t *if_root, char *name);

int
interfaces_data_ht_set_description(interface_ht_element_t *if_root, char *name, char *description);

int
interfaces_data_ht_set_type(interface_ht_element_t *if_root, char *name, char *type);

void
interfaces_data_ht_if_free(interfaces_interfaces_interface_element_t *interface);

void
interfaces_data_ht_free(interface_ht_element_t *if_root);

#endif /* IF_DATA_H */
