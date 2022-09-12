#include "hash.h"

interfaces_interface_hash_element_t* interfaces_interface_hash_new(void)
{
    return NULL;
}

int interfaces_interface_hash_add_element(interfaces_interface_hash_element_t** hash, interfaces_interface_hash_element_t* new_element)
{
    interfaces_interface_hash_element_t* found_element = NULL;

    HASH_FIND_STR(*hash, new_element->interface.name, found_element);

    // element already exists
    if (found_element != NULL) {
        return -1;
    }

    // element not found - add new element to the hash
    HASH_ADD_KEYPTR(hh, *hash, new_element->interface.name, sizeof(new_element->interface.name), new_element);

    return 0;
}

interfaces_interface_hash_element_t* interfaces_interface_hash_get_element(interfaces_interface_hash_element_t** hash, const char* name)
{
    interfaces_interface_hash_element_t* found_element = NULL;

    HASH_FIND_STR(*hash, name, found_element);

    return found_element;
}

void interfaces_interface_hash_free(interfaces_interface_hash_element_t** hash)
{
    interfaces_interface_hash_element_t *tmp = NULL, *element = NULL;

    HASH_ITER(hh, *hash, element, tmp)
    {
        HASH_DEL(*hash, element);
        interfaces_interface_hash_element_free(&element);
    }
}

interfaces_interface_hash_element_t* interfaces_interface_hash_element_new(void)
{
    interfaces_interface_hash_element_t* new_element = NULL;

    new_element = malloc(sizeof(interfaces_interface_hash_element_t));
    if (!new_element)
        return NULL;

    // NULL all fields
    new_element->interface = (interfaces_interfaces_interface_t) { 0 };

    return new_element;
}

int interfaces_interface_hash_element_set_name(interfaces_interface_hash_element_t** el, const char* name)
{
    if ((*el)->interface.name) {
        free((*el)->interface.name);
        (*el)->interface.name = NULL;
    }

    if (name) {
        (*el)->interface.name = strdup(name);
        return (*el)->interface.name == NULL;
    }

    return 0;
}

int interfaces_interface_hash_element_set_description(interfaces_interface_hash_element_t** el, const char* description)
{
    if ((*el)->interface.description) {
        free((*el)->interface.description);
        (*el)->interface.description = NULL;
    }

    if (description) {
        (*el)->interface.description = strdup(description);
        return (*el)->interface.description == NULL;
    }

    return 0;
}

int interfaces_interface_hash_element_set_type(interfaces_interface_hash_element_t** el, const char* type)
{
    if ((*el)->interface.type) {
        free((*el)->interface.type);
        (*el)->interface.type = NULL;
    }

    if (type) {
        (*el)->interface.type = strdup(type);
        return (*el)->interface.type == NULL;
    }

    return 0;
}

int interfaces_interface_hash_element_set_enabled(interfaces_interface_hash_element_t** el, uint8_t enabled)
{
    (*el)->interface.enabled = enabled;

    return 0;
}

int interfaces_interface_hash_element_set_link_up_down_trap_enable(interfaces_interface_hash_element_t** el, interfaces_interfaces_interface_link_up_down_trap_enable_t link_up_down_trap_enable)
{
    (*el)->interface.link_up_down_trap_enable = link_up_down_trap_enable;
    return 0;
}

int interfaces_interface_hash_element_set_carrier_delay(interfaces_interface_hash_element_t** el, interfaces_interfaces_interface_carrier_delay_t carrier_delay)
{
    (*el)->interface.carrier_delay = carrier_delay;
    return 0;
}

int interfaces_interface_hash_element_set_dampening(interfaces_interface_hash_element_t** el, interfaces_interfaces_interface_dampening_t dampening)
{
    (*el)->interface.dampening = dampening;
    return 0;
}

int interfaces_interface_hash_element_set_encapsulation(interfaces_interface_hash_element_t** el, interfaces_interfaces_interface_encapsulation_t encapsulation)
{
    (*el)->interface.encapsulation = encapsulation;
    return 0;
}

int interfaces_interface_hash_element_set_loopback(interfaces_interface_hash_element_t** el, const char* loopback)
{
    if ((*el)->interface.loopback) {
        free((*el)->interface.loopback);
        (*el)->interface.loopback = NULL;
    }

    if (loopback) {
        (*el)->interface.loopback = strdup(loopback);
        return (*el)->interface.loopback == NULL;
    }

    return 0;
}

int interfaces_interface_hash_element_set_max_frame_size(interfaces_interface_hash_element_t** el, uint32_t max_frame_size)
{
    (*el)->interface.max_frame_size = max_frame_size;
    return 0;
}

int interfaces_interface_hash_element_set_parent_interface(interfaces_interface_hash_element_t** el, const char* parent_interface)
{
    if ((*el)->interface.parent_interface) {
        free((*el)->interface.parent_interface);
        (*el)->interface.parent_interface = NULL;
    }

    if (parent_interface) {
        (*el)->interface.parent_interface = strdup(parent_interface);
        return (*el)->interface.parent_interface == NULL;
    }

    return 0;
}

void interfaces_interface_hash_element_free(interfaces_interface_hash_element_t** el)
{
    if (*el) {
        // name
        if ((*el)->interface.name) {
            free((*el)->interface.name);
        }

        // description
        if ((*el)->interface.description) {
            free((*el)->interface.description);
        }

        // type
        if ((*el)->interface.type) {
            free((*el)->interface.type);
        }

        // loopback
        if ((*el)->interface.loopback) {
            free((*el)->interface.loopback);
        }

        // parent-interface
        if ((*el)->interface.parent_interface) {
            free((*el)->interface.parent_interface);
        }

        // element data
        free(*el);
        *el = NULL;
    }
}
