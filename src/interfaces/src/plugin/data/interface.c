#include "interface.h"
#include "plugin/common.h"
#include "utils/memory.h"

#include <sysrepo.h>

void interfaces_data_ht_root_init(interface_ht_element_t** if_root)
{
    /* uthash root node has to be initialized to NULL */
    *if_root = NULL;
}

static void
interfaces_data_init(interfaces_interface_t* interface)
{
    /* TODO: init all struct members */
    interface->name = NULL;
    interface->description = NULL;
    interface->type = NULL;
    interface->enabled = 0;
    interface->loopback = NULL;
    interface->parent_interface = NULL;
}

interface_ht_element_t*
interfaces_data_ht_get_by_name(interface_ht_element_t* if_root, char* name)
{
    interface_ht_element_t* elem = NULL;
    HASH_FIND_STR(if_root, name, elem);
    return elem;
}

void interfaces_data_ht_set_name(interfaces_interface_t* interface, char* name)
{
    interface->name = xstrdup(name);
}

int interfaces_data_ht_add(interface_ht_element_t* if_root, char* name)
{
    interface_ht_element_t *tmp = NULL, *elem = NULL;
    int rc = 0;

    HASH_FIND_STR(if_root, name, tmp);
    if (tmp != NULL) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "interface with name key: %s already exists in hash table", name);
        goto error_out;
    }

    elem = (interface_ht_element_t*)xmalloc(sizeof elem);
    interfaces_data_init(&elem->interface);
    interfaces_data_ht_set_name(&elem->interface, name);

    /* since name is char *, *_KEYPTR has to be used instead of *_STR */
    HASH_ADD_KEYPTR(hh, if_root, elem->interface.name, sizeof(elem->interface.name), elem);

    goto out;
error_out:
    rc = -1;
out:
    return rc;
}

int interfaces_data_ht_set_description(interface_ht_element_t* if_root, char* name, char* description)
{
    interface_ht_element_t* elem = NULL;
    int rc = 0;

    elem = interfaces_data_ht_get_by_name(if_root, name);
    if (elem == NULL) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "interface with name key: %s non-existant in hash table", name);
        goto error_out;
    }

    if (elem->interface.description != NULL) {
        FREE_SAFE(elem->interface.description);
    }
    elem->interface.description = xstrdup(description);
    if (elem->interface.description == NULL) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "couldn't copy description: %s", description);
        goto error_out;
    }

    goto out;
error_out:
    rc = -1;
out:
    return rc;
}

int interfaces_data_ht_set_type(interface_ht_element_t* if_root, char* name, char* type)
{
    interface_ht_element_t* elem = NULL;
    int rc = 0;

    elem = interfaces_data_ht_get_by_name(if_root, name);
    if (elem == NULL) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "interface with name key: %s non-existant in hash table", name);
        goto error_out;
    }

    if (elem->interface.type != NULL) {
        FREE_SAFE(elem->interface.type);
    }
    elem->interface.type = xstrdup(type);
    if (elem->interface.type == NULL) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "couldn't copy type: %s", type);
        goto error_out;
    }

    goto out;
error_out:
    rc = -1;
out:
    return rc;
}

int interfaces_data_ht_set_loopback(interface_ht_element_t* if_root, char* name, char* loopback)
{
    interface_ht_element_t* elem = NULL;
    int rc = 0;

    elem = interfaces_data_ht_get_by_name(if_root, name);
    if (elem == NULL) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "interface with name key: %s non-existant in hash table", name);
        goto error_out;
    }

    if (elem->interface.loopback != NULL) {
        FREE_SAFE(elem->interface.loopback);
    }
    elem->interface.loopback = xstrdup(loopback);
    if (elem->interface.loopback == NULL) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "couldn't copy loopback: %s", loopback);
        goto error_out;
    }

    goto out;
error_out:
    rc = -1;
out:
    return rc;
}

int interfaces_data_ht_set_parent_interface(interface_ht_element_t* if_root, char* name, char* parent_interface)
{
    interface_ht_element_t* elem = NULL;
    int rc = 0;

    elem = interfaces_data_ht_get_by_name(if_root, name);
    if (elem == NULL) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "interface with name key: %s non-existant in hash table", name);
        goto error_out;
    }

    if (elem->interface.parent_interface != NULL) {
        FREE_SAFE(elem->interface.parent_interface);
    }
    elem->interface.parent_interface = xstrdup(parent_interface);
    if (elem->interface.parent_interface == NULL) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "couldn't copy parent interface: %s", parent_interface);
        goto error_out;
    }

    goto out;
error_out:
    rc = -1;
out:
    return rc;
}

void interfaces_data_ht_if_free(interfaces_interface_t* interface)
{
    /* TODO: free other struct members as needed */
    if (interface->name) {
        FREE_SAFE(interface->name);
    }
    if (interface->description) {
        FREE_SAFE(interface->description);
    }
    if (interface->type) {
        FREE_SAFE(interface->type);
    }
    if (interface->loopback) {
        FREE_SAFE(interface->loopback);
    }
    if (interface->parent_interface) {
        FREE_SAFE(interface->parent_interface);
    }
}

void interfaces_data_ht_free(interface_ht_element_t* if_root)
{
    interface_ht_element_t *tmp = NULL, *elem = NULL;

    HASH_ITER(hh, if_root, elem, tmp)
    {
        HASH_DEL(if_root, elem);
        interfaces_data_ht_if_free(&elem->interface);
        FREE_SAFE(elem);
    }
}
