#include "interface.h"
#include "interface/ipv4.h"
#include "libyang/tree_data.h"
#include "plugin/common.h"
#include "srpc/ly_tree.h"
#include "sysrepo.h"
#include "uthash.h"
#include "utils/memory.h"
#include "utlist.h"

// other data API
#include "interface/ipv4/address.h"
#include "interface/ipv4/neighbor.h"
#include "interface/ipv6/address.h"
#include "interface/ipv6/neighbor.h"
#include "interface/linked_list.h"

#include <assert.h>
#include <srpc.h>
#include <stdio.h>
#include <stdlib.h>

/*
    Libyang conversion functions.
*/

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

        // lists

        if ((*el)->interface.ipv4.address) {
            INTERFACES_INTERFACE_LIST_FREE((*el)->interface.ipv4.address);
        }

        if ((*el)->interface.ipv6.address) {
            INTERFACES_INTERFACE_LIST_FREE((*el)->interface.ipv6.address);
        }

        if ((*el)->interface.ipv4.address) {
            INTERFACES_INTERFACE_LIST_FREE((*el)->interface.ipv4.neighbor);
        }

        if ((*el)->interface.ipv6.address) {
            INTERFACES_INTERFACE_LIST_FREE((*el)->interface.ipv6.neighbor);
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
    interfaces_interface_ipv6_address_element_t* v6addr_iter = NULL;

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

        LL_FOREACH(iter->interface.ipv6.address, v6addr_iter)
        {
            SRPLG_LOG_INF(PLUGIN_NAME, "\t IPv6:Address %s:", v6addr_iter->address.ip);
            SRPLG_LOG_INF(PLUGIN_NAME, "\t IPv6:Address IP = %s", v6addr_iter->address.ip);
            SRPLG_LOG_INF(PLUGIN_NAME, "\t IPv6:Address Prefix Length = %d", v6addr_iter->address.prefix_length);
        }
    }
}

int interfaces_interface_hash_from_ly(interfaces_interface_hash_element_t** if_hash, const struct lyd_node* interface_list_node)
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
    struct lyd_node *ipv4_ip_node = NULL, *ipv4_prefix_node = NULL, *ipv4_netmask_node = NULL;
    struct lyd_node *ipv6_ip_node = NULL, *ipv6_prefix_node = NULL;

    // internal DS
    interfaces_interface_hash_element_t* new_element = NULL;
    interfaces_interface_ipv4_address_element_t* new_v4_element = NULL;
    interfaces_interface_ipv6_address_element_t* new_v6_element = NULL;

    while (if_iter) {
        // create new element
        new_element = interfaces_interface_hash_element_new();

        // get existing nodes
        SRPC_SAFE_CALL_PTR(if_name_node, srpc_ly_tree_get_child_leaf(if_iter, "name"), error_out);
        SRPC_SAFE_CALL_PTR(if_type_node, srpc_ly_tree_get_child_leaf(if_iter, "type"), error_out);
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
            SRPC_SAFE_CALL_ERR(error, interfaces_interface_hash_element_set_enabled(&new_element, strcmp(lyd_get_value(if_enabled_node), "true") == 0 ? 1 : 0), error_out);
        }

        if (ipv4_enabled_node) {
            SRPC_SAFE_CALL_ERR(error, interfaces_interface_hash_element_ipv4_set_enabled(&new_element->interface.ipv4, strcmp(lyd_get_value(ipv4_enabled_node), "true") == 0 ? 1 : 0), error_out);
        }

        if (ipv4_forwarding_node) {
            SRPC_SAFE_CALL_ERR(error, interfaces_interface_hash_element_ipv4_set_forwarding(&new_element->interface.ipv4, strcmp(lyd_get_value(ipv4_forwarding_node), "true") == 0 ? 1 : 0), error_out);
        }

        if (ipv4_mtu_node) {
            const char* mtu_str = lyd_get_value(ipv4_mtu_node);
            uint16_t mtu = (uint16_t)atoi(mtu_str);

            SRPC_SAFE_CALL_ERR(error, interfaces_interface_hash_element_ipv4_set_mtu(&new_element->interface.ipv4, mtu), error_out);
        }

        // init every list

        // ipv4
        INTERFACES_INTERFACE_LIST_NEW(new_element->interface.ipv4.address);
        INTERFACES_INTERFACE_LIST_NEW(new_element->interface.ipv4.neighbor);

        // ipv6
        INTERFACES_INTERFACE_LIST_NEW(new_element->interface.ipv6.address);
        INTERFACES_INTERFACE_LIST_NEW(new_element->interface.ipv6.neighbor);

        // IPv4 address list
        while (ipv4_address_node) {
            // add new ipv4 address element
            new_v4_element = interfaces_interface_ipv4_address_element_new();

            // fetch address info nodes
            SRPC_SAFE_CALL_PTR(ipv4_ip_node, srpc_ly_tree_get_child_leaf(ipv4_address_node, "ip"), error_out);
            ipv4_prefix_node = srpc_ly_tree_get_child_leaf(ipv4_address_node, "prefix-length");
            ipv4_netmask_node = srpc_ly_tree_get_child_leaf(ipv4_address_node, "netmask");

            // set IP
            SRPC_SAFE_CALL_ERR(error, interfaces_interface_ipv4_address_element_set_ip(&new_v4_element, lyd_get_value(ipv4_ip_node)), error_out);

            // set subnet data
            if (ipv4_prefix_node) {
                const char* prefix_length_str = NULL;

                SRPC_SAFE_CALL_PTR(prefix_length_str, lyd_get_value(ipv4_prefix_node), error_out);
                const uint8_t prefix_length = (uint8_t)atoi(prefix_length_str);

                // set prefix length
                SRPC_SAFE_CALL_ERR(error, interfaces_interface_ipv4_address_element_set_prefix_length(&new_v4_element, prefix_length), error_out);
            } else if (ipv4_netmask_node) {
                const char* netmask_str = NULL;

                SRPC_SAFE_CALL_PTR(netmask_str, lyd_get_value(ipv4_netmask_node), error_out);

                // set netmask
                SRPC_SAFE_CALL_ERR(error, interfaces_interface_ipv4_address_element_set_netmask(&new_v4_element, netmask_str), error_out);
            } else {
                // should be impossible due to libyang's mandatory statement... but throw error
                SRPLG_LOG_ERR(PLUGIN_NAME, "Unable to determine subnet of the following address: %s", lyd_get_value(ipv4_ip_node));
                goto error_out;
            }

            // data set correctly - add address to the list
            INTERFACES_INTERFACE_LIST_ADD_ELEMENT(new_element->interface.ipv4.address, new_v4_element);

            // null new element - free()
            new_v4_element = NULL;

            // iterate
            ipv4_address_node = srpc_ly_tree_get_list_next(ipv4_address_node);
        }

        // IPv6 address list
        while (ipv6_address_node) {
            // add new ipv4 address element
            new_v6_element = interfaces_interface_ipv6_address_element_new();

            // fetch address info nodes
            SRPC_SAFE_CALL_PTR(ipv6_ip_node, srpc_ly_tree_get_child_leaf(ipv6_address_node, "ip"), error_out);
            SRPC_SAFE_CALL_PTR(ipv6_prefix_node, srpc_ly_tree_get_child_leaf(ipv6_address_node, "prefix-length"), error_out);

            // set IP
            SRPC_SAFE_CALL_ERR(error, interfaces_interface_ipv6_address_element_set_ip(&new_v6_element, lyd_get_value(ipv6_ip_node)), error_out);

            // set prefix-length
            const char* prefix_length_str = NULL;

            SRPC_SAFE_CALL_PTR(prefix_length_str, lyd_get_value(ipv6_prefix_node), error_out);

            const uint8_t prefix_length = (uint8_t)atoi(prefix_length_str);

            // set prefix length
            SRPC_SAFE_CALL_ERR(error, interfaces_interface_ipv6_address_element_set_prefix_length(&new_v6_element, prefix_length), error_out);

            // data set correctly - add address to the list
            INTERFACES_INTERFACE_LIST_ADD_ELEMENT(new_element->interface.ipv6.address, new_v6_element);

            // null new element - free()
            new_v6_element = NULL;

            // iterate
            ipv6_address_node = srpc_ly_tree_get_list_next(ipv6_address_node);
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

    if (new_v4_element) {
        interfaces_interface_ipv4_address_element_free(&new_v4_element);
    }

    if (new_v6_element) {
        interfaces_interface_ipv6_address_element_free(&new_v6_element);
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

    new_element = xmalloc(sizeof(interfaces_interface_hash_element_t));
    if (!new_element)
        return NULL;

    // NULL all fields
    new_element->interface = (interfaces_interface_t) { 0 };

    return new_element;
}

int interfaces_interface_hash_element_set_name(interfaces_interface_hash_element_t** el, const char* name)
{
    if ((*el)->interface.name) {
        FREE_SAFE((*el)->interface.name);
    }

    if (name) {
        (*el)->interface.name = xstrdup(name);
        return (*el)->interface.name == NULL;
    }

    return 0;
}

int interfaces_interface_hash_element_set_description(interfaces_interface_hash_element_t** el, const char* description)
{
    if ((*el)->interface.description) {
        FREE_SAFE((*el)->interface.description);
    }

    if (description) {
        (*el)->interface.description = xstrdup(description);
        return (*el)->interface.description == NULL;
    }

    return 0;
}

int interfaces_interface_hash_element_set_type(interfaces_interface_hash_element_t** el, const char* type)
{
    if ((*el)->interface.type) {
        FREE_SAFE((*el)->interface.type);
    }

    if (type) {
        (*el)->interface.type = xstrdup(type);
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
    (*el)->interface.encapsulation.dot1q_vlan.outer_tag.tag_type = xstrdup(encapsulation.dot1q_vlan.outer_tag.tag_type);
    (*el)->interface.encapsulation.dot1q_vlan.second_tag.tag_type = xstrdup(encapsulation.dot1q_vlan.second_tag.tag_type);

    return 0;
}

int interfaces_interface_hash_element_set_loopback(interfaces_interface_hash_element_t** el, const char* loopback)
{
    if ((*el)->interface.loopback) {
        FREE_SAFE((*el)->interface.loopback);
    }

    if (loopback) {
        (*el)->interface.loopback = xstrdup(loopback);
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
        FREE_SAFE((*el)->interface.parent_interface);
    }

    if (parent_interface) {
        (*el)->interface.parent_interface = xstrdup(parent_interface);
        return (*el)->interface.parent_interface == NULL;
    }

    return 0;
}

int interfaces_interface_type_nl2ly(const char* nl_type, const char** ly_type)
{
    int error = 0;

    if (nl_type == NULL) {
        // fix for now - investigate more - lo interface has type == NULL
        *ly_type = "iana-if-type:softwareLoopback";
        return 0;
    }

    if (strcmp(nl_type, "veth") == 0) {
        *ly_type = "iana-if-type:ethernetCsmacd";
    } else if (strcmp(nl_type, "vcan") == 0) {
        *ly_type = "iana-if-type:softwareLoopback";
    } else if (strcmp(nl_type, "vlan") == 0) {
        *ly_type = "iana-if-type:l2vlan";
    } else if (strcmp(nl_type, "dummy") == 0) {
        *ly_type = "iana-if-type:other";
    } else {
        error = -2;
    }

    return error;
}

int interfaces_interface_type_ly2nl(const char* ly_type, const char** nl_type)
{
    int error = 0;

    if (ly_type == NULL) {
        return -1;
    }

    if (strcmp(ly_type, "iana-if-type:ethernetCsmacd") == 0) {
        *nl_type = "veth";
    } else if (strcmp(ly_type, "iana-if-type:softwareLoopback") == 0) {
        *nl_type = "vcan";
    } else if (strcmp(ly_type, "iana-if-type:l2vlan") == 0) {
        *nl_type = "vlan";
    } else if (strcmp(ly_type, "iana-if-type:other") == 0) {
        *nl_type = "dummy";
    } else {
        error = -2;
    }

    return error;
}
