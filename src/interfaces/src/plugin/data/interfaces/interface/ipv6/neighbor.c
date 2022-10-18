#include "neighbor.h"
#include "src/utlist.h"

interfaces_interface_ipv6_neighbor_element_t* interfaces_interface_ipv6_neighbor_new(void)
{
    return NULL;
}

int interfaces_interface_ipv6_neighbor_add_element(interfaces_interface_ipv6_neighbor_element_t** address, interfaces_interface_ipv6_neighbor_element_t* new_element)
{
    LL_PREPEND(*address, new_element);
    return 0;
}

void interfaces_interface_ipv6_neighbor_free(interfaces_interface_ipv6_neighbor_element_t** address)
{
    interfaces_interface_ipv6_neighbor_element_t *iter = NULL, *tmp = NULL;

    LL_FOREACH_SAFE(*address, iter, tmp)
    {
        // remove from list
        LL_DELETE(*address, iter);

        // free element data
        interfaces_interface_ipv6_neighbor_element_free(&iter);
    }
}

interfaces_interface_ipv6_neighbor_element_t* interfaces_interface_ipv6_neighbor_element_new(void)
{
    interfaces_interface_ipv6_neighbor_element_t* new_element = NULL;

    new_element = malloc(sizeof(interfaces_interface_ipv6_neighbor_element_t));
    new_element->neighbor = (interfaces_interface_ipv6_neighbor_t) { 0 };

    return new_element;
}

void interfaces_interface_ipv6_neighbor_element_free(interfaces_interface_ipv6_neighbor_element_t** el)
{
    if (*el) {
        if ((*el)->neighbor.ip) {
            free((*el)->neighbor.ip);
        }

        if ((*el)->neighbor.link_layer_address) {
            free((*el)->neighbor.link_layer_address);
        }

        // address data
        free(*el);
        *el = NULL;
    }
}

int interfaces_interface_ipv6_neighbor_element_set_ip(interfaces_interface_ipv6_neighbor_element_t** el, const char* ip)
{
    if ((*el)->neighbor.ip) {
        free((*el)->neighbor.ip);
    }

    if (ip) {
        (*el)->neighbor.ip = strdup(ip);
        return (*el)->neighbor.ip == NULL;
    }

    return 0;
}

int interfaces_interface_ipv6_neighbor_element_set_link_layer_address(interfaces_interface_ipv6_neighbor_element_t** el, const char* link_layer_address)
{
    if ((*el)->neighbor.link_layer_address) {
        free((*el)->neighbor.link_layer_address);
    }

    if (link_layer_address) {
        (*el)->neighbor.link_layer_address = strdup(link_layer_address);
        return (*el)->neighbor.link_layer_address == NULL;
    }

    return 0;
}

/* set (deepcopy) an ipv6 neighbor list */
int interfaces_interface_ipv6_neighbor_element_set(interfaces_interface_ipv6_neighbor_element_t** src,
                                                   interfaces_interface_ipv6_neighbor_element_t **dst)
{
    interfaces_interface_ipv6_neighbor_element_t *src_iter = NULL;
    interfaces_interface_ipv6_neighbor_element_t *new_elem = NULL;

    LL_FOREACH(*src, src_iter) {
        new_elem = interfaces_interface_ipv6_neighbor_element_new();
        interfaces_interface_ipv6_neighbor_element_set_ip(&new_elem, src_iter->neighbor.ip);
        interfaces_interface_ipv6_neighbor_element_set_link_layer_address(&new_elem, 
                                                                          src_iter->neighbor.link_layer_address);

        interfaces_interface_ipv6_neighbor_add_element(dst, new_elem);
    }
    
    return 0;
}
