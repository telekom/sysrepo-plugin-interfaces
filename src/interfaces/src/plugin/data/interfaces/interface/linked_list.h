#ifndef INTERFACES_PLUGIN_DATA_INTERFACES_INTERFACE_LIST_H
#define INTERFACES_PLUGIN_DATA_INTERFACES_INTERFACE_LIST_H

#include "plugin/types.h"

#include <utlist.h>
#include <string.h>

/*
    Linked list operations
*/

#define INTERFACES_INTERFACE_LINKED_LIST_NEW(ll_ptr)    \
    do                                                  \
    {                                                   \
        (ll_ptr) = NULL;                                \
    } while(0)                                          

#define INTERFACES_INTERFACE_LINKED_LIST_IPV4_FREE_ADDRESS(ll_ptr, elem_ptr, tmp_ptr)   \
    do                                                                                  \
    {                                                                                   \
        LL_FOREACH_SAFE(*ll_ptr, elem_ptr, tmp_ptr) {                                   \
            LL_DELETE(*ll_ptr, elem_ptr);                                               \
            if (elem_ptr) {                                                             \
                if (elem_ptr->address.ip != NULL) {                                     \
                    FREE_SAFE(elem_ptr->address.ip);                                    \
                }                                                                       \
                if (elem_ptr->address.subnet.netmask != NULL) {                         \
                    FREE_SAFE(elem_ptr->address.subnet.netmask);                        \
                }                                                                       \
                FREE_SAFE(elem_ptr);                                                    \
            }                                                                           \
        }                                                                               \
    }                                                                                   \
    while (0)

#define INTERFACES_INTERFACE_LINKED_LIST_IPV6_FREE_ADDRESS(ll_ptr, elem_ptr, tmp_ptr)   \
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

#define INTERFACES_INTERFACE_LINKED_LIST_FREE_NEIGHBOR(ll_ptr, elem_ptr, tmp_ptr)   \
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

#define INTERFACES_INTERFACE_LINKED_LIST_FREE(ll_ptr)                                                                         \
        _Generic((ll_ptr),                                                                                                    \
                 interfaces_interfaces_interface_ipv4_address_element_t**  : interfaces_interface_ipv4_address_list_free      \
                 interfaces_interfaces_interface_ipv4_neighbor_element_t** : interfaces_interface_ipv4_neighbor_list_free     \
                 interfaces_interfaces_interface_ipv6_address_element_t**  : interfaces_interface_ipv6_address_list_free      \
                 interfaces_interfaces_interface_ipv6_neighbor_element_t** : interfaces_interface_ipv6_neighbor_list_free     \
        )(ll_ptr)

/* prepend since ordering doesn't matter - O(1) */
#define INTERFACES_INTERFACE_LINKED_LIST_ADD_ELEMENT(ll_ptr, new_element_ptr)   \
    do                                                                          \
    {                                                                           \
        LL_PREPEND(ll_ptr, new_element_ptr);                                    \
    } while(0)                                                                  \

#define INTERFACES_INTERFACE_LINKED_LIST_GET_ELEMENT_STRING(ll_ptr, element_ptr, member, value)   \
    do                                                                                            \
    {                                                                                             \
        LL_FOREACH(ll_ptr, element_ptr) {                                                         \
            if (strcmp(element_ptr->member, value) == 0) {                                        \
                break;                                                                            \
            }                                                                                     \
        }                                                                                         \
    } while(0)                                                                         

#define INTERFACES_INTERFACE_LINKED_LIST_GET_ELEMENT_SCALAR(ll_ptr, element_ptr, member, value)   \
    do                                                                                            \
    {                                                                                             \
        LL_SEARCH_SCALAR(ll_ptr, element_ptr, member, value);                                     \
    } while(0)                                                                         

void interfaces_interface_ipv4_address_list_free(interfaces_interfaces_interface_ipv4_address_element_t **ll);

void interfaces_interface_ipv4_neighbor_list_free(interfaces_interfaces_interface_ipv4_neighbor_element_t **ll);

void interfaces_interface_ipv6_address_list_free(interfaces_interfaces_interface_ipv6_address_element_t **ll);

void interfaces_interface_ipv6_neighbor_list_free(interfaces_interfaces_interface_ipv6_neighbor_element_t **ll);

/*
    Element operations
*/

void *interfaces_interface_ipv4_address_list_element_new(void);
void *interfaces_interface_ipv4_neighbor_list_element_new(void);
void *interfaces_interface_ipv6_address_list_element_new(void);
void *interfaces_interface_ipv6_neighbor_list_element_new(void);

int interfaces_interface_ipv4_address_list_element_set_address(interfaces_interfaces_interface_ipv4_address_element_t **ll, interfaces_interfaces_interface_ipv4_address_t *address);
int interfaces_interface_ipv4_address_list_element_set_neighbor(interfaces_interfaces_interface_ipv4_neighbor_element_t **ll, interfaces_interfaces_interface_ipv4_neighbor_t *neighbor);

#endif // INTERFACES_PLUGIN_DATA_INTERFACES_INTERFACE_LIST_H

