#include "load.h"
#include "utlist.h"
#include "plugin/common.h"
#include "utils/memory.h"

#include <errno.h>

#include <linux/if_arp.h>

#include <netlink/route/link.h>
#include <netlink/route/link/vlan.h>

#include <sysrepo.h>

static char *interfaces_get_interface_name(struct rtnl_link *link)
{
        char *name = NULL;

		name = rtnl_link_get_name(link);
		if (name == NULL) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_get_name error");
		}

        return name;
}

static char *interfaces_get_interface_description(interfaces_ctx_t *ctx, char *name)
{
	int error = SR_ERR_OK;
	char path_buffer[PATH_MAX] = {0};
	sr_val_t *val = {0};
    char *description = NULL;

	/* conjure description path for this interface: /ietf-interfaces:interfaces/interface[name='test_interface']/description */
	error = snprintf(path_buffer, sizeof(path_buffer) / sizeof(char), "%s[name=\"%s\"]/description", INTERFACES_INTERFACES_LIST_YANG_PATH, name);
	if (error < 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf error");
		goto error_out;
	}

	// get the interface description value 
	error = sr_get_item(ctx->startup_session, path_buffer, 0, &val);
	if (error != SR_ERR_OK) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_get_item error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	if (strlen(val->data.string_val) > 0) {
	    description = val->data.string_val;
	}
    
error_out:
	return description;
}

static int read_from_sys_file(const char *dir_path, char *interface, int *val)
{
	int error = 0;
	char tmp_buffer[PATH_MAX];
	FILE *fptr = NULL;
	char tmp_val[4] = {0};

	error = snprintf(tmp_buffer, sizeof(tmp_buffer), "%s/%s/type", dir_path, interface);
	if (error < 0) {
		// snprintf error
		SRPLG_LOG_ERR(PLUGIN_NAME, "%s: snprintf failed", __func__);
		goto out;
	}

	/* snprintf returns return the number of bytes that are written - reset error to 0 */
	error = 0;

	fptr = fopen((const char *) tmp_buffer, "r");

	if (fptr != NULL) {
		fgets(tmp_val, sizeof(tmp_val), fptr);

		*val = atoi(tmp_val);

		fclose(fptr);
	} else {
		SRPLG_LOG_ERR(PLUGIN_NAME, "%s: failed to open %s: %s", __func__, tmp_buffer, strerror(errno));
		error = -1;
		goto out;
	}

out:
	return error;
}

static char *interfaces_get_interface_type(struct rtnl_link *link, char *name)
{
    int error = 0;
    char *type = NULL;

    type = rtnl_link_get_type(link);
    if (type == NULL) {
        /* rtnl_link_get_type() will return NULL for interfaces that were not
         * set with rtnl_link_set_type()
         *
         * get the type from: /sys/class/net/<interface_name>/type
         */
        const char *path_to_sys = "/sys/class/net/";
        int type_id = 0;

        error = read_from_sys_file(path_to_sys, name, &type_id);
        if (error != 0) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "%s: read_from_sys_file error", __func__);
            goto error_out;
        }

        switch (type_id) {
            case ARPHRD_ETHER:
                type = "eth";
                break;
            case ARPHRD_LOOPBACK:
                type = "lo";
                break;
            default:
                SRPLG_LOG_ERR(PLUGIN_NAME, "%s: unkown type_id: %d", __func__, type_id);
        }
    }
    
error_out:
	return type;
}

static uint8_t interfaces_get_interface_enabled(struct rtnl_link *link)
{
    uint8_t enabled = rtnl_link_get_operstate(link);

    /*
     * if the lo interface state is unknown, treat it as enabled
     * otherwise it will be set to down, and dns resolution won't work
     */
    if (IF_OPER_UP == enabled || IF_OPER_UNKNOWN == enabled) {
        enabled = interfaces_interfaces_interface_enable_enabled;
    } else if (IF_OPER_DOWN == enabled ) {
        enabled = interfaces_interfaces_interface_enable_disabled;
    }

	return enabled;
}

static char *interfaces_get_interface_parent_interface(struct nl_cache *cache, struct rtnl_link *link)
{
    int parent_index = 0;
	char parent_buffer[MAX_IF_NAME_LEN] = {0};
    char *parent_interface = NULL;

    if (rtnl_link_is_vlan(link)) {
        parent_index = rtnl_link_get_link(link);
        parent_interface = rtnl_link_i2name(cache, parent_index, parent_buffer, MAX_IF_NAME_LEN);
    }

    return parent_interface;
}

/* TODO: outer tag, second id, tag - maybe refactor all to pass by reference, return error */
static int interfaces_get_interface_vlan_id(struct rtnl_link *link, interfaces_interfaces_interface_t *interface)
{
    uint16_t *outer_vlan_id = &interface->encapsulation.dot1q_vlan.outer_tag.vlan_id;
    char *first = NULL;
    char *second = NULL;
    
    if (rtnl_link_is_vlan(link)) {
        *outer_vlan_id = (uint16_t) rtnl_link_vlan_get_id(link);
        if (*outer_vlan_id <= 0) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "%s: couldn't get vlan ID", __func__);
            return interfaces_load_failure;
        }

        /* check if vlan_id in name, if it is this is the QinQ interface, skip it */
        first = strchr(interface->name, '.');
        second = strchr(first+1, '.');

        if (second != 0) {
            return interfaces_load_continue;
        }
    }

    return interfaces_load_success;
}

static int interfaces_parse_link(interfaces_ctx_t *ctx, struct nl_sock *socket, struct nl_cache *cache, struct rtnl_link *link, interfaces_interfaces_interface_t *interface)
{
    int error = interfaces_load_success;
    *interface = (interfaces_interfaces_interface_t) {0};

    interface->name = interfaces_get_interface_name(link);
    if (interface->name == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "%s: name error", __func__);
        goto error_out;
    }
    
    interface->description = interfaces_get_interface_description(ctx, interface->name);
    if (interface->description == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "%s: description error", __func__);
        goto error_out;
    }
     
    interface->type = interfaces_get_interface_type(link, interface->name);
    if (interface->type == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "%s: type error", __func__);
        goto error_out;
    }

    interface->parent_interface = interfaces_get_interface_parent_interface(cache, link);
    if (interface->parent_interface == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "%s: parent_interface error", __func__);
        goto error_out;
    }

    error = interfaces_get_interface_vlan_id(link, interface);
    if (error != interfaces_load_success) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "%s: vlan id error", __func__);
        goto out; // error_out would possibly change the error
    }

    interface->enabled = interfaces_get_interface_enabled(link);

    goto out;
error_out:
    error = interfaces_load_failure;
out:
    /* allocated in interfaces_get_interface_description */
    if (interface->description != NULL) {
        FREE_SAFE(interface->description);
    }

    return error;
}

static int interfaces_add_link(interfaces_interface_hash_element_t **if_hash, interfaces_interfaces_interface_t *interface)
{
    int error = 0;
    
    interfaces_interface_hash_element_t *new_if_hash_elem = interfaces_interface_hash_element_new();
    interfaces_interface_hash_element_set_name(&new_if_hash_elem, interface->name);

    error = interfaces_interface_hash_add_element(if_hash, new_if_hash_elem);
    if (error != 0) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "%s: error adding link (%d)", __func__, error);
        goto error_out;
    }

    if (interface->description != NULL) {
        error = interfaces_interface_hash_element_set_description(&new_if_hash_elem, interface->description);
        if (error != 0) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "%s: error setting description (%d)", __func__, error);
            goto error_out;
        }
    }

    if (interface->type != NULL) {
        error = interfaces_interface_hash_element_set_type(&new_if_hash_elem, interface->type);
        if (error != 0) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "%s: error setting type (%d)", __func__, error);
            goto error_out;
        }
    }

    if (interface->parent_interface != NULL) {
        error = interfaces_interface_hash_element_set_parent_interface(&new_if_hash_elem, interface->parent_interface);
        if (error != 0) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "%s: error setting type (%d)", __func__, error);
            goto error_out;
        }
    }

    interfaces_interface_hash_element_set_enabled(&new_if_hash_elem, interface->enabled);

    goto out;
error_out:
out:
    return error; 
}

static struct rtnl_link *interfaces_get_next_link(struct rtnl_link *link) 
{
    return (struct rtnl_link *) nl_cache_get_next((struct nl_object *) link);
}

static int interfaces_interfaces_worker(interfaces_ctx_t *ctx, struct nl_sock *socket, struct nl_cache *cache, interfaces_interface_hash_element_t **if_hash)
{
    int error = 0;
	struct rtnl_link *link = NULL;
    interfaces_interfaces_interface_t interface = {0};

    link = (struct rtnl_link *) nl_cache_get_first(cache);

    while (link != NULL) {
        error = interfaces_parse_link(ctx, socket, cache, link, &interface);
        switch (error) {
            case interfaces_load_success:
                error = interfaces_add_link(if_hash, &interface);
                if (error != 0) {
                    SRPLG_LOG_ERR(PLUGIN_NAME, "%s: error adding link (%d)", __func__, error);
                    goto error_out;
                }
                break;
            case interfaces_load_continue:
                break;
            default:
		        SRPLG_LOG_ERR(PLUGIN_NAME, "%s: error parsing link (%d)", __func__, error);
                goto error_out;
        }

		link = interfaces_get_next_link(link);
	}

    goto out;
error_out:
    error = -1;
out:
    if (link != NULL) {
		rtnl_link_put(link);
	}

    return error;
}

int interfaces_load_interface(interfaces_ctx_t* ctx, interfaces_interface_hash_element_t **if_hash)
{
    int error = 0;
    struct nl_sock *socket = NULL;
	struct nl_cache *cache = NULL;

    *if_hash = interfaces_interface_hash_new();

	socket = nl_socket_alloc();
	if (socket == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nl_socket_alloc error: invalid socket");
		goto error_out;
	}
    
    error = nl_connect(socket, NETLINK_ROUTE);
	if (error != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nl_connect error (%d): %s", error, nl_geterror(error));
		goto error_out;
	}

    error = rtnl_link_alloc_cache(socket, AF_UNSPEC, &cache);
	if (error != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_alloc_cache error (%d): %s", error, nl_geterror(error));
		goto error_out;
	}

    error = interfaces_interfaces_worker(ctx, socket, cache, if_hash);
    if (error != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "interfaces_parse_links: error parsing links (%d)", error);
		goto error_out;
    }
    
    goto out; 
error_out:
    error = -1;    
out:
	if (socket != NULL) {
		nl_socket_free(socket);
	}

    if (cache != NULL) {
	    nl_cache_free(cache);
    }

    return error;
}

