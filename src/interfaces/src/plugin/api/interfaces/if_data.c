#include "if_data.h"
#include "utils/memory.h"

void 
if_data_init(interfaces_interfaces_interface_element_t* interface)
{
    /* TODO: init all struct members */
	interface->name = NULL;
	interface->description = NULL;
	interface->type = NULL;
	interface->enabled = 0;
}

void 
interfaces_data_ht_root_init(interface_ht_element_t **if_root) 
{
    /* uthash root node has to be initialized to NULL */
    *root = NULL;
}

interface_ht_element_t *
interfaces_data_ht_get_by_name(interface_ht_element_t *if_root, char *name)
{
    interface_ht_element_t *elem = NULL;
    HASH_FIND_STR(if_root, name, elem);
    return elem;
}

void
interfaces_data_ht_set_name(interfaces_interfaces_interface_element_t *interface, char *name)
{
	interface->name = xstrdup(name);
}

int
interfaces_data_ht_add(interface_ht_element_t *if_root, char *name) 
{
    interface_ht_element_t *tmp = NULL, *elem = NULL;
    int rc = 0;

    HASH_FIND_STR(if_root, name, tmp);
    if (tmp != NULL) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "interface with name key: %s already exists in hash table", name);
        goto error_out;
    }

    elem = (interface_ht_element_t *) xmalloc(sizeof elem);
    if_data_init(&elem->interface);
    interfaces_data_ht_set_name(&elem->interface)

    /* since name is char *, *_KEYPTR has to be used instead of *_STR */
    HASH_ADD_KEYPTR(hh, if_root, elem->interface.name, sizeof(elem->interface.name), elem);
   
    goto out;
error_out:
    rc = -1;
out:
    return rc;
}

int
interfaces_data_ht_set_description(interface_ht_element_t *if_root, char *name, char *description) 
{
    interface_ht_element_t *elem = NULL;
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

int
interfaces_data_ht_set_type(interface_ht_element_t *if_root, char *name, char *type) 
{
    interface_ht_element_t *elem = NULL;
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

void
interfaces_data_ht_if_free(interfaces_interfaces_interface_element_t *interface)
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
}

void
interfaces_data_ht_free(interface_ht_element_t *if_root)
{
    interface_ht_element_t tmp = NULL, elem = NULL;
    
    HASH_ITER(hh, if_root, elem, tmp) {
        HASH_DEL(if_root, elem);
        interfaces_data_ht_if_free(&elem->interface);
        FREE_SAFE(elem);
    }
}

