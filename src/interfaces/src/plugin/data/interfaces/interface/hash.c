#include "hash.h"
#include "libyang/tree_data.h"
#include "plugin/common.h"
#include "srpc/ly_tree.h"
#include "sysrepo.h"
#include "uthash.h"
#include "utlist.h"

#include <assert.h>
#include <srpc.h>
#include <stdio.h>
#include <stdlib.h>

interfaces_interface_hash_element_t* interfaces_interface_hash_new(void)
{
    return NULL;
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

void interfaces_interface_hash_print_debug(const interfaces_interface_hash_element_t* if_hash)
{
    const interfaces_interface_hash_element_t *iter = NULL, *tmp = NULL;
    interfaces_interface_ipv4_address_element_t* v4addr_iter = NULL;

    HASH_ITER(hh, if_hash, iter, tmp)
    {
        SRPLG_LOG_INF(PLUGIN_NAME, "Interface %s:", iter->interface.name);
        SRPLG_LOG_INF(PLUGIN_NAME, "\t Name = %s", iter->interface.name);
        SRPLG_LOG_INF(PLUGIN_NAME, "\t Type = %s", iter->interface.type);
        SRPLG_LOG_INF(PLUGIN_NAME, "\t Enabled = %d", iter->interface.enabled);
        SRPLG_LOG_INF(PLUGIN_NAME, "\t IPv4:");
        SRPLG_LOG_INF(PLUGIN_NAME, "\t IPv4:Enabled = %d", iter->interface.ipv4.enabled);
        SRPLG_LOG_INF(PLUGIN_NAME, "\t IPv4:Forwarding = %d", iter->interface.ipv4.forwarding);
        SRPLG_LOG_INF(PLUGIN_NAME, "\t IPv4:MTU = %hu", iter->interface.ipv4.mtu);
        SRPLG_LOG_INF(PLUGIN_NAME, "\t IPv4:Address List:");

        LL_FOREACH(iter->interface.ipv4.address, v4addr_iter)
        {
            SRPLG_LOG_INF(PLUGIN_NAME, "\t IPv4:Address %s:", v4addr_iter->address.ip);
            SRPLG_LOG_INF(PLUGIN_NAME, "\t IPv4:Address IP = %s", v4addr_iter->address.ip);
            SRPLG_LOG_INF(PLUGIN_NAME, "\t IPv4:Address Prefix Length = %d", v4addr_iter->address.subnet.prefix_length);
        }
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
    struct lyd_node *ipv4_container_node = NULL, *ipv6_container_node = NULL;
    struct lyd_node *ipv4_enabled_node = NULL, *ipv4_forwarding_node = NULL, *ipv4_mtu_node = NULL, *ipv4_address_node = NULL;
    struct lyd_node *ipv6_enabled_node = NULL, *ipv6_forwarding_node = NULL, *ipv6_mtu_node = NULL, *ipv6_address_node = NULL;

    // internal DS
    interfaces_interface_hash_element_t* new_element = NULL;
    interfaces_interface_ipv4_address_element_t* new_v4_element = NULL;
    interfaces_interface_ipv6_address_element_t* new_v6_element = NULL;

    while (if_iter) {
        // create new element
        new_element = interfaces_interface_hash_element_new();

        // get existing nodes
        if_name_node = srpc_ly_tree_get_child_leaf(if_iter, "name");
        if_type_node = srpc_ly_tree_get_child_leaf(if_iter, "type");
        if_enabled_node = srpc_ly_tree_get_child_leaf(if_iter, "enabled");
        ipv4_container_node = srpc_ly_tree_get_child_container(if_iter, "ipv4");
        ipv6_container_node = srpc_ly_tree_get_child_container(if_iter, "ipv6");

        // ipv4
        if (ipv4_container_node) {
            ipv4_enabled_node = srpc_ly_tree_get_child_leaf(ipv4_container_node, "enabled");
            ipv4_forwarding_node = srpc_ly_tree_get_child_leaf(ipv4_container_node, "forwarding");
            ipv4_mtu_node = srpc_ly_tree_get_child_leaf(ipv4_container_node, "mtu");
            ipv4_address_node = srpc_ly_tree_get_child_list(ipv4_container_node, "address");
        }

        // ipv6
        if (ipv6_container_node) {
            ipv6_enabled_node = srpc_ly_tree_get_child_leaf(ipv6_container_node, "enabled");
            ipv6_forwarding_node = srpc_ly_tree_get_child_leaf(ipv6_container_node, "forwarding");
            ipv6_mtu_node = srpc_ly_tree_get_child_leaf(ipv6_container_node, "mtu");
            ipv6_address_node = srpc_ly_tree_get_child_list(ipv6_container_node, "address");
        }

        // extract and set data
        if (if_name_node) {
            SRPC_SAFE_CALL_ERR(error, interfaces_interface_hash_element_set_name(&new_element, lyd_get_value(if_name_node)), error_out);
        }
        if (if_type_node) {
            SRPC_SAFE_CALL_ERR(error, interfaces_interface_hash_element_set_type(&new_element, lyd_get_value(if_type_node)), error_out);
        }
        if (if_enabled_node) {
            SRPC_SAFE_CALL_ERR(error, interfaces_interface_hash_element_set_enabled(&new_element, strcmp(lyd_get_value(if_enabled_node), "true") ? 1 : 0), error_out);
        }
        if (ipv4_enabled_node) {
            SRPC_SAFE_CALL_ERR(error, interfaces_interface_hash_element_set_ipv4_enabled(&new_element, strcmp(lyd_get_value(ipv4_enabled_node), "true") ? 1 : 0), error_out);
        }
        if (ipv4_forwarding_node) {
            SRPC_SAFE_CALL_ERR(error, interfaces_interface_hash_element_set_ipv4_forwarding(&new_element, strcmp(lyd_get_value(ipv4_forwarding_node), "true") ? 1 : 0), error_out);
        }
        if (ipv4_mtu_node) {
            const char* mtu_str = lyd_get_value(ipv4_mtu_node);
            uint16_t mtu = (uint16_t)atoi(mtu_str);

            SRPC_SAFE_CALL_ERR(error, interfaces_interface_hash_element_set_ipv4_mtu(&new_element, mtu), error_out);
        }

        // IPv4 address list
        while (ipv4_address_node) {
            ipv4_address_node = srpc_ly_tree_get_list_next(ipv4_address_node);
        }

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
    new_element->interface = (interfaces_interface_t) { 0 };

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

int interfaces_interface_hash_element_set_link_up_down_trap_enable(interfaces_interface_hash_element_t** el, interfaces_interface_link_up_down_trap_enable_t link_up_down_trap_enable)
{
    (*el)->interface.link_up_down_trap_enable = link_up_down_trap_enable;
    return 0;
}

int interfaces_interface_hash_element_set_carrier_delay(interfaces_interface_hash_element_t** el, interfaces_interface_carrier_delay_t carrier_delay)
{
    (*el)->interface.carrier_delay = carrier_delay;
    return 0;
}

int interfaces_interface_hash_element_set_dampening(interfaces_interface_hash_element_t** el, interfaces_interface_dampening_t dampening)
{
    (*el)->interface.dampening = dampening;
    return 0;
}

int interfaces_interface_hash_element_set_encapsulation(interfaces_interface_hash_element_t** el, interfaces_interface_encapsulation_t encapsulation)
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

int interfaces_interface_hash_element_set_ipv4(interfaces_interface_hash_element_t** el, interfaces_interface_ipv4_t ipv4)
{
    (*el)->interface.ipv4 = ipv4;

    return 0;
}

int interfaces_interface_hash_element_set_ipv4_enabled(interfaces_interface_hash_element_t** el, uint8_t enabled)
{
    (*el)->interface.ipv4.enabled = enabled;

    return 0;
}

int interfaces_interface_hash_element_set_ipv4_forwarding(interfaces_interface_hash_element_t** el, uint8_t forwarding)
{
    (*el)->interface.ipv4.forwarding = forwarding;

    return 0;
}

int interfaces_interface_hash_element_set_ipv4_mtu(interfaces_interface_hash_element_t** el, uint16_t mtu)
{
    (*el)->interface.ipv4.mtu = mtu;

    return 0;
}

int interfaces_interface_hash_element_set_ipv6(interfaces_interface_hash_element_t** el, interfaces_interface_ipv6_t ipv6)
{
    (*el)->interface.ipv6 = ipv6;

    return 0;
}

int interfaces_interface_hash_element_set_ipv6_enabled(interfaces_interface_hash_element_t** el, uint8_t enabled)
{
    (*el)->interface.ipv6.enabled = enabled;

    return 0;
}

int interfaces_interface_hash_element_set_ipv6_forwarding(interfaces_interface_hash_element_t** el, uint8_t forwarding)
{
    (*el)->interface.ipv6.forwarding = forwarding;

    return 0;
}

int interfaces_interface_hash_element_set_ipv6_mtu(interfaces_interface_hash_element_t** el, uint16_t mtu)
{
    (*el)->interface.ipv6.mtu = mtu;

    return 0;
}