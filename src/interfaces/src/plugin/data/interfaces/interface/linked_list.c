#include "linked_list.h"
#include "utils/memory.h"

void *interfaces_interface_ipv4_address_list_element_new(void)
{
    interfaces_interfaces_interface_ipv4_address_element_t *new_element = NULL;

    new_element = xmalloc(sizeof(interfaces_interfaces_interface_ipv4_address_element_t));

    /* NULL all address fields */
    new_element->address = (interfaces_interfaces_interface_ipv4_address_t) {0};

    return new_element;
}

void *interfaces_interface_ipv4_neighbor_list_element_new(void)
{
    interfaces_interfaces_interface_ipv4_neighbor_element_t *new_element = NULL;

    new_element = xmalloc(sizeof(interfaces_interfaces_interface_ipv4_neighbor_element_t));

    /* NULL all address fields */
    new_element->neighbor = (interfaces_interfaces_interface_ipv4_neighbor_t) {0};

    return new_element;
}

void *interfaces_interface_ipv6_address_list_element_new(void)
{
    interfaces_interfaces_interface_ipv6_address_element_t *new_element = NULL;

    new_element = xmalloc(sizeof(interfaces_interfaces_interface_ipv6_address_element_t));

    /* NULL all address fields */
    new_element->address = (interfaces_interfaces_interface_ipv6_address_t) {0};

    return new_element;
}

void *interfaces_interface_ipv6_neighbor_list_element_new(void)
{
    interfaces_interfaces_interface_ipv6_neighbor_element_t *new_element = NULL;

    new_element = xmalloc(sizeof(interfaces_interfaces_interface_ipv6_neighbor_element_t));

    /* NULL all address fields */
    new_element->neighbor = (interfaces_interfaces_interface_ipv6_neighbor_t) {0};

    return new_element;
}

void interfaces_interface_ipv4_address_list_free(interfaces_interfaces_interface_ipv4_address_element_t **ll)
{
    interfaces_interfaces_interface_ipv4_address_element_t *elem, *tmp;

    INTERFACES_INTERFACE_IPV4_ADDRESS_LIST_FREE(ll, elem, tmp);
}

void interfaces_interface_ipv4_neighbor_list_free(interfaces_interfaces_interface_ipv4_neighbor_element_t **ll)
{
    interfaces_interfaces_interface_ipv4_neighbor_element_t *elem, *tmp;

    INTERFACES_INTERFACE_NEIGHBOR_LIST_FREE(ll, elem, tmp);
}

void interfaces_interface_ipv6_address_list_free(interfaces_interfaces_interface_ipv6_address_element_t **ll)
{
    interfaces_interfaces_interface_ipv6_address_element_t *elem, *tmp;

    INTERFACES_INTERFACE_IPV6_ADDRESS_LIST_FREE(ll, elem, tmp);
}

void interfaces_interface_ipv6_neighbor_list_free(interfaces_interfaces_interface_ipv6_neighbor_element_t **ll)
{
    interfaces_interfaces_interface_ipv6_neighbor_element_t *elem, *tmp; 
    INTERFACES_INTERFACE_NEIGHBOR_LIST_FREE(ll, elem, tmp);
}

int interfaces_interface_ipv4_address_list_element_set_address(interfaces_interfaces_interface_ipv4_address_element_t **ll, interfaces_interfaces_interface_ipv4_address_t *address)
{
     
    if ((*ll)->address.ip != NULL) {
        FREE_SAFE((*ll)->address.ip);
    }
    if ((*ll)->address.subnet.prefix_length != NULL) {
        FREE_SAFE((*ll)->address.subnet.prefix_length);
    }

    if (address != NULL) {
        (*ll)->address.subnet.prefix_length = address->subnet.prefix_length;
        /* deepcopy char * */
        if (address->ip != NULL) {
            (*ll)->address.ip = xstrdup(address->ip);
        }
        /* netmask is part of a union with prefix_length, doesn't have to be set */
        if (address->subnet.netmask != NULL) {
            (*ll)->address.subnet.netmask = xstrdup(address->subnet.netmask);
            return (*ll)->address.ip == NULL || (*ll)->address.subnet.netmask == NULL;
        }
        return (*ll)->address.ip == NULL;
    }

    return 0;
}

int interfaces_interface_ipv4_address_list_element_set_neighbor(interfaces_interfaces_interface_ipv4_neighbor_element_t **ll, interfaces_interfaces_interface_ipv4_neighbor_t *neighbor)
{
     
    if ((*ll)->neighbor.ip != NULL) {
        FREE_SAFE((*ll)->neighbor.ip);
    }
    if ((*ll)->neighbor.link_layer_address != NULL) {
        FREE_SAFE((*ll)->neighbor.link_layer_address);
    }

    if (neighbor != NULL) {
        if (neighbor->ip != NULL) {
            (*ll)->neighbor.ip = xstrdup(neighbor->ip);
        }
        if (neighbor->link_layer_address != NULL) {
            (*ll)->neighbor.link_layer_address = xstrdup(neighbor->link_layer_address);
        }

        return (*ll)->neighbor.ip == NULL || (*ll)->neighbor.link_layer_address == NULL;
    }

    return 0;
}

int interfaces_interface_ipv6_address_list_element_set_address(interfaces_interfaces_interface_ipv6_address_element_t **ll, interfaces_interfaces_interface_ipv6_address_t *address)
{
     
    if ((*ll)->address.ip != NULL) {
        FREE_SAFE((*ll)->address.ip);
    }
    if ((*ll)->address.prefix_length != NULL) {
        FREE_SAFE((*ll)->address.prefix_length);
    }

    if (address != NULL) {
        (*ll)->address.prefix_length = address->prefix_length;
        /* deepcopy char * */
        if (address->ip != NULL) {
            (*ll)->address.ip = xstrdup(address->ip);
        }
        return (*ll)->address.ip == NULL;
    }

    return 0;
}

int interfaces_interface_ipv6_address_list_element_set_neighbor(interfaces_interfaces_interface_ipv6_neighbor_element_t **ll, interfaces_interfaces_interface_ipv6_neighbor_t *neighbor)
{
     
    if ((*ll)->neighbor.ip != NULL) {
        FREE_SAFE((*ll)->neighbor.ip);
    }
    if ((*ll)->neighbor.link_layer_address != NULL) {
        FREE_SAFE((*ll)->neighbor.link_layer_address);
    }

    if (neighbor != NULL) {
        if (neighbor->ip != NULL) {
            (*ll)->neighbor.ip = xstrdup(neighbor->ip);
        }
        if (neighbor->link_layer_address != NULL) {
            (*ll)->neighbor.link_layer_address = xstrdup(neighbor->link_layer_address);
        }

        return (*ll)->neighbor.ip == NULL || (*ll)->neighbor.link_layer_address == NULL;
    }

    return 0;
}
