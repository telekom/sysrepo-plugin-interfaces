#ifndef INTERFACES_PLUGIN_DATA_INTERFACES_INTERFACE_IPV6_LIST_H
#define INTERFACES_PLUGIN_DATA_INTERFACES_INTERFACE_IPV6_LIST_H

#include "plugin/types.h"

#include <utlist.h>
#include <string.h>

/*
    Linked list operations
*/

#define INTERFACES_INTERFACE_IPV6_ADDRESS_LIST_FREE(ll_ptr, elem_ptr, tmp_ptr)   \
    do                                                                                  \
    {                                                                                   \
        LL_FOREACH_SAFE(*ll_ptr, elem_ptr, tmp_ptr) {                                   \
            LL_DELETE(*ll_ptr, elem_ptr);                                               \
            if (elem_ptr) {                                                             \
                if (elem_ptr->address.ip != NULL) {                                     \
                    FREE_SAFE(elem_ptr->address.ip);                                    \
                }                                                                       \
                FREE_SAFE(elem_ptr);                                                    \
            }                                                                           \
        }                                                                               \
    }                                                                                   \
    while (0)

#define INTERFACES_INTERFACE_IPV6_NEIGHBOR_LIST_FREE(ll_ptr, elem_ptr, tmp_ptr)   \
    do                                                                              \
    {                                                                               \
        LL_FOREACH_SAFE(*ll_ptr, elem_ptr, tmp_ptr) {                               \
            LL_DELETE(*ll_ptr, elem_ptr);                                           \
            if (elem_ptr) {                                                         \
                if (elem_ptr->neighbor.ip != NULL) {                                \
                    FREE_SAFE(elem_ptr->neighbor.ip);                               \
                }                                                                   \
                if (elem_ptr->neighbor.link_layer_address != NULL) {                \
                    FREE_SAFE(elem_ptr->neighbor.link_layer_address);               \
                }                                                                   \
                FREE_SAFE(elem_ptr);                                                \
            }                                                                       \
        }                                                                           \
    }                                                                               \
    while (0)

void interfaces_interface_ipv6_address_list_free(interfaces_interface_ipv6_address_element_t **ll);

void interfaces_interface_ipv6_neighbor_list_free(interfaces_interface_ipv6_neighbor_element_t **ll);

/*
    Element operations
*/

void *interfaces_interface_ipv6_address_list_element_new(void);
void *interfaces_interface_ipv6_neighbor_list_element_new(void);

int interfaces_interface_ipv6_address_list_element_set_address(interfaces_interface_ipv6_address_element_t **ll, interfaces_interface_ipv6_address_t *address);
int interfaces_interface_ipv6_address_list_element_set_neighbor(interfaces_interface_ipv6_neighbor_element_t **ll, interfaces_interface_ipv6_neighbor_t *neighbor);

#endif // INTERFACES_PLUGIN_DATA_INTERFACES_INTERFACE_IPV6_LIST_H

