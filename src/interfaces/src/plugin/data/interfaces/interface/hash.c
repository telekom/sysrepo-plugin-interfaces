#include "hash.h"
#include "libyang/tree_data.h"
#include "plugin/common.h"
#include "src/uthash.h"
#include "srpc/ly_tree.h"
#include "sysrepo.h"

#include <assert.h>
#include <srpc.h>

interfaces_interface_hash_element_t* interfaces_interface_hash_new(void)
{
    return NULL;
}

void interfaces_interface_hash_print_debug(const interfaces_interface_hash_element_t* if_hash)
{
    const interfaces_interface_hash_element_t *iter = NULL, *tmp = NULL;

    HASH_ITER(hh, if_hash, iter, tmp)
    {
        SRPLG_LOG_INF(PLUGIN_NAME, "Interface %s:", iter->interface.name);
        SRPLG_LOG_INF(PLUGIN_NAME, "\t Name: %s", iter->interface.name);
        SRPLG_LOG_INF(PLUGIN_NAME, "\t Type: %s", iter->interface.type);
        SRPLG_LOG_INF(PLUGIN_NAME, "\t Enabled: %d", iter->interface.enabled);
    }
}

int interfaces_interface_hash_from_ly(const struct lyd_node* interface_list_node, interfaces_interface_hash_element_t** if_hash)
{
    int error = 0;

    // make sure the hash is empty at the start
    assert(*if_hash == NULL);

    // libyang
    struct lyd_node* if_iter = (struct lyd_node*)interface_list_node;
    struct lyd_node *if_name_node = NULL, *if_type_node = NULL, *if_enabled_node = NULL;

    // extracted data
    const char *if_name = NULL, *if_type = NULL, *if_enabled = NULL;

    // internal DS
    interfaces_interface_hash_element_t* new_element = NULL;
    // interfaces_interfaces_interface_ipv4_t v4_config = { 0 };
    // interfaces_interfaces_interface_ipv4_t v6_config = { 0 };

    while (if_iter) {
        if_name_node = srpc_ly_tree_get_child_leaf(if_iter, "name");
        if_type_node = srpc_ly_tree_get_child_leaf(if_iter, "type");
        if_enabled_node = srpc_ly_tree_get_child_leaf(if_iter, "enabled");

        // extract data
        if (if_name_node) {
            if_name = lyd_get_value(if_name_node);
        }
        if (if_type_node) {
            if_type = lyd_get_value(if_type_node);
        }
        if (if_enabled_node) {
            if_enabled = lyd_get_value(if_enabled_node);
        }

        // create new element
        new_element = interfaces_interface_hash_element_new();

        // set data
        SRPC_SAFE_CALL_ERR(error, interfaces_interface_hash_element_set_name(&new_element, if_name), error_out);
        SRPC_SAFE_CALL_ERR(error, interfaces_interface_hash_element_set_type(&new_element, if_type), error_out);
        SRPC_SAFE_CALL_ERR(error, interfaces_interface_hash_element_set_enabled(&new_element, strcmp(if_enabled, "true") ? 1 : 0), error_out);

        // add element to the hash
        interfaces_interface_hash_add_element(if_hash, new_element);

        // set to NULL - free()
        new_element = NULL;

        // iterate next
        if_iter = srpc_ly_tree_get_list_next(if_iter);
    }

    goto out;
error_out:
    error = -1;

out:
    if (new_element) {
        interfaces_interface_hash_element_free(&new_element);
    }

    return error;
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
    HASH_ADD_KEYPTR(hh, *hash, new_element->interface.name, strlen(new_element->interface.name), new_element);

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

    *hash = NULL;
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

int interfaces_interface_hash_element_set_ipv4(interfaces_interface_hash_element_t** el, interfaces_interfaces_interface_ipv4_t ipv4)
{
    (*el)->interface.ipv4 = ipv4;

    return 0;
}

int interfaces_interface_hash_element_set_ipv6(interfaces_interface_hash_element_t** el, interfaces_interfaces_interface_ipv6_t ipv6)
{
    (*el)->interface.ipv6 = ipv6;

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
