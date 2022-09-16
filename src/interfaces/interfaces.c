/*
 * telekom / sysrepo-plugin-interfaces
 *
 * This program is made available under the terms of the
 * BSD 3-Clause license which is available at
 * https://opensource.org/licenses/BSD-3-Clause
 *
 * SPDX-FileCopyrightText: 2021 Deutsche Telekom AG
 * SPDX-FileContributor: Sartura Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <arpa/inet.h>
#include <dirent.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <linux/if.h>
#include <linux/if.h>
#include <linux/if_addr.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/limits.h>

#include <netlink/addr.h>
#include <netlink/cache.h>
#include <netlink/errno.h>
#include <netlink/netlink.h>
#include <netlink/netlink.h>
#include <netlink/route/addr.h>
#include <netlink/route/link.h>
#include <netlink/route/link/inet.h>
#include <netlink/route/link/inet6.h>
#include <netlink/route/link/vlan.h>
#include <netlink/route/neighbour.h>
#include <netlink/route/qdisc.h>
#include <netlink/route/tc.h>
#include <netlink/socket.h>

#include <libyang/libyang.h>
#include <libyang/tree_data.h>
#include <sysrepo.h>
#include <sysrepo/xpath.h>

#include "if_nic_stats.h"
#include "if_state.h"
#include "ip_data.h"
#include "link_data.h"
#include "utils/memory.h"
#include "datastore.h"

// other #defines
#define MAC_ADDR_MAX_LENGTH 18
#define MAX_DESCR_LEN 100
#define DATETIME_BUF_SIZE 30
#define CLASS_NET_LINE_LEN 1024
#define ADDR_STR_BUF_SIZE 45	 // max ip string length (15 for ipv4 and 45 for ipv6)
#define MAX_IF_NAME_LEN IFNAMSIZ // 16 bytes

// callbacks
static int interfaces_module_change_cb(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data);
static int interfaces_state_data_cb(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *path, const char *request_xpath, uint32_t request_id, struct lyd_node **parent, void *private_data);

// helper functions
static bool system_running_datastore_is_empty_check(sr_session_ctx_t *session);
static int fill_datastore_interface_list(sr_session_ctx_t *session, link_data_list_t *ld);
static int load_startup(sr_session_ctx_t *session, link_data_list_t *ld);
static bool check_system_interface(const char *interface_name, bool *system_interface);
int set_config_value(const char *xpath, const char *value);
int add_interface_ipv4(link_data_t *ld, struct rtnl_link *old, struct rtnl_link *req, int if_idx);
static int remove_ipv4_address(ip_address_list_t *addr_list, struct nl_sock *socket, struct rtnl_link *old);
int add_interface_ipv6(link_data_t *ld, struct rtnl_link *old, struct rtnl_link *req, int if_idx);
static int remove_ipv6_address(ip_address_list_t *addr_list, struct nl_sock *socket, struct rtnl_link *old);
static int remove_neighbors(ip_neighbor_list_t *nbor_list, struct nl_sock *socket, int addr_ver, int if_index);
int write_to_proc_file(const char *dir_path, char *interface, const char *fn, int val);
static int read_from_proc_file(const char *dir_path, char *interface, const char *fn, int *val);
static int read_interface_type_from_sys_file(const char *dir_path, char *interface, int *val);
int delete_config_value(const char *xpath, const char *value);
int update_link_info(link_data_list_t *ld, sr_change_oper_t operation);
static char *convert_ianaiftype(char *iana_if_type);
int add_existing_links(sr_session_ctx_t *session, link_data_list_t *ld);
static int get_interface_description(sr_session_ctx_t *session, char *name, char **description);
static int create_vlan_qinq(struct nl_sock *socket, struct nl_cache *cache, char *second_vlan_name, char *name, char *parent_interface, uint16_t outer_vlan_id, uint16_t second_vlan_id);
static int get_system_boot_time(char boot_datetime[]);

// function to start all threads for each interface
static int init_state_changes(void);

// callback function for a thread to track state changes on a specific interface (ifindex passed using void* data param)
static void *manager_thread_cb(void *data);
static void cache_change_cb(struct nl_cache *cache, struct nl_object *obj, int val, void *arg);

// static list of interface states for tracking state changes using threads
static if_state_list_t if_state_changes;

// global list of link_data structs
static link_data_list_t link_data_list = {0};

// link manager used for cacheing links info constantly
static struct nl_cache_mngr *link_manager = NULL;
static struct nl_cache *link_cache = NULL;

volatile int exit_application = 0;

int sr_plugin_init_cb(sr_session_ctx_t *session, void **private_data)
{
	int error = 0;
	sr_conn_ctx_t *connection = NULL;
	sr_session_ctx_t *startup_session = NULL;
	sr_subscription_ctx_t *subscription = NULL;
	char *desc_file_path = NULL;

	*private_data = NULL;

	error = link_data_list_init(&link_data_list);
	if (error != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "link_data_list_init error");
		goto out;
	}

	error = add_existing_links(session, &link_data_list);
	if (error != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "add_existing_links error");
		goto out;
	}

	SRPLG_LOG_INF(PLUGIN_NAME, "start session to startup datastore");

	if_state_list_init(&if_state_changes);

	error = init_state_changes();
	if (error != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "Error occurred while initializing threads to track interface changes... exiting");
		goto out;
	}

	connection = sr_session_get_connection(session);
	error = sr_session_start(connection, SR_DS_STARTUP, &startup_session);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_session_start error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	*private_data = startup_session;

	if (system_running_datastore_is_empty_check(session) == true) {
		SRPLG_LOG_INF(PLUGIN_NAME, "running DS is empty, loading data");

		error = fill_datastore_interface_list(session, &link_data_list);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "fill_datastore_interface_list error");
			goto error_out;
		}

		error = sr_copy_config(startup_session, BASE_YANG_MODEL, SR_DS_RUNNING, 0);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "sr_copy_config error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}
	}

	// load data from startup datastore to internal list
	error = load_startup(startup_session, &link_data_list);
	if (error != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "load_startup error");
		goto error_out;
	}

	// apply what is present in the startup datastore
	error = update_link_info(&link_data_list, SR_OP_CREATED);
	if (error != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "update_link_info error");
		goto error_out;
	}

	SRPLG_LOG_INF(PLUGIN_NAME, "subscribing to module change");

	// sub to any module change - for now
	error = sr_module_change_subscribe(session, BASE_YANG_MODEL, "/" BASE_YANG_MODEL ":*", interfaces_module_change_cb, *private_data, 0, 0, &subscription);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_module_change_subscribe error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	error = sr_oper_get_subscribe(session, BASE_YANG_MODEL, INTERFACES_YANG_MODEL "/*", interfaces_state_data_cb, NULL, 1, &subscription);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_oper_get_subscribe error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	SRPLG_LOG_INF(PLUGIN_NAME, "plugin init done");

	FREE_SAFE(desc_file_path);

	goto out;

error_out:
	if (subscription != NULL) {
		sr_unsubscribe(subscription);
	}

	if (desc_file_path != NULL) {
		FREE_SAFE(desc_file_path);
	}
out:
	return error ? SR_ERR_CALLBACK_FAILED : SR_ERR_OK;
}

static bool system_running_datastore_is_empty_check(sr_session_ctx_t *session)
{
	int error = SR_ERR_OK;
	bool is_empty = true;
	sr_val_t *values = NULL;
	size_t value_cnt = 0;

	error = sr_get_items(session, INTERFACE_LIST_YANG_PATH, 0, SR_OPER_DEFAULT, &values, &value_cnt);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_get_items error (%d): %s", error, sr_strerror(error));
		goto out;
	}

	// check if interface list is empty
	if (value_cnt > 0) {
		sr_free_values(values, value_cnt);
		is_empty = false;
	}
out:
	return is_empty;
}

static int fill_datastore_interface_list(sr_session_ctx_t *session, link_data_list_t *ld)
{
	int error = 0;
	for (int i = 0; i < ld->count; i++) {
		link_data_t *link = &ld->links[i];

		// general interface config
		char *interface_leaf_values[IF_LEAF_COUNT] = {NULL};
		interface_leaf_values[IF_DESCRIPTION] = link->description != NULL ? ld->links[i].description : "";
		interface_leaf_values[IF_ENABLED] = link->enabled;
		interface_leaf_values[IF_PARENT_INTERFACE] = link->extensions.parent_interface;
		char *type = link->type;
		if (strcmp(type, "lo") == 0) {
			interface_leaf_values[IF_TYPE] = "iana-if-type:softwareLoopback";
		} else if (strcmp(type, "eth") == 0) {
			interface_leaf_values[IF_TYPE] = "iana-if-type:ethernetCsmacd";
		} else if (strcmp(type, "vlan") == 0) {
			interface_leaf_values[IF_TYPE] = "iana-if-type:l2vlan";
		} else if (strcmp(type, "bridge") == 0) {
			interface_leaf_values[IF_TYPE] = "iana-if-type:bridge";
		} else if (strcmp(type, "dummy") == 0) {
			interface_leaf_values[IF_TYPE] = "iana-if-type:other"; // since dummy is not a real type
		} else {
			SRPLG_LOG_INF(PLUGIN_NAME, "fill_datastore_interface_list unsupported interface type %s", type);
			continue;
		}

		error = ds_set_general_interface_config(session, link->name, interface_leaf_values);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "ds_set_interface_general_info error");
			goto error_out;
		}

		// TODO: handle vlan interfaces

		// ipv4 config
		char *ipv4_leaf_values[IF_IP_LEAF_COUNT] = {NULL};
		ipv4_leaf_values[IF_IP_ENABLED] = link->ipv4.enabled == 0 ? "false" : "true";
		ipv4_leaf_values[IF_IP_FORWARDING] = link->ipv4.forwarding == 0 ? "false" : "true";
		char ipv4_mtu[16] = {0};
		if (link->ipv4.mtu > 0) {
			snprintf(ipv4_mtu, sizeof(ipv4_mtu), "%u", ld->links[i].ipv4.mtu);
			ipv4_leaf_values[IF_IP_MTU] = ipv4_mtu;
		}
		error = ds_set_interface_ipv4_config(session, link->name, ipv4_leaf_values);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "ds_set_interface_ipv4_config error");
			goto error_out;
		}

		// list of ipv4 addresses
		uint32_t ipv4_addr_count = link->ipv4.addr_list.count;
		for (uint32_t j = 0; j < ipv4_addr_count; j++) {
			if (link->ipv4.addr_list.addr[j].ip == NULL) { // in case we deleted an ip address it will be NULL
				continue;
			}
			ip_address_t *addr = &link->ipv4.addr_list.addr[j];

			char *ipv4_addr_leaf_values[IF_IPV4_ADDR_LEAF_COUNT] = {NULL};
			char prefix_len[8] = {0};
			snprintf(prefix_len, sizeof(prefix_len), "%u", addr->subnet);
			ipv4_addr_leaf_values[IF_IPV4_ADDR_PREFIX_LENGTH] = prefix_len;

			error = ds_add_interface_ipv4_address(session, link->name, addr->ip, ipv4_addr_leaf_values);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "ds_add_interface_ipv4_address error");
				goto error_out;
			}
		}

		// list of ipv4 neighbors
		uint32_t ipv4_neigh_count = link->ipv4.nbor_list.count;
		for (uint32_t j = 0; j < ipv4_neigh_count; j++) {
			ip_neighbor_t *neigh = &link->ipv4.nbor_list.nbor[j];

			char *ipv4_neigh_leaf_values[IF_IPV4_NEIGH_LEAF_COUNT] = {NULL};
			ipv4_neigh_leaf_values[IF_IPV4_NEIGH_LINK_LAYER_ADDR] = neigh->phys_addr;

			error = ds_add_interface_ipv4_neighbor(session, link->name, neigh->ip, ipv4_neigh_leaf_values);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "ds_add_interface_ipv4_neighbor error");
				goto error_out;
			}
		}

		// ipv6 config
		char *ipv6_leaf_values[IF_IP_LEAF_COUNT] = {NULL};
		ipv6_leaf_values[IF_IP_ENABLED] = link->ipv6.ip_data.enabled == 0 ? "false" : "true";
		ipv6_leaf_values[IF_IP_FORWARDING] = link->ipv6.ip_data.forwarding == 0 ? "false" : "true";
		char ipv6_mtu[16] = {0};
		if (link->ipv6.ip_data.mtu > 0) {
			snprintf(ipv6_mtu, sizeof(ipv6_mtu), "%u", ld->links[i].ipv6.ip_data.mtu);
			ipv4_leaf_values[IF_IP_MTU] = ipv6_mtu;
		}
		error = ds_set_interface_ipv6_config(session, link->name, ipv6_leaf_values);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "ds_set_interface_ipv6_config error");
			goto error_out;
		}

		// list of ipv6 addresses
		uint32_t ipv6_addr_count = link->ipv6.ip_data.addr_list.count;
		for (uint32_t j = 0; j < ipv6_addr_count; j++) {
			if (link->ipv6.ip_data.addr_list.addr[j].ip == NULL) { // in case we deleted an ip address it will be NULL
				continue;
			}
			ip_address_t *addr = &link->ipv6.ip_data.addr_list.addr[j];

			char *ipv6_addr_leaf_values[IF_IPV6_ADDR_LEAF_COUNT] = {NULL};
			char prefix_len[8] = {0};
			snprintf(prefix_len, sizeof(prefix_len), "%u", addr->subnet);
			ipv6_addr_leaf_values[IF_IPV6_ADDR_PREFIX_LENGTH] = prefix_len;

			error = ds_add_interface_ipv6_address(session, link->name, addr->ip, ipv6_addr_leaf_values);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "ds_add_interface_ipv6_address error");
				goto error_out;
			}
		}

		// list of ipv6 neighbors
		uint32_t ipv6_neigh_count = ld->links[i].ipv6.ip_data.nbor_list.count;
		for (uint32_t j = 0; j < ipv6_neigh_count; j++) {
			ip_neighbor_t *neigh = &link->ipv6.ip_data.nbor_list.nbor[j];

			char *ipv6_neigh_leaf_values[IF_IPV6_NEIGH_LEAF_COUNT] = {NULL};
			ipv6_neigh_leaf_values[IF_IPV6_NEIGH_LINK_LAYER_ADDR] = neigh->phys_addr;

			error = ds_add_interface_ipv6_neighbor(session, link->name, neigh->ip, ipv6_neigh_leaf_values);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "ds_add_interface_ipv6_neighbor error");
				goto error_out;
			}
		}
	}
	error = sr_apply_changes(session, 0);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_apply_changes error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	return 0;
error_out:
	return -1;
}

static int load_startup(sr_session_ctx_t *session, link_data_list_t *ld)
{
	int error = 0;
	sr_val_t *vals = NULL;
	char *val = NULL;
	char val_buf[10] = {0};
	char *xpath = NULL;
	size_t i, val_count = 0;
	int rc = -1;

	error = sr_get_items(session, "/ietf-interfaces:interfaces//.", 0, 0, &vals, &val_count);
	if (error != SR_ERR_OK) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_get_items error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	for (i = 0; i < val_count; ++i) {
		switch (vals[i].type) {
			case SR_STRING_T:
			case SR_IDENTITYREF_T:
				val = xstrdup(vals[i].data.string_val);
				break;
			case SR_BOOL_T:
				val = xstrdup(vals[i].data.bool_val ? "true" : "false");
				break;
			case SR_UINT8_T:
			case SR_UINT16_T:
			case SR_UINT32_T:
				error = snprintf(val_buf, sizeof(val_buf), "%d", vals[i].data.uint16_val);
				if (error < 0) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf error");
					goto error_out;
				}

				val = xstrdup(val_buf);
				break;
			default:
				continue;
		}

		xpath = xstrdup(vals[i].xpath);

		error = set_config_value(xpath, val);

		if (error != 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "set_config_value error (%d)", error);
			goto error_out;
		}

		FREE_SAFE(xpath);
		FREE_SAFE(val);
	}

	rc = 0;

error_out:
	if (xpath != NULL) {
		FREE_SAFE(xpath);
	}
	if (val != NULL) {
		FREE_SAFE(val);
	}
	if (vals) {
		sr_free_values(vals, val_count);
	}

	return rc;
}

void sr_plugin_cleanup_cb(sr_session_ctx_t *session, void *private_data)
{
	sr_session_ctx_t *startup_session = (sr_session_ctx_t *) private_data;

	char path_buffer[PATH_MAX] = {0};

	// copy the running datastore to startup one, in case we reboot
	sr_copy_config(startup_session, BASE_YANG_MODEL, SR_DS_RUNNING, 0);

	for (int i = 0; i < link_data_list.count; i++) {
		const link_data_t *link = &link_data_list.links[i];
		for (size_t j = 0; j < link->ipv4.addr_list.count; j++) {
			const ip_address_t *addr = &link->ipv4.addr_list.addr[j];

			SRPLG_LOG_DBG(PLUGIN_NAME, "Address = %s, origin = %d", addr->ip, addr->origin);
			if (addr->origin == ip_address_origin_dhcp) {
				// delete from startup
				SRPLG_LOG_DBG(PLUGIN_NAME, "Deleting %s from startup", addr->ip);
				snprintf(path_buffer, sizeof(path_buffer), "/" BASE_YANG_MODEL ":interfaces/interface[name=\"%s\"]/ietf-ip:ipv4/address[ip=\"%s\"]", link->name, addr->ip);
				SRPLG_LOG_DBG(PLUGIN_NAME, "Path = %s", path_buffer);
				sr_delete_item(startup_session, path_buffer, 0);
			}
		}
		for (size_t j = 0; j < link->ipv6.ip_data.addr_list.count; j++) {
			const ip_address_t *addr = &link->ipv6.ip_data.addr_list.addr[j];

			SRPLG_LOG_DBG(PLUGIN_NAME, "Address = %s, origin = %d", addr->ip, addr->origin);
			if (addr->origin == ip_address_origin_dhcp) {
				snprintf(path_buffer, sizeof(path_buffer), "/" BASE_YANG_MODEL ":interfaces/interface[name=\"%s\"]/ipv6/address[ip=\"%s\"]/*", link->name, addr->ip);
				sr_delete_item(startup_session, path_buffer, 0);
			}
		}
	}

	sr_apply_changes(startup_session, 0);

	exit_application = 1;

	if (startup_session) {
		sr_session_stop(startup_session);
	}

	link_data_list_free(&link_data_list);
	if_state_list_free(&if_state_changes);
	nl_cache_mngr_free(link_manager);

	SRPLG_LOG_INF(PLUGIN_NAME, "plugin cleanup finished");
}

static int interfaces_module_change_cb(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data)
{
	int error = 0;
	sr_change_iter_t *system_change_iter = NULL;
	sr_change_oper_t operation = SR_OP_CREATED;
	const struct lyd_node *node = NULL;
	char path[512] = {0};
	const char *prev_value = NULL;
	const char *prev_list = NULL;
	int prev_default = false;
	char *node_xpath = NULL;
	char *node_value = NULL;

	SRPLG_LOG_INF(PLUGIN_NAME, "module_name: %s, xpath: %s, event: %d, request_id: %u", module_name, xpath, event, request_id);

	if (event == SR_EV_ABORT) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "aborting changes for: %s", xpath);
		error = -1;
		goto error_out;
	}

	if (event == SR_EV_DONE) {
		error = sr_copy_config(session, BASE_YANG_MODEL, SR_DS_RUNNING, 0);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "sr_copy_config error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}
	}

	if (event == SR_EV_CHANGE) {
		sprintf(path, "%s//.", xpath);

		error = sr_get_changes_iter(session, path, &system_change_iter);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "sr_get_changes_iter error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}
		while (sr_get_change_tree_next(session, system_change_iter, &operation, &node, &prev_value, &prev_list, &prev_default) == SR_ERR_OK) {
			node_xpath = lyd_path(node, LYD_PATH_STD, NULL, 0);

			if (node->schema->nodetype == LYS_LEAF || node->schema->nodetype == LYS_LEAFLIST) {
				node_value = xstrdup(lyd_get_value(node));
			}

			SRPLG_LOG_DBG(PLUGIN_NAME, "node_xpath: %s; prev_val: %s; node_val: %s; operation: %d", node_xpath, prev_value, node_value, operation);

			if (node->schema->nodetype == LYS_LEAF || node->schema->nodetype == LYS_LEAFLIST) {
				if (operation == SR_OP_CREATED || operation == SR_OP_MODIFIED) {
					error = set_config_value(node_xpath, node_value);
					if (error) {
						SRPLG_LOG_ERR(PLUGIN_NAME, "set_config_value error (%d)", error);
						goto error_out;
					}
				} else if (operation == SR_OP_DELETED) {
					// check if this is a system interface (e.g.: lo, wlan0, enp0s31f6 etc.)
					bool system_interface = false;
					error = check_system_interface(node_value, &system_interface);
					if (error) {
						SRPLG_LOG_ERR(PLUGIN_NAME, "check_system_interface error");
						goto error_out;
					}

					// check if system interface but also
					// check if parent-interface node (virtual interfaces can have a system interface as parent)
					if (system_interface && !(strstr(node_xpath, "/ietf-if-extensions:parent-interface") != NULL)) {
						SRPLG_LOG_ERR(PLUGIN_NAME, "Can't delete a system interface");
						FREE_SAFE(node_xpath);
						sr_free_change_iter(system_change_iter);
						return SR_ERR_INVAL_ARG;
					}

					error = delete_config_value(node_xpath, node_value);
					if (error) {
						SRPLG_LOG_ERR(PLUGIN_NAME, "delete_config_value error (%d)", error);
						goto error_out;
					}
				}
			}
			FREE_SAFE(node_xpath);
			FREE_SAFE(node_value);
		}

		error = update_link_info(&link_data_list, operation);
		if (error) {
			error = SR_ERR_CALLBACK_FAILED;
			SRPLG_LOG_ERR(PLUGIN_NAME, "update_link_info error");
			goto error_out;
		}
	}
	goto out;

error_out:
	// nothing for now
out:
	if (node_xpath != NULL) {
		FREE_SAFE(node_xpath);
	}
	if (node_value != NULL) {
		FREE_SAFE(node_value);
	}
	if (system_change_iter != NULL) {
		sr_free_change_iter(system_change_iter);
	}

	return error ? SR_ERR_CALLBACK_FAILED : SR_ERR_OK;
}

int set_config_value(const char *xpath, const char *value)
{
	int error = SR_ERR_OK;
	char *interface_node = NULL;
	char *interface_node_name = NULL;
	char *address_node_ip = NULL;
	char *neighbor_node_ip = NULL;
	sr_xpath_ctx_t state = {0};
	char *xpath_cpy = strdup(xpath);

	interface_node = sr_xpath_node_name((char *) xpath);
	if (interface_node == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_xpath_node_name error");
		error = SR_ERR_CALLBACK_FAILED;
		goto out;
	}

	interface_node_name = sr_xpath_key_value((char *) xpath, "interface", "name", &state);

	if (interface_node_name == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_xpath_key_value error");
		error = SR_ERR_CALLBACK_FAILED;
		goto out;
	}

	error = link_data_list_add(&link_data_list, interface_node_name);
	if (error != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "link_data_list_add error");
		error = SR_ERR_CALLBACK_FAILED;
		goto out;
	}

	if (strcmp(interface_node, "description") == 0) {
		// change desc
		error = link_data_list_set_description(&link_data_list, interface_node_name, (char *) value);
		if (error != 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "link_data_list_set_description error");
			error = SR_ERR_CALLBACK_FAILED;
			goto out;
		}
	} else if (strcmp(interface_node, "type") == 0) {
		// change type

		// convert the iana-if-type to a "real" interface type which libnl understands
		char *interface_type = convert_ianaiftype((char *) value);
		if (interface_type == NULL) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "convert_ianaiftype error");
			error = SR_ERR_CALLBACK_FAILED;
			goto out;
		}

		error = link_data_list_set_type(&link_data_list, interface_node_name, interface_type);
		if (error != 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "link_data_list_set_type error");
			error = SR_ERR_CALLBACK_FAILED;
			goto out;
		}
	} else if (strcmp(interface_node, "enabled") == 0 && strstr(xpath_cpy, "ietf-ip:") == 0) {
		// change enabled
		error = link_data_list_set_enabled(&link_data_list, interface_node_name, (char *) value);
		if (error != 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "link_data_list_set_enabled error");
			error = SR_ERR_CALLBACK_FAILED;
			goto out;
		}
	} else if (strstr(xpath_cpy, "ietf-ip:ipv4") != 0) {
		SRPLG_LOG_DBG(PLUGIN_NAME, "ietf-ip:ipv4 change: '%s' on interface '%s'", interface_node, interface_node_name);
		memset(&state, 0, sizeof(sr_xpath_ctx_t));
		// check if an ip address has been added
		address_node_ip = sr_xpath_key_value((char *) xpath_cpy, "address", "ip", &state);

		// check if a neighbor has been added
		neighbor_node_ip = sr_xpath_key_value((char *) xpath_cpy, "neighbor", "ip", &state);

		if (address_node_ip != NULL) {
			// address has been added -> check for interface node (last node of the path) -> (prefix-length || netmask)
			if (strcmp(interface_node, "prefix-length") == 0) {
				error = link_data_list_add_ipv4_address(&link_data_list, interface_node_name, address_node_ip, (char *) value, ip_subnet_type_prefix_length, IFA_F_PERMANENT);
				if (error != 0) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "link_data_list_add_ipv4_address error (%d) : %s", error, strerror(error));
					error = SR_ERR_CALLBACK_FAILED;
					goto out;
				}
			} else if (strcmp(interface_node, "netmask") == 0) {
				error = link_data_list_add_ipv4_address(&link_data_list, interface_node_name, address_node_ip, (char *) value, ip_subnet_type_netmask, IFA_F_PERMANENT);
				if (error != 0) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "link_data_list_add_ipv4_address error (%d) : %s", error, strerror(error));
					error = SR_ERR_CALLBACK_FAILED;
					goto out;
				}
			}
		} else if (neighbor_node_ip != NULL) {
			if (strcmp(interface_node, "link-layer-address") == 0) {
				error = link_data_list_add_ipv4_neighbor(&link_data_list, interface_node_name, neighbor_node_ip, (char *) value);
				if (error != 0) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "link_data_list_add_ipv4_neighbor error (%d) : %s", error, strerror(error));
					error = SR_ERR_CALLBACK_FAILED;
					goto out;
				}
			}
		} else if (strcmp(interface_node, "enabled") == 0) {
			error = link_data_list_set_ipv4_enabled(&link_data_list, interface_node_name, (char *) value);
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "link_data_list_set_ipv4_enabled error (%d) : %s", error, strerror(error));
				error = SR_ERR_CALLBACK_FAILED;
				goto out;
			}
		} else if (strcmp(interface_node, "forwarding") == 0) {
			error = link_data_list_set_ipv4_forwarding(&link_data_list, interface_node_name, (char *) value);
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "link_data_list_set_ipv4_forwarding error (%d) : %s", error, strerror(error));
				error = SR_ERR_CALLBACK_FAILED;
				goto out;
			}
		} else if (strcmp(interface_node, "mtu") == 0) {
			error = link_data_list_set_ipv4_mtu(&link_data_list, interface_node_name, (char *) value);
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "link_data_list_set_ipv4_mtu error (%d) : %s", error, strerror(error));
				error = SR_ERR_CALLBACK_FAILED;
				goto out;
			}
		}
	} else if (strstr(xpath_cpy, "ietf-ip:ipv6") != 0) {
		SRPLG_LOG_DBG(PLUGIN_NAME, "ietf-ip:ipv6 change: '%s' on interface '%s'", interface_node, interface_node_name);
		// check if an ip address has been added
		address_node_ip = sr_xpath_key_value((char *) xpath_cpy, "address", "ip", &state);
		// check if a neighbor has been added
		neighbor_node_ip = sr_xpath_key_value((char *) xpath_cpy, "neighbor", "ip", &state);

		if (address_node_ip != NULL) {
			// address has been added -> check for interface node (last node of the path) -> (prefix-length || netmask)
			if (strcmp(interface_node, "prefix-length") == 0) {
				error = link_data_list_add_ipv6_address(&link_data_list, interface_node_name, address_node_ip, (char *) value, IFA_F_PERMANENT);
				if (error != 0) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "link_data_list_add_ipv4_address error (%d) : %s", error, strerror(error));
					error = SR_ERR_CALLBACK_FAILED;
					goto out;
				}
			} else if (strcmp(interface_node, "netmask") == 0) {
				error = link_data_list_add_ipv6_address(&link_data_list, interface_node_name, address_node_ip, (char *) value, IFA_F_PERMANENT);
				if (error != 0) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "link_data_list_add_ipv4_address error (%d) : %s", error, strerror(error));
					error = SR_ERR_CALLBACK_FAILED;
					goto out;
				}
			}
		} else if (neighbor_node_ip != NULL) {
			if (strcmp(interface_node, "link-layer-address") == 0) {
				error = link_data_list_add_ipv6_neighbor(&link_data_list, interface_node_name, neighbor_node_ip, (char *) value);
				if (error != 0) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "link_data_list_add_ipv4_neighbor error (%d) : %s", error, strerror(error));
					error = SR_ERR_CALLBACK_FAILED;
					goto out;
				}
			}
		} else if (strcmp(interface_node, "enabled") == 0) {
			error = link_data_list_set_ipv6_enabled(&link_data_list, interface_node_name, (char *) value);
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "link_data_list_set_ipv4_enabled error (%d) : %s", error, strerror(error));
				error = SR_ERR_CALLBACK_FAILED;
				goto out;
			}
		} else if (strcmp(interface_node, "forwarding") == 0) {
			error = link_data_list_set_ipv6_forwarding(&link_data_list, interface_node_name, (char *) value);
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "link_data_list_set_ipv4_forwarding error (%d) : %s", error, strerror(error));
				error = SR_ERR_CALLBACK_FAILED;
				goto out;
			}
		} else if (strcmp(interface_node, "mtu") == 0) {
			error = link_data_list_set_ipv6_mtu(&link_data_list, interface_node_name, (char *) value);
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "link_data_list_set_ipv4_mtu error (%d) : %s", error, strerror(error));
				error = SR_ERR_CALLBACK_FAILED;
				goto out;
			}
		}
	} else if (strcmp(interface_node, "ietf-if-extensions:parent-interface") == 0) {
		// change parent-interface
		error = link_data_list_set_parent(&link_data_list, interface_node_name, (char *) value);
		if (error != 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "link_data_list_set_parent error");
			error = SR_ERR_CALLBACK_FAILED;
			goto out;
		}
	} else if (strstr(xpath_cpy, "ietf-if-extensions:encapsulation/ietf-if-vlan-encapsulation:dot1q-vlan/outer-tag/tag-type") != 0) {
		error = link_data_list_set_outer_tag_type(&link_data_list, interface_node_name, (char *) value);
		if (error != 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "link_data_list_set_parent error");
			error = SR_ERR_CALLBACK_FAILED;
			goto out;
		}
	} else if (strstr(xpath_cpy, "ietf-if-extensions:encapsulation/ietf-if-vlan-encapsulation:dot1q-vlan/outer-tag/vlan-id") != 0) {
		error = link_data_list_set_outer_vlan_id(&link_data_list, interface_node_name, (uint16_t) atoi(value));
		if (error != 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "link_data_list_set_parent error");
			error = SR_ERR_CALLBACK_FAILED;
			goto out;
		}
	} else if (strstr(xpath_cpy, "ietf-if-extensions:encapsulation/ietf-if-vlan-encapsulation:dot1q-vlan/second-tag/tag-type") != 0) {
		error = link_data_list_set_second_tag_type(&link_data_list, interface_node_name, (char *) value);
		if (error != 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "link_data_list_set_parent error");
			error = SR_ERR_CALLBACK_FAILED;
			goto out;
		}
	} else if (strstr(xpath_cpy, "ietf-if-extensions:encapsulation/ietf-if-vlan-encapsulation:dot1q-vlan/second-tag/vlan-id") != 0) {
		error = link_data_list_set_second_vlan_id(&link_data_list, interface_node_name, (uint16_t) atoi(value));
		if (error != 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "link_data_list_set_parent error");
			error = SR_ERR_CALLBACK_FAILED;
			goto out;
		}
	}

	goto out;

out:
	FREE_SAFE(xpath_cpy);
	if (xpath_cpy != NULL) {
		FREE_SAFE(xpath_cpy);
	}

	return error ? SR_ERR_CALLBACK_FAILED : SR_ERR_OK;
}

// TODO: move this function to a helper functions file in utils
static char *convert_ianaiftype(char *iana_if_type)
{
	char *if_type = NULL;

	if (strstr(iana_if_type, "softwareLoopback") != NULL) {
		if_type = "lo";
	} else if (strstr(iana_if_type, "ethernetCsmacd") != NULL) {
		if_type = "eth";
	} else if (strstr(iana_if_type, "l2vlan") != NULL) {
		if_type = "vlan";
	} else if (strstr(iana_if_type, "bridge") != NULL) {
		if_type = "bridge";
	} else if (strstr(iana_if_type, "other") != NULL) {
		if_type = "dummy";
	}
	// TODO: add support for more interface types

	return if_type;
}

int delete_config_value(const char *xpath, const char *value)
{
	int error = SR_ERR_OK;
	char *interface_node = NULL;
	char *interface_node_name = NULL;
	sr_xpath_ctx_t state = {0};
	char *xpath_cpy = strdup(xpath);

	interface_node = sr_xpath_node_name((char *) xpath);
	if (interface_node == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_xpath_node_name error");
		error = SR_ERR_CALLBACK_FAILED;
		goto out;
	}

	interface_node_name = sr_xpath_key_value((char *) xpath, "interface", "name", &state);
	if (interface_node_name == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_xpath_key_value error");
		error = SR_ERR_CALLBACK_FAILED;
		goto out;
	}

	if (strcmp(interface_node, "name") == 0) {
		// mark for deletion
		error = link_data_list_set_delete(&link_data_list, interface_node_name, true);
		if (error != 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "link_data_list_set_delete error");
			error = SR_ERR_CALLBACK_FAILED;
			goto out;
		}
	} else if (strcmp(interface_node, "description") == 0) {
		// set description to empty string
		error = link_data_list_set_description(&link_data_list, interface_node_name, "");
		if (error != 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "link_data_list_set_description error");
			error = SR_ERR_CALLBACK_FAILED;
			goto out;
		}
	} else if (strcmp(interface_node, "enabled") == 0) {
		// set enabled to false
		error = link_data_list_set_enabled(&link_data_list, interface_node_name, "");
		if (error != 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "link_data_list_set_enabled error");
			error = SR_ERR_CALLBACK_FAILED;
			goto out;
		}
	} else if (strstr(xpath_cpy, "ietf-ip:ipv4") != 0) {
		memset(&state, 0, sizeof(sr_xpath_ctx_t));

		// check if an ip address needs to be removed
		char *address_node_ip = sr_xpath_key_value((char *) xpath_cpy, "address", "ip", &state);

		// check if a neighbor needs to be removed
		char *neighbor_node_ip = sr_xpath_key_value((char *) xpath_cpy, "neighbor", "ip", &state);

		if (address_node_ip != NULL) {
			if (strcmp(interface_node, "ip") == 0) {
				error = link_data_list_set_delete_ipv4_address(&link_data_list, interface_node_name, address_node_ip);
				if (error != 0) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "link_data_list_set_delete_ipv4_address error (%d) : %s", error, strerror(error));
					error = SR_ERR_CALLBACK_FAILED;
					goto out;
				}
			}
		} else if (neighbor_node_ip != NULL) {
			if (strcmp(interface_node, "ip") == 0) {
				error = link_data_list_set_delete_ipv4_neighbor(&link_data_list, interface_node_name, (char *) value);
				if (error != 0) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "link_data_list_set_delete_ipv4_neighbor error (%d) : %s", error, strerror(error));
					error = SR_ERR_CALLBACK_FAILED;
					goto out;
				}
			}
		}
	} else if (strstr(xpath_cpy, "ietf-ip:ipv6") != 0) {
		memset(&state, 0, sizeof(sr_xpath_ctx_t));

		// check if an ip address needs to be removed
		char *address_node_ip = sr_xpath_key_value((char *) xpath_cpy, "address", "ip", &state);

		// check if a neighbor needs to be removed
		char *neighbor_node_ip = sr_xpath_key_value((char *) xpath_cpy, "neighbor", "ip", &state);

		if (address_node_ip != NULL) {
			if (strcmp(interface_node, "ip") == 0) {
				error = link_data_list_set_delete_ipv6_address(&link_data_list, interface_node_name, address_node_ip);
				if (error != 0) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "link_data_list_set_delete_ipv6_address error (%d) : %s", error, strerror(error));
					error = SR_ERR_CALLBACK_FAILED;
					goto out;
				}
			}
		} else if (neighbor_node_ip != NULL) {
			if (strcmp(interface_node, "ip") == 0) {
				error = link_data_list_set_delete_ipv6_neighbor(&link_data_list, interface_node_name, (char *) value);
				if (error != 0) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "link_data_list_set_delete_ipv6_neighbor error (%d) : %s", error, strerror(error));
					error = SR_ERR_CALLBACK_FAILED;
					goto out;
				}
			}
		}
	}

out:
	FREE_SAFE(xpath_cpy);
	return error ? SR_ERR_CALLBACK_FAILED : SR_ERR_OK;
}

int update_link_info(link_data_list_t *ld, sr_change_oper_t operation)
{
	struct nl_sock *socket = NULL;
	struct nl_cache *cache = NULL;
	struct rtnl_link *old = NULL;
	struct rtnl_link *old_vlan_qinq = NULL;
	struct rtnl_link *request = NULL;

	int error = SR_ERR_OK;

	socket = nl_socket_alloc();
	if (socket == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nl_socket_alloc error: invalid socket");
		goto out;
	}

	if ((error = nl_connect(socket, NETLINK_ROUTE)) != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nl_connect error (%d): %s", error, nl_geterror(error));
		goto out;
	}

	error = rtnl_link_alloc_cache(socket, AF_UNSPEC, &cache);
	if (error != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_alloc_cache error (%d): %s", error, nl_geterror(error));
		goto out;
	}

	for (int i = 0; i < ld->count; i++) {
		char *name = ld->links[i].name;
		char *type = ld->links[i].type;
		char *enabled = ld->links[i].enabled;
		char *parent_interface = ld->links[i].extensions.parent_interface;
		uint16_t outer_vlan_id = ld->links[i].extensions.encapsulation.dot1q_vlan.outer_vlan_id;
		uint16_t second_vlan_id = ld->links[i].extensions.encapsulation.dot1q_vlan.second_vlan_id;
		char second_vlan_name[MAX_IF_NAME_LEN] = {0};
		bool delete = ld->links[i].delete;

		if (name == NULL) { //|| type == 0) {
			continue;
		}

		// handle vlan QinQ interfaces
		if (type != NULL && strcmp(type, "vlan") == 0 && second_vlan_id != 0) {
			error = snprintf(second_vlan_name, sizeof(second_vlan_name), "%s.%d", name, second_vlan_id);
			if (error < 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf error");
				goto out;
			}
		}

		old = rtnl_link_get_by_name(cache, name);
		// check for second vlan (QinQ) as well
		old_vlan_qinq = rtnl_link_get_by_name(cache, second_vlan_name);
		request = rtnl_link_alloc();

		// delete link (interface) if marked for deletion
		if (delete) {
			// delete the link
			error = rtnl_link_delete(socket, old);
			if (error < 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_delete error (%d): %s", error, nl_geterror(error));
				goto out;
			}

			// free the link from link_data_list
			link_data_free(&ld->links[i]);
			// set delete to false
			ld->links[i].delete = false;

			// cleanup
			if (old != NULL) {
				rtnl_link_put(old);
			}

			if (old_vlan_qinq != NULL) {
				rtnl_link_put(old_vlan_qinq);
			}

			if (request != NULL) {
				rtnl_link_put(request);
			}

			// and continue
			continue;
		}

		// check if any ipv4 addresses need to be removed
		uint32_t ipv4_addr_count = ld->links[i].ipv4.addr_list.count;

		if (ipv4_addr_count > 0) {
			error = remove_ipv4_address(&ld->links[i].ipv4.addr_list, socket, old);
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "remove_ipv4_address error (%d): %s", error, nl_geterror(error));
				goto out;
			}
		}

		// check if any ipv6 addresses need to be removed
		uint32_t ipv6_addr_count = ld->links[i].ipv6.ip_data.addr_list.count;

		if (ipv6_addr_count > 0) {
			error = remove_ipv6_address(&ld->links[i].ipv6.ip_data.addr_list, socket, old);
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "remove_ipv6_address error (%d): %s", error, nl_geterror(error));
				goto out;
			}
		}

		if (old != NULL) {
			int index = rtnl_link_get_ifindex(old);

			// check if any ipv4 neighbors need to be removed
			uint32_t ipv4_neigh_count = ld->links[i].ipv4.nbor_list.count;

			if (ipv4_neigh_count > 0) {
				error = remove_neighbors(&ld->links[i].ipv4.nbor_list, socket, AF_INET, index);
				if (error != 0) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "remove_neighbors error");
					goto out;
				}
			}

			// check if any ipv6 neighbors need to be removed
			uint32_t ipv6_neigh_count = ld->links[i].ipv6.ip_data.nbor_list.count;

			if (ipv6_neigh_count > 0) {
				error = remove_neighbors(&ld->links[i].ipv6.ip_data.nbor_list, socket, AF_INET6, index);
				if (error != 0) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "remove_neighbors error");
					goto out;
				}
			}
		}

		// type
		if (type != NULL) {
			// handle vlan interfaces
			if (strcmp(type, "vlan") == 0) {
				// if second vlan id is present treat it as QinQ vlan
				if (second_vlan_id != 0) {
					// update the last-change state list
					uint8_t state = rtnl_link_get_operstate(request);

					if_state_list_add(&if_state_changes, state, second_vlan_name);
					if_state_list_add(&if_state_changes, state, name);

					// then create the interface with new parameters
					create_vlan_qinq(socket, cache, second_vlan_name, name, parent_interface, outer_vlan_id, second_vlan_id);

				} else if (second_vlan_id == 0) {
					// normal vlan interface
					error = rtnl_link_set_type(request, type);
					if (error < 0) {
						SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_set_type error (%d): %s", error, nl_geterror(error));
						goto out;
					}
					// if only the outer vlan is present, treat is an normal vlan
					int master_index = rtnl_link_name2i(cache, parent_interface);
					rtnl_link_set_link(request, master_index);

					error = rtnl_link_vlan_set_id(request, outer_vlan_id);
				}
			} else {
				error = rtnl_link_set_type(request, type);
				if (error < 0) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_set_type error (%d): %s", error, nl_geterror(error));
					goto out;
				}
			}
		}

		// enabled
		if (enabled != NULL) {
			if (strcmp(enabled, "true") == 0) {
				// set the interface to UP
				rtnl_link_set_flags(request, (unsigned int) rtnl_link_str2flags("up"));
				rtnl_link_set_operstate(request, IF_OPER_UP);
			} else {
				// set the interface to DOWN
				rtnl_link_unset_flags(request, (unsigned int) rtnl_link_str2flags("up"));
				rtnl_link_set_operstate(request, IF_OPER_DOWN);
			}
		}

		if (old != NULL) {
			// add ipv4/ipv6 options
			error = add_interface_ipv4(&ld->links[i], old, request, rtnl_link_get_ifindex(old));
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "add_interface_ipv4 error");
				goto out;
			}

			// the interface with name already exists, change it
			error = rtnl_link_change(socket, old, request, 0);
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_change error (%d): %s", error, nl_geterror(error));
				goto out;
			}

			error = add_interface_ipv6(&ld->links[i], old, request, rtnl_link_get_ifindex(old));
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "add_interface_ipv6 error");
				goto out;
			}

			// update ipv6 config
			error = rtnl_link_change(socket, old, request, 0);
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_change error (%d): %s", error, nl_geterror(error));
				goto out;
			}
		} else if (operation != SR_OP_DELETED) {
			// the interface doesn't exist

			// check if the interface is a system interface
			// non-virtual interfaces can't be created
			bool system_interface = false;
			error = check_system_interface(name, &system_interface);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "check_system_interface error");
				error = -1;
				goto out;
			}

			if (system_interface || strcmp(type, "eth") == 0 || strcmp(type, "lo") == 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "Can't create non-virtual interface %s of type: %s", name, type);
				error = -1;
				goto out;
			}

			// don't create if it's a QinQ vlan interface since it's already been created
			if (second_vlan_id == 0) {
				// update the last-change state list
				uint8_t state = rtnl_link_get_operstate(request);

				if_state_list_add(&if_state_changes, state, name);

				// set the new name
				rtnl_link_set_name(request, name);

				// add the interface
				// note: if type is not set, you can't add the new link
				error = rtnl_link_add(socket, request, NLM_F_CREATE);
				if (error != 0) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_add error (%d): %s", error, nl_geterror(error));
					goto out;
				}
			}

			// in order to add ipv4/ipv6 options we first have to create the interface
			// and then get its interface index, because we need it inside add_interface_ipv4/ipv6
			// and don't want to set it manually...
			nl_cache_free(cache);
			error = rtnl_link_alloc_cache(socket, AF_UNSPEC, &cache);
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_alloc_cache error (%d): %s", error, nl_geterror(error));
				goto out;
			}

			old = rtnl_link_get_by_name(cache, name);
			old_vlan_qinq = rtnl_link_get_by_name(cache, second_vlan_name);

			if (old != NULL) {
				error = add_interface_ipv4(&ld->links[i], old, request, rtnl_link_get_ifindex(old));
				if (error != 0) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "add_interface_ipv4 error");
					goto out;
				}
				// ipv4 config
				error = rtnl_link_change(socket, old, request, 0);
				if (error != 0) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_change error (%d): %s", error, nl_geterror(error));
					goto out;
				}
				error = add_interface_ipv6(&ld->links[i], old, request, rtnl_link_get_ifindex(old));
				if (error != 0) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "add_interface_ipv6 error");
					goto out;
				}

				// ipv6 config
				error = rtnl_link_change(socket, old, request, 0);
				if (error != 0) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_change error (%d): %s", error, nl_geterror(error));
					goto out;
				}
			}

			if (old_vlan_qinq != NULL) {
				error = add_interface_ipv4(&ld->links[i], old_vlan_qinq, request, rtnl_link_get_ifindex(old_vlan_qinq));
				if (error != 0) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "add_interface_ipv4 error");
					goto out;
				}

				error = add_interface_ipv6(&ld->links[i], old_vlan_qinq, request, rtnl_link_get_ifindex(old_vlan_qinq));
				if (error != 0) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "add_interface_ipv6 error");
					goto out;
				}

				error = rtnl_link_change(socket, old_vlan_qinq, request, 0);
				if (error != 0) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_change error (%d): %s", error, nl_geterror(error));
					goto out;
				}
			}
		}
		rtnl_link_put(old);
		rtnl_link_put(old_vlan_qinq);
		rtnl_link_put(request);
	}

out:
	nl_socket_free(socket);
	nl_cache_free(cache);

	return error;
}

static int remove_ipv4_address(ip_address_list_t *addr_list, struct nl_sock *socket, struct rtnl_link *old)
{
	int error = 0;
	uint32_t addr_count = addr_list->count;

	// iterate through list of IPv4 addresses and check delete flag
	for (uint32_t j = 0; j < addr_count; j++) {

		if (addr_list->addr[j].delete == true) {
			struct rtnl_addr *addr = rtnl_addr_alloc();
			struct nl_addr *local_addr = NULL;

			error = nl_addr_parse(addr_list->addr[j].ip, AF_INET, &local_addr);
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "nl_addr_parse error (%d): %s", error, nl_geterror(error));
				rtnl_addr_put(addr);
				nl_addr_put(local_addr);
				return -1;
			}
			int32_t if_index = rtnl_link_get_ifindex(old);

			nl_addr_set_prefixlen(local_addr, addr_list->addr[j].subnet);

			rtnl_addr_set_ifindex(addr, if_index);

			rtnl_addr_set_local(addr, local_addr);

			error = rtnl_addr_delete(socket, addr, 0);
			if (error < 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_addr_delete error (%d): %s", error, nl_geterror(error));
				rtnl_addr_put(addr);
				nl_addr_put(local_addr);
				return -1;
			}

			rtnl_addr_put(addr);
			nl_addr_put(local_addr);

			// remove this IP address from list
			ip_address_free(&addr_list->addr[j]);
		}
	}

	return 0;
}

int add_interface_ipv4(link_data_t *ld, struct rtnl_link *old, struct rtnl_link *req, int if_idx)
{
	int error = 0;
	const char *ipv4_base = "/proc/sys/net/ipv4/conf";
	char *if_name = ld->name;
	ipv4_data_t *ipv4 = &ld->ipv4;
	ip_address_list_t *addr_ls = &ipv4->addr_list;
	ip_neighbor_list_t *neigh_ls = &ipv4->nbor_list;
	struct nl_sock *socket = NULL;
	struct nl_addr *local_addr = NULL;
	struct rtnl_addr *r_addr = NULL;
	struct rtnl_neigh *neigh = NULL;
	struct nl_addr *ll_addr = NULL;
	struct nl_cache *cache = NULL;

	// add ipv4 options from given link data to the req link object
	// also set forwarding options to the given files for a particular link
	// enabled
	// TODO: fix this
	// note: commented out because there is no disable_ipv4 file on arch (need to find workaround)
	/*
	error = write_to_proc_file(ipv4_base, if_name, "disable_ipv4", ipv4->enabled == 0);
	if (error != 0) {
		goto out;
	}*/

	// forwarding
	error = write_to_proc_file(ipv4_base, if_name, "forwarding", ipv4->forwarding);
	if (error != 0) {
		goto out;
	}

	// set mtu
	if (ipv4->mtu != 0) {
		rtnl_link_set_mtu(req, ipv4->mtu);
	}

	// address list
	socket = nl_socket_alloc();
	if (socket == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nl_socket_alloc error: invalid socket");
		goto out;
	}

	if ((error = nl_connect(socket, NETLINK_ROUTE)) != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nl_connect error (%d): %s", error, nl_geterror(error));
		goto out;
	}

	for (uint i = 0; i < addr_ls->count; i++) {
		if (addr_ls->addr[i].ip == NULL) { // ip was deleted
			continue;
		}
		r_addr = rtnl_addr_alloc();
		error = nl_addr_parse(addr_ls->addr[i].ip, AF_INET, &local_addr);
		if (error != 0) {
			rtnl_addr_put(r_addr);
			nl_addr_put(local_addr);
			SRPLG_LOG_ERR(PLUGIN_NAME, "nl_addr_parse error (%d): %s", error, nl_geterror(error));
			goto out;
		}
		nl_addr_set_prefixlen(local_addr, addr_ls->addr[i].subnet);

		// configure rtln_addr for a link
		rtnl_addr_set_ifindex(r_addr, if_idx);
		rtnl_addr_set_local(r_addr, local_addr);

		// send message
		rtnl_addr_add(socket, r_addr, 0);

		// Free the memory
		nl_addr_put(local_addr);
		rtnl_addr_put(r_addr);
	}

	for (uint i = 0; i < neigh_ls->count; i++) {
		if (neigh_ls->nbor[i].ip == NULL) { // neighbor was deleted
			continue;
		}
		neigh = rtnl_neigh_alloc();

		error = nl_addr_parse(neigh_ls->nbor[i].ip, AF_INET, &local_addr);
		if (error != 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "nl_addr_parse error (%d): %s", error, nl_geterror(error));
			goto loop_end;
		}

		error = nl_addr_parse(neigh_ls->nbor[i].phys_addr, AF_LLC, &ll_addr);
		if (error != 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "nl_addr_parse error (%d): %s", error, nl_geterror(error));
			goto loop_end;
		}

		rtnl_neigh_set_ifindex(neigh, if_idx);

		// set dst and ll addr
		rtnl_neigh_set_lladdr(neigh, ll_addr);
		rtnl_neigh_set_dst(neigh, local_addr);

		error = rtnl_link_alloc_cache(socket, AF_UNSPEC, &cache);
		if (error != 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_alloc_cache error (%d): %s", error, nl_geterror(error));
			goto loop_end;
		}

		struct rtnl_neigh *tmp_neigh = rtnl_neigh_get(cache, if_idx, local_addr);
		int neigh_oper = NLM_F_CREATE;

		if (tmp_neigh != NULL) {
			// if the neighbor already exists, replace it
			// otherwise create it (NLM_F_CREATE)
			neigh_oper = NLM_F_REPLACE;
			rtnl_neigh_put(tmp_neigh);
		}

		error = rtnl_neigh_add(socket, neigh, neigh_oper);
		if (error != 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_neigh_add error (%d): %s", error, nl_geterror(error));
			goto loop_end;
		}

	loop_end:
		nl_cache_free(cache);
		nl_addr_put(ll_addr);
		nl_addr_put(local_addr);
		rtnl_neigh_put(neigh);
		if (error != 0) {
			break;
		}
	}

out:
	nl_socket_free(socket);
	return error;
}

static int remove_ipv6_address(ip_address_list_t *addr_list, struct nl_sock *socket, struct rtnl_link *old)
{
	int error = 0;
	uint32_t addr_count = addr_list->count;

	// iterate through list of IPv6 addresses and check delete flag
	if (addr_count > 0) {
		for (uint32_t j = 0; j < addr_count; j++) {

			if (addr_list->addr[j].delete == true) {
				struct rtnl_addr *addr = rtnl_addr_alloc();
				struct nl_addr *local_addr = NULL;

				error = nl_addr_parse(addr_list->addr[j].ip, AF_INET6, &local_addr);
				if (error != 0) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "nl_addr_parse error (%d): %s", error, nl_geterror(error));
					rtnl_addr_put(addr);
					nl_addr_put(local_addr);
					return -1;
				}
				int32_t if_index = rtnl_link_get_ifindex(old);

				nl_addr_set_prefixlen(local_addr, addr_list->addr[j].subnet);

				rtnl_addr_set_ifindex(addr, if_index);

				rtnl_addr_set_local(addr, local_addr);

				error = rtnl_addr_delete(socket, addr, 0);
				if (error < 0) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_addr_delete error (%d): %s", error, nl_geterror(error));
					rtnl_addr_put(addr);
					nl_addr_put(local_addr);
					return -1;
				}
				rtnl_addr_put(addr);
				nl_addr_put(local_addr);

				// remove this IP address from list
				ip_address_free(&addr_list->addr[j]);
			}
		}
	}

	return 0;
}

int add_interface_ipv6(link_data_t *ld, struct rtnl_link *old, struct rtnl_link *req, int if_idx)
{
	int error = 0;
	const char *ipv6_base = "/proc/sys/net/ipv6/conf";
	char *if_name = ld->name;
	ipv6_data_t *ipv6 = &ld->ipv6;
	ip_address_list_t *addr_ls = &ipv6->ip_data.addr_list;
	ip_neighbor_list_t *neigh_ls = &ipv6->ip_data.nbor_list;
	struct nl_sock *socket = NULL;
	struct nl_addr *local_addr = NULL;
	struct rtnl_addr *r_addr = NULL;
	struct rtnl_neigh *neigh = NULL;
	struct nl_addr *ll_addr = NULL;
	struct nl_cache *cache = NULL;

	// enabled
	error = write_to_proc_file(ipv6_base, if_name, "disable_ipv6", ipv6->ip_data.enabled == 0);
	if (error != 0) {
		goto out;
	}

	// forwarding
	error = write_to_proc_file(ipv6_base, if_name, "forwarding", ipv6->ip_data.forwarding);
	if (error != 0) {
		goto out;
	}

	// set mtu
	if (ipv6->ip_data.mtu != 0) {
		// If the new ipv6 MTU value is greater than link (ipv4) MTU, the
		// kernel will return EINVAL when attempting to write
		// the value to the MTU proc file.
		if (ipv6->ip_data.mtu > ld->ipv4.mtu) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "Attempted to set ipv6 MTU value (%hd) greater than the current ipv4 MTU value (%hd) on interface: %s.", ipv6->ip_data.mtu, ld->ipv4.mtu, ld->name);
			error = -1;
			goto out;
		}
		error = write_to_proc_file(ipv6_base, if_name, "mtu", ipv6->ip_data.mtu);
		if (error != 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "write_to_proc_file error (mtu)");
			goto out;
		}
	}

	// address list
	socket = nl_socket_alloc();
	if (socket == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nl_socket_alloc error: invalid socket");
		goto out;
	}

	if ((error = nl_connect(socket, NETLINK_ROUTE)) != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nl_connect error (%d): %s", error, nl_geterror(error));
		goto out;
	}

	for (uint i = 0; i < addr_ls->count; i++) {
		if (addr_ls->addr[i].ip == NULL) { // ip was deleted
			continue;
		}

		r_addr = rtnl_addr_alloc();

		error = nl_addr_parse(addr_ls->addr[i].ip, AF_INET6, &local_addr);
		if (error != 0) {
			rtnl_addr_put(r_addr);
			nl_addr_put(local_addr);
			goto out;
		}
		nl_addr_set_prefixlen(local_addr, addr_ls->addr[i].subnet);

		// configure rtln_addr for a link
		rtnl_addr_set_ifindex(r_addr, if_idx);
		rtnl_addr_set_local(r_addr, local_addr);

		if (ipv6->autoconf.temp_valid_lifetime != 0) {
			rtnl_addr_set_valid_lifetime(r_addr, ipv6->autoconf.temp_valid_lifetime);
		}

		if (ipv6->autoconf.temp_preffered_lifetime != 0) {
			rtnl_addr_set_preferred_lifetime(r_addr, ipv6->autoconf.temp_preffered_lifetime);
		}

		// send message
		rtnl_addr_add(socket, r_addr, 0);

		// Free the memory
		nl_addr_put(local_addr);
		rtnl_addr_put(r_addr);
	}

	for (uint i = 0; i < neigh_ls->count; i++) {
		if (neigh_ls->nbor[i].ip == NULL) { // neighbor was deleted
			continue;
		}
		neigh = rtnl_neigh_alloc();

		error = nl_addr_parse(neigh_ls->nbor[i].ip, AF_INET6, &local_addr);
		if (error != 0) {
			goto loop_end;
		}

		error = nl_addr_parse(neigh_ls->nbor[i].phys_addr, AF_LLC, &ll_addr);
		if (error != 0) {
			goto loop_end;
		}

		rtnl_neigh_set_ifindex(neigh, if_idx);

		// set dst and ll addr
		rtnl_neigh_set_lladdr(neigh, ll_addr);
		rtnl_neigh_set_dst(neigh, local_addr);

		error = rtnl_link_alloc_cache(socket, AF_UNSPEC, &cache);
		if (error != 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_alloc_cache error (%d): %s", error, nl_geterror(error));
			goto loop_end;
		}

		struct rtnl_neigh *tmp_neigh = rtnl_neigh_get(cache, if_idx, local_addr);
		int neigh_oper = NLM_F_CREATE;

		if (tmp_neigh != NULL) {
			// if the neighbor already exists, replace it
			// otherwise create it (NLM_F_CREATE)
			neigh_oper = NLM_F_REPLACE;
			rtnl_neigh_put(tmp_neigh);
		}

		error = rtnl_neigh_add(socket, neigh, neigh_oper);
		if (error != 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_neigh_add error (%d): %s", error, nl_geterror(error));
			goto loop_end;
		}

	loop_end:
		nl_cache_free(cache);
		nl_addr_put(ll_addr);
		nl_addr_put(local_addr);
		rtnl_neigh_put(neigh);

		if (error != 0) {
			break;
		}
	}
out:
	nl_socket_free(socket);
	return error;
}

static int remove_neighbors(ip_neighbor_list_t *nbor_list, struct nl_sock *socket, int addr_ver, int if_index)
{
	int error = 0;
	// Iterate through list of neighbors and check delete flag
	for (uint32_t i = 0; i < nbor_list->count; i++) {

		if (nbor_list->nbor[i].delete == true) {
			// Allocate an empty neighbour object to be filled out with the attributes
			// matching the neighbour to be deleted. Alternatively a fully equipped
			// neighbour object out of a cache can be used instead.
			struct nl_addr *dst_addr = NULL;
			struct rtnl_neigh *neigh = rtnl_neigh_alloc();

			error = nl_addr_parse(nbor_list->nbor[i].ip, addr_ver, &dst_addr);
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "nl_addr_parse error (%d): %s", error, nl_geterror(error));
				nl_addr_put(dst_addr);
				rtnl_neigh_put(neigh);
				return -1;
			}
			// Neighbours are uniquely identified by their interface index and
			// destination address, you may fill out other attributes but they
			// will have no influence.
			rtnl_neigh_set_ifindex(neigh, if_index);
			rtnl_neigh_set_dst(neigh, dst_addr);

			// Build the netlink message and send it to the kernel, the operation will
			// block until the operation has been completed. Alternatively the required
			// netlink message can be built using rtnl_neigh_build_delete_request()
			// to be sent out using nl_send_auto_complete().
			error = rtnl_neigh_delete(socket, neigh, NLM_F_ACK);
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_neigh_delete error (%d): %s", error, nl_geterror(error));
				nl_addr_put(dst_addr);
				rtnl_neigh_put(neigh);
				return -1;
			}

			// Free the memory
			nl_addr_put(dst_addr);
			rtnl_neigh_put(neigh);

			// Remove neighbor from list
			ip_neighbor_free(&nbor_list->nbor[i]);
		}
	}

	return 0;
}

static int create_vlan_qinq(struct nl_sock *socket, struct nl_cache *cache, char *second_vlan_name, char *name, char *parent_interface, uint16_t outer_vlan_id, uint16_t second_vlan_id)
{
	int error = 0;
	struct rtnl_link *outer_tag_link = rtnl_link_alloc();
	struct rtnl_link *second_tag_link = rtnl_link_alloc();

	// create virtual link for outer tag
	error = rtnl_link_set_type(outer_tag_link, "vlan");
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_set_type error (%d): %s", error, nl_geterror(error));
		goto out;
	}
	rtnl_link_set_name(outer_tag_link, name);
	// set parent interface
	int parent_index = rtnl_link_name2i(cache, parent_interface);
	rtnl_link_set_link(outer_tag_link, parent_index);
	// set protocol to 802.1ad (QinQ)
	error = rtnl_link_vlan_set_protocol(outer_tag_link, htons(ETH_P_8021AD));
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_vlan_set_protocol error (%d): %s", error, nl_geterror(error));
		goto out;
	}
	// set outer vlan id (s-tag)
	error = rtnl_link_vlan_set_id(outer_tag_link, outer_vlan_id);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_vlan_set_id error (%d): %s", error, nl_geterror(error));
		goto out;
	}
	error = rtnl_link_add(socket, outer_tag_link, NLM_F_CREATE);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_add error (%d): %s", error, nl_geterror(error));
		goto out;
	}

	// create virtual link for second tag
	error = rtnl_link_set_type(second_tag_link, "vlan");
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_set_type error (%d): %s", error, nl_geterror(error));
		goto out;
	}
	rtnl_link_set_name(second_tag_link, second_vlan_name);
	// retrieve outer_tag_link struct from the kernel so that it contains the correct ifindex
	rtnl_link_put(outer_tag_link);
	outer_tag_link = NULL;
	error = rtnl_link_get_kernel(socket, 0, name, &outer_tag_link);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_get_kernel error (%d): %s", error, nl_geterror(error));
		goto out;
	}
	// set outer_tag_link as the parent of second_tag_link to ensure it
	// will be deleted automatically when outer_tag_link is deleted
	rtnl_link_set_link(second_tag_link, rtnl_link_get_ifindex(outer_tag_link));
	// set second vlan id (c-tag)
	error = rtnl_link_vlan_set_id(second_tag_link, second_vlan_id);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_vlan_set_id error (%d): %s", error, nl_geterror(error));
		goto out;
	}
	error = rtnl_link_add(socket, second_tag_link, NLM_F_CREATE);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_add error (%d): %s", error, nl_geterror(error));
		goto out;
	}
out:
	rtnl_link_put(outer_tag_link);
	rtnl_link_put(second_tag_link);
	return error;
}

static bool check_system_interface(const char *interface_name, bool *system_interface)
{
	int error = 0;
	char *all_devices_cmd = "ls /sys/class/net";
	char *check_system_devices_cmd = "ls -l /sys/class/net";
	char line[CLASS_NET_LINE_LEN] = {0};
	int all_cnt = 0;
	int sys_cnt = 0;
	char *all_interfaces[LD_MAX_LINKS] = {0};
	char *system_interfaces[LD_MAX_LINKS] = {0};
	FILE *system_interface_check = NULL;

	system_interface_check = popen(all_devices_cmd, "r");
	if (system_interface_check == NULL) {
		SRPLG_LOG_WRN(PLUGIN_NAME, "could not execute %s", all_devices_cmd);
		*system_interface = false;
		error = -1;
		goto out;
	}

	// get all interfaces from /sys/class/net
	while (fgets(line, sizeof(line), system_interface_check) != NULL) {
		// remove newline char from line
		line[strlen(line) - 1] = '\0';
		all_interfaces[all_cnt] = strndup(line, strlen(line) + 1);
		all_cnt++;
	}

	pclose(system_interface_check);
	// reset everything to reuse it
	system_interface_check = NULL;
	memset(line, 0, CLASS_NET_LINE_LEN);

	system_interface_check = popen(check_system_devices_cmd, "r");
	if (system_interface_check == NULL) {
		SRPLG_LOG_WRN(PLUGIN_NAME, "could not execute %s", check_system_devices_cmd);
		*system_interface = false;
		error = -1;
		goto out;
	}

	// check if an interface is virtual or system
	while (fgets(line, sizeof(line), system_interface_check) != NULL) {
		// loopback device is virtual but handle it as a physical device here
		// because libnl won't let us delete it
		if (strstr(line, "/lo") != NULL ||
			(strstr(line, "/virtual/") == NULL &&
			 strncmp(line, "total", strlen("total") != 0))) {
			// this is a system interface

			// add it to system_interfaces
			for (int i = 0; i < LD_MAX_LINKS; i++) {
				if (all_interfaces[i] != 0) {
					if (strstr(line, all_interfaces[i]) != NULL) {
						system_interfaces[sys_cnt] = strndup(all_interfaces[i], strlen(all_interfaces[i]) + 1);
						sys_cnt++;
						break;
					}
				}
			}
		}
		memset(line, 0, CLASS_NET_LINE_LEN);
	}

	for (int i = 0; i < LD_MAX_LINKS; i++) {
		if (system_interfaces[i] != 0) {
			if (strcmp(interface_name, system_interfaces[i]) == 0) {
				*system_interface = true;
				break;
			}
		}
	}

	// cleanup
	for (int i = 0; i < LD_MAX_LINKS; i++) {
		if (system_interfaces[i] != 0) {
			FREE_SAFE(system_interfaces[i]);
		}

		if (all_interfaces[i] != 0) {
			FREE_SAFE(all_interfaces[i]);
		}
	}

out:
	if (system_interface_check) {
		pclose(system_interface_check);
	}

	return error;
}

int write_to_proc_file(const char *dir_path, char *interface, const char *fn, int val)
{
	int error = 0;
	char tmp_buffer[PATH_MAX];
	FILE *fptr = NULL;

	error = snprintf(tmp_buffer, sizeof(tmp_buffer), "%s/%s/%s", dir_path, interface, fn);
	if (error < 0) {
		// snprintf error
		SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf failed");
		goto out;
	}

	// snprintf returns return the number of bytes that are written
	// reset error to 0
	error = 0;

	fptr = fopen((const char *) tmp_buffer, "w");
	if (fptr == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "failed to open %s: %s", tmp_buffer, strerror(errno));
		error = -1;
		goto out;
	}

	error = fprintf(fptr, "%d", val);
	if (error < 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "fprintf error: %s", strerror(errno));
		error = -1;
		goto out;
	}

	error = 0;
	error = fclose(fptr);
	if (error != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "fclose error: %s", strerror(errno));
		error = -1;
		goto out;
	}
out:
	return error;
}

static int read_from_proc_file(const char *dir_path, char *interface, const char *fn, int *val)
{
	int error = 0;
	char tmp_buffer[PATH_MAX];
	FILE *fptr = NULL;
	char val_str[20] = {0};

	error = snprintf(tmp_buffer, sizeof(tmp_buffer), "%s/%s/%s", dir_path, interface, fn);
	if (error < 0) {
		// snprintf error
		SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf failed");
		goto out;
	}

	// snprintf returns return the number of bytes that are written
	// reset error to 0
	error = 0;
	fptr = fopen((const char *) tmp_buffer, "r");
	if (fptr == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "failed to open %s: %s", tmp_buffer, strerror(errno));
		error = -1;
		goto out;
	}

	char *s = fgets(val_str, sizeof(val_str), fptr);
	if (s == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "fgets error: %s", strerror(errno));
		error = -1;
		goto out;
	}

	errno = 0;
	*val = (int) strtol(val_str, NULL, 10);
	if (errno != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "strtol error: %s", strerror(errno));
		error = -1;
		goto out;
	}

	error = fclose(fptr);
	if (error != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "fclose error: %s", strerror(errno));
		error = -1;
		goto out;
	}

out:
	return error;
}

static int read_interface_type_from_sys_file(const char *dir_path, char *interface, int *val)
{
	int error = 0;
	char tmp_buffer[PATH_MAX];
	FILE *fptr = NULL;
	char val_str[20] = {0};

	error = snprintf(tmp_buffer, sizeof(tmp_buffer), "%s/%s/type", dir_path, interface);
	if (error < 0) {
		// snprintf error
		SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf failed");
		goto out;
	}

	// snprintf returns return the number of bytes that are written
	// reset error to 0
	error = 0;
	fptr = fopen((const char *) tmp_buffer, "r");
	if (fptr == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "failed to open %s: %s", tmp_buffer, strerror(errno));
		error = -1;
		goto out;
	}

	char *s = fgets(val_str, sizeof(val_str), fptr);
	if (s == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "fgets error: %s", strerror(errno));
		error = -1;
		goto out;
	}

	errno = 0;
	*val = (int) strtol(val_str, NULL, 10);
	if (errno != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "strtol error: %s", strerror(errno));
		error = -1;
		goto out;
	}

	error = fclose(fptr);
	if (error != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "fclose error: %s", strerror(errno));
		error = -1;
		goto out;
	}

out:
	return error;
}

int add_existing_links(sr_session_ctx_t *session, link_data_list_t *ld)
{
	int error = 0;
	struct nl_sock *socket = NULL;
	struct nl_cache *cache = NULL;
	struct rtnl_link *link = NULL;
	struct nl_cache *addr_cache = NULL;
	struct nl_cache *neigh_cache = NULL;
	struct rtnl_addr *addr = {0};
	char *name = NULL;
	char *description = NULL;
	char *type = NULL;
	char *enabled = NULL;
	char *parent_interface = NULL;
	uint16_t vlan_id = 0;
	unsigned int mtu = 0;
	char tmp_buffer[10] = {0};
	char parent_buffer[MAX_IF_NAME_LEN] = {0};
	int addr_family = 0;
	char addr_str[ADDR_STR_BUF_SIZE];
	char dst_addr_str[ADDR_STR_BUF_SIZE];
	char ll_addr_str[ADDR_STR_BUF_SIZE];
	char command_buffer[100] = {0};

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

	link = (struct rtnl_link *) nl_cache_get_first(cache);

	while (link != NULL) {
		// reset parent interface
		parent_interface = NULL;

		name = rtnl_link_get_name(link);
		if (name == NULL) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_get_name error");
			goto error_out;
		}

		error = get_interface_description(session, name, &description);
		if (error != 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "get_interface_description error");
			goto error_out;
		}

		// address origin
		snprintf(command_buffer, sizeof(command_buffer), "ip a show %s", name);

		type = rtnl_link_get_type(link);
		if (type == NULL) {
			/* rtnl_link_get_type() will return NULL for interfaces that were not
			 * set with rtnl_link_set_type()
			 *
			 * get the type from: /sys/class/net/<interface_name>/type
			 */
			const char *path_to_sys = "/sys/class/net/";
			int type_id = 0;

			error = read_interface_type_from_sys_file(path_to_sys, name, &type_id);
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "read_interface_type_from_sys_file error");
				goto error_out;
			}

			// values taken from: if_arp.h
			if (type_id == 1) {
				// eth interface
				type = "eth";
			} else if (type_id == 772) {
				// loopback interface
				type = "lo";
			}
		}

		// enabled
		uint8_t tmp_enabled = rtnl_link_get_operstate(link);
		// lo interface has state unknown, treat it as enabled
		// otherwise it will be set to down, and dns resolution won't work
		if (IF_OPER_UP == tmp_enabled || IF_OPER_UNKNOWN == tmp_enabled) {
			enabled = "true";
		} else if (IF_OPER_DOWN == tmp_enabled) {
			enabled = "false";
		}

		// vlan
		if (rtnl_link_is_vlan(link)) {
			// parent interface
			int parent_index = rtnl_link_get_link(link);
			parent_interface = rtnl_link_i2name(cache, parent_index, parent_buffer, MAX_IF_NAME_LEN);

			// outer vlan id
			vlan_id = (uint16_t) rtnl_link_vlan_get_id(link);
			if (vlan_id <= 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "couldn't get vlan ID");
				goto error_out;
			}

			// check if vlan_id in name, if it is this is the QinQ interface, skip it
			char *first = NULL;
			char *second = NULL;

			first = strchr(name, '.');
			second = strchr(first + 1, '.');

			if (second != 0) {
				link = (struct rtnl_link *) nl_cache_get_next((struct nl_object *) link);
				continue;
			}
		}

		error = link_data_list_add(ld, name);
		if (error != 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "link_data_list_add error");
			goto error_out;
		}

		if (description != NULL) {
			error = link_data_list_set_description(ld, name, description);
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "link_data_list_set_description error");
				goto error_out;
			}
		}

		if (type != NULL) {
			error = link_data_list_set_type(ld, name, type);
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "link_data_list_set_type error");
				goto error_out;
			}
		}

		error = link_data_list_set_enabled(ld, name, enabled);
		if (error != 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "link_data_list_set_enabled error");
			goto error_out;
		}

		if (parent_interface != 0) {
			error = link_data_list_set_parent(ld, name, parent_interface);
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "link_data_list_set_parent error");
				goto error_out;
			}
		}

		if (vlan_id != 0) {
			error = link_data_list_set_outer_vlan_id(ld, name, vlan_id);
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "link_data_list_set_outer_vlan_id error");
				goto error_out;
			}
		}

		// get ipv4 mtu
		mtu = rtnl_link_get_mtu(link);

		snprintf(tmp_buffer, sizeof(tmp_buffer), "%u", mtu);

		if (mtu > 0) {
			error = link_data_list_set_ipv4_mtu(&link_data_list, name, tmp_buffer);
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "link_data_list_set_ipv4_mtu error (%d) : %s", error, strerror(error));
				goto error_out;
			}
		}

		// get ipv6 mtu
		const char *ipv6_base = "/proc/sys/net/ipv6/conf";
		int ipv6_mtu = 0;

		error = read_from_proc_file(ipv6_base, name, "mtu", &ipv6_mtu);
		if (error != 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "read_from_proc_file error (%d) : %s", error, strerror(error));
			goto error_out;
		}

		snprintf(tmp_buffer, sizeof(tmp_buffer), "%u", ipv6_mtu);

		if (ipv6_mtu > 0) {
			error = link_data_list_set_ipv6_mtu(&link_data_list, name, tmp_buffer);
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "link_data_list_set_ipv6_mtu error (%d) : %s", error, strerror(error));
				goto error_out;
			}
		}

		int if_index = rtnl_link_get_ifindex(link);

		// neighbors
		error = rtnl_neigh_alloc_cache(socket, &neigh_cache);
		if (error != 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_neigh_alloc_cache error (%d): %s", error, nl_geterror(error));
			goto error_out;
		}

		int neigh_count = nl_cache_nitems(neigh_cache);

		struct nl_object *nl_neigh_object;
		nl_neigh_object = nl_cache_get_first(neigh_cache);

		for (int i = 0; i < neigh_count; i++) {
			struct nl_addr *nl_dst_addr = rtnl_neigh_get_dst((struct rtnl_neigh *) nl_neigh_object);

			char *dst_addr = nl_addr2str(nl_dst_addr, dst_addr_str, sizeof(dst_addr_str));
			if (dst_addr == NULL) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "nl_addr2str error");
				goto error_out;
			}

			struct rtnl_neigh *neigh = rtnl_neigh_get(neigh_cache, if_index, nl_dst_addr);

			if (neigh != NULL) {
				// get neigh state
				int neigh_state = rtnl_neigh_get_state(neigh);

				// skip neighs with no arp state
				if (NUD_NOARP == neigh_state) {
					nl_neigh_object = nl_cache_get_next(nl_neigh_object);
					rtnl_neigh_put(neigh);
					continue;
				}

				int cur_neigh_index = rtnl_neigh_get_ifindex(neigh);

				if (if_index != cur_neigh_index) {
					nl_neigh_object = nl_cache_get_next(nl_neigh_object);
					rtnl_neigh_put(neigh);
					continue;
				}

				struct nl_addr *ll_addr = rtnl_neigh_get_lladdr(neigh);

				char *ll_addr_s = nl_addr2str(ll_addr, ll_addr_str, sizeof(ll_addr_str));
				if (NULL == ll_addr_s) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "nl_addr2str error");
					rtnl_neigh_put(neigh);
					goto error_out;
				}

				// check if ipv4 or ipv6
				addr_family = rtnl_neigh_get_family(neigh);

				if (addr_family == AF_INET) {
					error = link_data_list_add_ipv4_neighbor(&link_data_list, name, dst_addr, ll_addr_s);
					if (error != 0) {
						SRPLG_LOG_ERR(PLUGIN_NAME, "link_data_list_add_ipv4_neighbor error (%d) : %s", error, strerror(error));
						rtnl_neigh_put(neigh);
						goto error_out;
					}
				} else if (addr_family == AF_INET6) {
					error = link_data_list_add_ipv6_neighbor(&link_data_list, name, dst_addr, ll_addr_s);
					if (error != 0) {
						SRPLG_LOG_ERR(PLUGIN_NAME, "link_data_list_add_ipv6_neighbor error (%d) : %s", error, strerror(error));
						rtnl_neigh_put(neigh);
						goto error_out;
					}
				}
				rtnl_neigh_put(neigh);
			}
			nl_neigh_object = nl_cache_get_next(nl_neigh_object);
		}

		nl_cache_free(neigh_cache);
		neigh_cache = NULL;

		error = rtnl_addr_alloc_cache(socket, &addr_cache);
		if (error != 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_addr_alloc_cache error (%d): %s", error, nl_geterror(error));
			goto error_out;
		}

		// get ipv4 and ipv6 addresses
		int addr_count = nl_cache_nitems(addr_cache);

		struct nl_object *nl_object;
		nl_object = nl_cache_get_first(addr_cache);

		addr = (struct rtnl_addr *) nl_object;

		for (int i = 0; i < addr_count; i++) {
			struct nl_addr *nl_addr_local = rtnl_addr_get_local(addr);
			if (nl_addr_local == NULL) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_addr_get_local error");
				goto error_out;
			}

			int cur_if_index = rtnl_addr_get_ifindex(addr);

			if (if_index != cur_if_index) {
				nl_object = nl_cache_get_next(nl_object);
				addr = (struct rtnl_addr *) nl_object;
				continue;
			}

			const char *addr_s = nl_addr2str(nl_addr_local, addr_str, sizeof(addr_str));
			if (NULL == addr_s) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "nl_addr2str error");
				goto error_out;
			}

			char *str = xstrdup(addr_s);

			// get address
			char *token = strtok(str, "/");
			if (token == NULL) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "couldn't parse ip address");

				FREE_SAFE(str);
				goto error_out;
			}

			char *address = xstrdup(token);

			char flags_buffer[100] = {0};
			rtnl_addr_flags2str((int) rtnl_addr_get_flags(addr), flags_buffer, sizeof(flags_buffer));

			SRPLG_LOG_DBG(PLUGIN_NAME, "Address = %s, Scope = %d, Flags = %s", address, rtnl_addr_get_flags(addr) & IFA_F_HOMEADDRESS, flags_buffer);

			// get subnet
			token = strtok(NULL, "/");
			if (token == NULL) {
				// the address exists
				// skip it
				// we didn't add this address
				// e.g.: ::1
				FREE_SAFE(str);
				FREE_SAFE(address);
				continue;
			}

			char *subnet = xstrdup(token);

			// check if ipv4 or ipv6
			addr_family = rtnl_addr_get_family(addr);

			if (addr_family == AF_INET) {
				// ipv4
				error = link_data_list_add_ipv4_address(&link_data_list, name, address, subnet, ip_subnet_type_prefix_length, (int) rtnl_addr_get_flags(addr));
				if (error != 0) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "link_data_list_add_ipv4_address error (%d) : %s", error, strerror(error));

					FREE_SAFE(str);
					FREE_SAFE(address);
					FREE_SAFE(subnet);
					goto error_out;
				}

				// enabled
				const char *ipv4_base = "/proc/sys/net/ipv4/conf";
				// TODO: figure out how to enable/disable ipv4
				//		since disable_ipv4 doesn't exist in /proc/sys/net/ipv6/conf/interface_name

				// forwarding
				int ipv4_forwarding = 0;

				error = read_from_proc_file(ipv4_base, name, "forwarding", &ipv4_forwarding);
				if (error != 0) {
					FREE_SAFE(str);
					FREE_SAFE(address);
					FREE_SAFE(subnet);
					goto error_out;
				}

				error = link_data_list_set_ipv4_forwarding(&link_data_list, name, ipv4_forwarding == 0 ? "false" : "true");
				if (error != 0) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "link_data_list_set_ipv4_forwarding error (%d) : %s", error, strerror(error));

					FREE_SAFE(str);
					FREE_SAFE(address);
					FREE_SAFE(subnet);
					goto error_out;
				}

			} else if (addr_family == AF_INET6) {
				// ipv6
				error = link_data_list_add_ipv6_address(&link_data_list, name, address, subnet, (int) rtnl_addr_get_flags(addr));
				if (error != 0) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "link_data_list_add_ipv6_address error (%d) : %s", error, strerror(error));

					FREE_SAFE(str);
					FREE_SAFE(address);
					FREE_SAFE(subnet);
					goto error_out;
				}

				// enabled
				ipv6_base = "/proc/sys/net/ipv6/conf";

				int ipv6_enabled = 0;

				error = read_from_proc_file(ipv6_base, name, "disable_ipv6", &ipv6_enabled);
				if (error != 0) {
					FREE_SAFE(str);
					FREE_SAFE(address);
					FREE_SAFE(subnet);
					goto error_out;
				}
				// since we check the value of 'disable_ipv6' file, the ipv6_enabled should be reversed
				error = link_data_list_set_ipv6_enabled(&link_data_list, name, ipv6_enabled == 0 ? "true" : "false");
				if (error != 0) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "link_data_list_set_ipv6_enabled error (%d) : %s", error, strerror(error));

					FREE_SAFE(str);
					FREE_SAFE(address);
					FREE_SAFE(subnet);
					goto error_out;
				}

				// forwarding
				int ipv6_forwarding = 0;

				error = read_from_proc_file(ipv6_base, name, "forwarding", &ipv6_forwarding);
				if (error != 0) {
					FREE_SAFE(str);
					FREE_SAFE(address);
					FREE_SAFE(subnet);
					goto error_out;
				}

				error = link_data_list_set_ipv6_forwarding(&link_data_list, name, ipv6_forwarding == 0 ? "false" : "true");
				if (error != 0) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "link_data_list_set_ipv6_forwarding error (%d) : %s", error, strerror(error));

					FREE_SAFE(str);
					FREE_SAFE(address);
					FREE_SAFE(subnet);
					goto error_out;
				}
			}

			nl_object = nl_cache_get_next(nl_object);
			addr = (struct rtnl_addr *) nl_object;

			FREE_SAFE(str);
			FREE_SAFE(address);
			FREE_SAFE(subnet);
		}
		nl_cache_free(addr_cache);
		addr_cache = NULL;

		link = (struct rtnl_link *) nl_cache_get_next((struct nl_object *) link);

		if (description != NULL) { // it was allocated in get_interface_description
			FREE_SAFE(description);
		}
	}

	rtnl_link_put(link);

	nl_socket_free(socket);
	nl_cache_free(cache);

	return 0;

error_out:
	if (socket != NULL) {
		nl_socket_free(socket);
	}

	if (link != NULL) {
		rtnl_link_put(link);
	}

	nl_cache_free(cache);

	if (addr_cache != NULL) {
		nl_cache_free(addr_cache);
	}

	if (description != NULL) {
		FREE_SAFE(description);
	}

	return -1;
}

static int get_interface_description(sr_session_ctx_t *session, char *name, char **description)
{
	int error = SR_ERR_OK;
	char path_buffer[PATH_MAX] = {0};
	sr_val_t *val = NULL;

	// conjure description path for this interface
	// /ietf-interfaces:interfaces/interface[name='test_interface']/description
	error = snprintf(path_buffer, sizeof(path_buffer) / sizeof(char), "%s[name=\"%s\"]/description", INTERFACE_LIST_YANG_PATH, name);
	if (error < 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf error");
		goto error_out;
	}

	// get the interface description value
	error = sr_get_item(session, path_buffer, 0, &val);
	if (error != SR_ERR_OK) {
		SRPLG_LOG_INF(PLUGIN_NAME, "interface description is not yet present in the datastore");
	} else if (strlen(val->data.string_val) > 0) {
		*description = xstrdup(val->data.string_val);
	}

	sr_free_values(val, 1);

	return 0;

error_out:
	return -1;
}

/*
 * function: create_node_neighbor_origin
 * -------------------------------------
 * creates a neighbor <origin> node: "static", "dynamic" or "other" 
 *
 * @arg ip_addr
 * 	neighbor destination address
 * 	
 * @arg if_index
 * 	interface index
 *
 * @arg family
 * 	address family AF_INET OR AF_INET6
 *
 * @returns:
 *      0 on successful neighbor-origin node creation, -1 on error
 */
int create_node_neighbor_origin(struct lyd_node **parent, const struct ly_ctx *ly_ctx, char *interface_name,
								struct nl_sock *socket, int32_t if_index, char *ip_addr, int family)
{
	struct nl_cache *cache = NULL;
	struct nl_addr *dst_addr = NULL;
	struct rtnl_neigh *neigh = NULL;
	char xpath_buffer[PATH_MAX];

	int state = -1;
	char *origin = NULL;

	int error = -1;
	int rc = -1;

	error = rtnl_neigh_alloc_cache(socket, &cache);
	if (error < 0) {
		goto error;
	}

	error = nl_addr_parse(ip_addr, family, &dst_addr);
	if (error != 0) {
		goto error;
	}

	neigh = rtnl_neigh_get(cache, if_index, dst_addr);
	if (neigh == NULL) {
		goto error;
	}

	// TODO: discern dynamic/static/other neighbor origin, currently only dynamic/static are used
	state = rtnl_neigh_get_state(neigh);
	if (state == -1) {
		goto error;
	}

	// since state is a bit mask
	// NUD_PERMANENT signifies a static entry
	origin = state & NUD_PERMANENT ? "static" : "dynamic";

	error = snprintf(xpath_buffer, PATH_MAX, "%s[name=\"%s\"]/ietf-ip:ipv%u/neighbor[ip='%s']/origin",
					 INTERFACE_LIST_YANG_PATH, interface_name, family == AF_INET6 ? 6 : 4, ip_addr);
	// null character not counted in written chars, therefore greater or equal than
	if (error < 0 || error >= PATH_MAX) {
		goto error;
	}

	// add neighbor origin node to the oper datastore, if successful show it (debug)
	error = lyd_new_path(*parent, ly_ctx, xpath_buffer, origin, LYD_ANYDATA_STRING, 0);
	if (error != LY_SUCCESS) {
		goto error;
	}

	SRPLG_LOG_DBG(PLUGIN_NAME, "%s = %s", xpath_buffer, origin);
	rc = 0;
	goto out;

error:
	SRPLG_LOG_ERR(PLUGIN_NAME, "create_node_neighbor_origin failed for address %s", ip_addr);
out:
	if (cache != NULL) {
		nl_cache_free(cache);
	}
	if (dst_addr != NULL) {
		nl_addr_put(dst_addr);
	}
	if (neigh) {
		rtnl_neigh_put(neigh);
	}

	return rc;
}

static int interfaces_state_data_cb(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *path, const char *request_xpath, uint32_t request_id, struct lyd_node **parent, void *private_data)
{
	int error = SR_ERR_OK;
	const struct ly_ctx *ly_ctx = NULL;
	struct nl_sock *socket = NULL;
	struct nl_cache *cache = NULL;
	struct rtnl_link *link = NULL;
	struct nl_addr *addr = NULL;
	struct rtnl_tc *tc = NULL;
	struct rtnl_qdisc *qdisc = NULL;
	if_state_t *tmp_ifs = NULL;
	struct tm *last_change = NULL;

	const char *OPER_STRING_MAP[] = {
		[IF_OPER_UNKNOWN] = "unknown",
		[IF_OPER_NOTPRESENT] = "not-present",
		[IF_OPER_DOWN] = "down",
		[IF_OPER_LOWERLAYERDOWN] = "lower-layer-down",
		[IF_OPER_TESTING] = "testing",
		[IF_OPER_DORMANT] = "dormant",
		[IF_OPER_UP] = "up",
	};

	if (*parent == NULL) {
		ly_ctx = sr_acquire_context(sr_session_get_connection(session));
		if (ly_ctx == NULL) {
			error = SR_ERR_CALLBACK_FAILED;
			goto error_out;
		}
		lyd_new_path(*parent, ly_ctx, request_xpath, NULL, 0, NULL);
	}

	// copy configuration data (subtree of the requested xpath)
	// from the running datastore in order to return it along with the state data
	// (merge it with the data tree which will be filled in the rest of this callback)
	error = sr_session_switch_ds(session, SR_DS_RUNNING);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_session_switch_ds error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}
	sr_data_t *running_ds_data = NULL;
	error = sr_get_subtree(session, request_xpath, 0, &running_ds_data);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_get_subtree error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}
	error = lyd_merge_tree(parent, running_ds_data->tree, 0);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "lyd_merge_tree error");
		goto error_out;
	}
	sr_release_data(running_ds_data);
	error = sr_session_switch_ds(session, SR_DS_OPERATIONAL); // switch back
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_session_switch_ds error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	socket = nl_socket_alloc();
	if (socket == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nl_socket_alloc error: invalid socket");
		goto error_out;
	}

	if ((error = nl_connect(socket, NETLINK_ROUTE)) != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nl_connect error (%d): %s", error, nl_geterror(error));
		goto error_out;
	}

	error = rtnl_link_alloc_cache(socket, AF_UNSPEC, &cache);
	if (error != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_alloc_cache error (%d): %s", error, nl_geterror(error));
		goto error_out;
	}

	char system_boot_time[DATETIME_BUF_SIZE] = {0};
	error = get_system_boot_time(system_boot_time);
	if (error != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "get_system_boot_time error: %s", strerror(errno));
		goto error_out;
	}

	link = (struct rtnl_link *) nl_cache_get_first(cache);
	qdisc = rtnl_qdisc_alloc();

	while (link != NULL) {
		// get tc and set the link
		tc = TC_CAST(qdisc);
		rtnl_tc_set_link(tc, link);

		char *interface_name = rtnl_link_get_name(link);

		// collect operational state information
		char *interface_oper_leaf_values[IF_OPER_LEAF_COUNT] = {NULL};

		// interface_data.link_up_down_trap_enable = ?
		// interface_data.admin_status = ?

		// oper-status
		interface_oper_leaf_values[IF_OPER_STATUS] = (char *) OPER_STRING_MAP[rtnl_link_get_operstate(link)];

		// last-change
		tmp_ifs = if_state_list_get_by_if_name(&if_state_changes, interface_name);
		last_change = (tmp_ifs->last_change != 0) ? localtime(&tmp_ifs->last_change) : NULL;
		char last_change_time[DATETIME_BUF_SIZE] = {0};
		// last-change -> only if changed at one point
		if (last_change != NULL) {
			// convert it to human readable format here
			strftime(last_change_time, sizeof(last_change_time), "%FT%TZ", last_change);
			interface_oper_leaf_values[IF_LAST_CHANGE] = last_change_time;
		} else {
			// default value of last-change should be system boot time
			interface_oper_leaf_values[IF_LAST_CHANGE] = system_boot_time;
		}

		// if-index
		char if_index[16] = {0};
		snprintf(if_index, sizeof(if_index), "%u", rtnl_link_get_ifindex(link));
		interface_oper_leaf_values[IF_IF_INDEX] = if_index;

		// phys-address
		addr = rtnl_link_get_addr(link);
		char phys_address[MAC_ADDR_MAX_LENGTH + 1] = {0};
		nl_addr2str(addr, phys_address, MAC_ADDR_MAX_LENGTH + 1);
		phys_address[MAC_ADDR_MAX_LENGTH] = 0;
		interface_oper_leaf_values[IF_PHYS_ADDRESS] = phys_address;

		// speed
		char speed[32] = {0};
		snprintf(speed, sizeof(speed), "%lu", rtnl_tc_get_stat(tc, RTNL_TC_RATE_BPS));
		interface_oper_leaf_values[IF_SPEED] = speed;

		// set interface state info in operational ds
		error = ds_oper_set_interface_info(*parent, ly_ctx, interface_name, interface_oper_leaf_values);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "ds_oper_set_interface_info error");
			goto error_out;
		}

		// collect operational statistics
		char *statistics[IF_STATS_LEAF_COUNT] = {NULL};

		// gather interface statistics that are not accessable via netlink
		nic_stats_t nic_stats = {0};
		error = get_nic_stats(interface_name, &nic_stats);
		if (error != 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "get_nic_stats error: %s", strerror(errno));
		}

		// discontinuity-time
		statistics[IF_STATS_DISCONTINUITY_TIME] = system_boot_time;

		// Rx
		// in-octets
		char in_octets[32] = {0};
		snprintf(in_octets, sizeof(in_octets), "%lu", rtnl_link_get_stat(link, RTNL_LINK_RX_BYTES));
		statistics[IF_STATS_IN_OCTETS] = in_octets;
		// in-broadcast-pkts
		char in_broadcast_pkts[32] = {0};
		snprintf(in_broadcast_pkts, sizeof(in_broadcast_pkts), "%lu", nic_stats.rx_broadcast);
		statistics[IF_STATS_IN_BROADCAST_PKTS] = in_broadcast_pkts;
		// in-multicast-pkts
		uint64_t in_multicast_pkts = rtnl_link_get_stat(link, RTNL_LINK_MULTICAST);
		char in_multicast_pkts_str[32] = {0};
		snprintf(in_multicast_pkts_str, sizeof(in_multicast_pkts_str), "%lu", in_multicast_pkts);
		statistics[IF_STATS_IN_MULTICAST_PKTS] = in_multicast_pkts_str;
		// in-unicast-pkts
		uint64_t in_unicast_pkts = nic_stats.rx_packets - nic_stats.rx_broadcast - in_multicast_pkts;
		char in_unicast_pkts_str[32] = {0};
		snprintf(in_unicast_pkts_str, sizeof(in_unicast_pkts_str), "%lu", in_unicast_pkts);
		statistics[IF_STATS_IN_UNICAST_PKTS] = in_unicast_pkts_str;
		// in-discards
		char in_discards[32] = {0};
		snprintf(in_discards, sizeof(in_discards), "%u", (uint32_t) rtnl_link_get_stat(link, RTNL_LINK_RX_DROPPED));
		statistics[IF_STATS_IN_DISCARDS] = in_discards;
		// in-errors
		char in_errors[32] = {0};
		snprintf(in_errors, sizeof(in_errors), "%u", (uint32_t) rtnl_link_get_stat(link, RTNL_LINK_RX_ERRORS));
		statistics[IF_STATS_IN_ERRORS] = in_errors;
		// in-unknown-protos
		char in_unknown_protos[32] = {0};
		snprintf(in_unknown_protos, sizeof(in_unknown_protos), "%u", (uint32_t) rtnl_link_get_stat(link, RTNL_LINK_IP6_INUNKNOWNPROTOS));
		statistics[IF_STATS_IN_UNKNOWN_PROTOS] = in_unknown_protos;

		// Tx
		// out-octets
		char out_octets[32] = {0};
		snprintf(out_octets, sizeof(out_octets), "%lu", rtnl_link_get_stat(link, RTNL_LINK_TX_BYTES));
		statistics[IF_STATS_OUT_OCTETS] = out_octets;
		// out-unicast-pkts
		char out_unicast_pkts[32] = {0};
		snprintf(out_unicast_pkts, sizeof(out_unicast_pkts), "%lu", nic_stats.tx_packets - nic_stats.tx_broadcast - nic_stats.tx_multicast);
		statistics[IF_STATS_OUT_UNICAST_PKTS] = out_unicast_pkts;
		// out-broadcast-pkts
		char out_broadcast_pkts[32] = {0};
		snprintf(out_broadcast_pkts, sizeof(out_broadcast_pkts), "%lu", nic_stats.tx_broadcast);
		statistics[IF_STATS_OUT_BROADCAST_PKTS] = out_broadcast_pkts;
		// out-multicast-pkts
		char out_multicast_pkts[32] = {0};
		snprintf(out_multicast_pkts, sizeof(out_multicast_pkts), "%lu", nic_stats.tx_multicast);
		statistics[IF_STATS_OUT_MULTICAST_PKTS] = out_multicast_pkts;
		// out-discards
		char out_discards[32] = {0};
		snprintf(out_discards, sizeof(out_discards), "%u", (uint32_t) rtnl_link_get_stat(link, RTNL_LINK_TX_DROPPED));
		statistics[IF_STATS_OUT_DISCARDS] = out_discards;
		// out-errors
		char out_errors[32] = {0};
		snprintf(out_errors, sizeof(out_errors), "%u", (uint32_t) rtnl_link_get_stat(link, RTNL_LINK_TX_ERRORS));
		statistics[IF_STATS_OUT_ERRORS] = out_errors;

		error = ds_oper_set_interface_statistics(*parent, ly_ctx, interface_name, statistics);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "ds_oper_set_interface_statistics error");
			goto error_out;
		}

		// find all interfaces which are layered on top of this interface
		// and update the operational datastore accordingly
		int32_t master_if_index = rtnl_link_get_master(link);
		struct rtnl_link *master_link = NULL;
		while (master_if_index) {
			master_link = rtnl_link_get(cache, master_if_index);
			char *master_name = rtnl_link_get_name(master_link);

			error = ds_oper_add_interface_higher_layer_if(*parent, ly_ctx, interface_name, master_name);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "ds_oper_add_interface_higher_layer_if error");
				goto error_out;
			}
			error = ds_oper_add_interface_lower_layer_if(*parent, ly_ctx, master_name, interface_name);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "ds_oper_add_interface_lower_layer_if error");
				goto error_out;
			}

			// go one layer higher
			master_if_index = rtnl_link_get_master(master_link);
		}

		link_data_t *l = data_list_get_by_name(&link_data_list, interface_name);
		if (l != NULL) {
			// set origin for ipv4 neighbors
			uint32_t ipv4_neigh_count = l->ipv4.nbor_list.count;
			for (uint32_t i = 0; i < ipv4_neigh_count; i++) {
				char *neigh_ip = l->ipv4.nbor_list.nbor[i].ip;
				error = create_node_neighbor_origin(parent, ly_ctx, interface_name, socket, rtnl_link_get_ifindex(link), neigh_ip, AF_INET);
				if (error < 0) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "create_node_neighbor_origin error (ipv4)");
					goto error_out;
				}
			}
			// set origin for ipv6 neighbors
			uint32_t ipv6_neigh_count = l->ipv6.ip_data.nbor_list.count;
			for (uint32_t i = 0; i < ipv6_neigh_count; i++) {
				char *neigh_ip = l->ipv6.ip_data.nbor_list.nbor[i].ip;
				error = create_node_neighbor_origin(parent, ly_ctx, interface_name, socket, rtnl_link_get_ifindex(link), neigh_ip, AF_INET6);
				if (error < 0) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "create_node_neighbor_origin error (ipv6)");
					goto error_out;
				}
			}
		}

		// free all allocated data

		// continue to next link node
		link = (struct rtnl_link *) nl_cache_get_next((struct nl_object *) link);
	}

	rtnl_qdisc_put(qdisc);
	nl_cache_free(cache);

	error = SR_ERR_OK; // set error to OK, since it will be modified by snprintf

	goto out;

error_out:
	error = SR_ERR_CALLBACK_FAILED;

out:
	nl_socket_free(socket);
	return error ? SR_ERR_CALLBACK_FAILED : SR_ERR_OK;
}

static int get_system_boot_time(char boot_datetime[])
{
	time_t now = 0;
	struct tm *ts = {0};
	struct sysinfo s_info = {0};
	time_t uptime_seconds = 0;

	now = time(NULL);

	ts = localtime(&now);
	if (ts == NULL)
		return -1;

	if (sysinfo(&s_info) != 0)
		return -1;

	uptime_seconds = s_info.uptime;

	time_t diff = now - uptime_seconds;

	ts = localtime(&diff);
	if (ts == NULL)
		return -1;

	/* must satisfy constraint (type yang:date-and-time):
		"\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}(\.\d+)?(Z|[\+\-]\d{2}:\d{2})"
		TODO: Add support for:
			- 2021-02-09T06:02:39.234+01:00
			- 2021-02-09T06:02:39.234Z
			- 2021-02-09T06:02:39+11:11
	*/

	strftime(boot_datetime, DATETIME_BUF_SIZE, "%FT%TZ", ts);

	return 0;
}

static int init_state_changes(void)
{
	int error = 0;
	struct nl_sock *socket = NULL;
	struct nl_cache *cache = NULL;
	struct rtnl_link *link = NULL;
	if_state_t *tmp_st = NULL;
	pthread_attr_t attr;

	uint if_cnt = 0;

	struct {
		pthread_t *data;
		uint count;
	} thread_ls;

	pthread_t manager_thread;

	socket = nl_socket_alloc();
	if (socket == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nl_socket_alloc error: invalid socket");
		return -1;
	}

	if ((error = nl_connect(socket, NETLINK_ROUTE)) != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nl_connect error (%d): %s", error, nl_geterror(error));
		goto error_out;
	}

	error = rtnl_link_alloc_cache(socket, AF_UNSPEC, &cache);
	if (error != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_alloc_cache error (%d): %s", error, nl_geterror(error));
		goto error_out;
	}

	link = (struct rtnl_link *) nl_cache_get_first(cache);

	while (link != NULL) {
		++if_cnt;

		link = (struct rtnl_link *) nl_cache_get_next((struct nl_object *) link);
	}

	// allocate a list to contain if_cnt number of interface states
	if_state_list_alloc(&if_state_changes, if_cnt);

	thread_ls.data = (pthread_t *) malloc(sizeof(pthread_t) * if_cnt);
	thread_ls.count = if_cnt;

	link = (struct rtnl_link *) nl_cache_get_first(cache);
	if_cnt = 0;

	while (link != NULL) {
		tmp_st = if_state_list_get(&if_state_changes, if_cnt);
		if (tmp_st) {
			tmp_st->state = rtnl_link_get_operstate(link);

			char *tmp_name = NULL;
			tmp_name = rtnl_link_get_name(link);

			size_t len = strlen(tmp_name);
			tmp_st->name = xcalloc(len + 1, sizeof(char));
			strncpy(tmp_st->name, tmp_name, len);
			tmp_st->name[len] = '\0';
		}

		++if_cnt;
		link = (struct rtnl_link *) nl_cache_get_next((struct nl_object *) link);
	}

	error = nl_cache_mngr_alloc(NULL, NETLINK_ROUTE, 0, &link_manager);
	if (error != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nl_cache_mngr_alloc failed (%d): %s", error, nl_geterror(error));
		goto error_out;
	}

	error = nl_cache_mngr_add(link_manager, "route/link", cache_change_cb, NULL, &link_cache);
	if (error != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nl_cache_mngr_add failed (%d): %s", error, nl_geterror(error));
		goto error_out;
	}

	error = pthread_attr_init(&attr);
	if (error != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "pthread_attr_init failed (%d)", error);
		goto error_out;
	}
	error = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if (error != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "pthread_attr_setdetachstate failed (%d): invalid value in detachstate", error);
		goto error_out;
	}
	error = pthread_create(&manager_thread, &attr, manager_thread_cb, NULL);
	if (error != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "pthread_create failed (%d)", error);
		goto error_out;
	}

error_out:

	// clear libnl data
	nl_cache_free(cache);
	nl_socket_free(socket);

	// free tmp struct
	if (thread_ls.count) {
		free(thread_ls.data);
	}

	return error;
}

static void cache_change_cb(struct nl_cache *cache, struct nl_object *obj, int val, void *arg)
{
	struct rtnl_link *link = NULL;
	char *name = NULL;
	if_state_t *tmp_st = NULL;
	uint8_t tmp_state = 0;

	SRPLG_LOG_DBG(PLUGIN_NAME, "entered cb function for a link manager");

	link = (struct rtnl_link *) nl_cache_get_first(cache);

	while (link != NULL) {
		name = rtnl_link_get_name(link);

		tmp_st = if_state_list_get_by_if_name(&if_state_changes, name);
		if (!tmp_st) {
			link = (struct rtnl_link *) nl_cache_get_next((struct nl_object *) link);
			continue;
		}

		tmp_state = rtnl_link_get_operstate(link);

		if (tmp_state != tmp_st->state) {
			SRPLG_LOG_DBG(PLUGIN_NAME, "Interface %s changed operstate from %d to %d", name, tmp_st->state, tmp_state);
			tmp_st->state = tmp_state;
			tmp_st->last_change = time(NULL);
		}
		link = (struct rtnl_link *) nl_cache_get_next((struct nl_object *) link);
	}
}

static void *manager_thread_cb(void *data)
{
	do {
		nl_cache_mngr_data_ready(link_manager);
		sleep(1);
	} while (exit_application == 0);

	return NULL;
}

#ifndef PLUGIN
#include <signal.h>
#include <unistd.h>

static void sigint_handler(__attribute__((unused)) int signum);

int main(void)
{
	int error = SR_ERR_OK;
	sr_conn_ctx_t *connection = NULL;
	sr_session_ctx_t *session = NULL;
	void *private_data = NULL;

	sr_log_stderr(SR_LL_DBG);

	error = sr_connect(SR_CONN_DEFAULT, &connection);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_connect error (%d): %s", error, sr_strerror(error));
		goto out;
	}

	error = sr_session_start(connection, SR_DS_RUNNING, &session);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_session_start error (%d): %s", error, sr_strerror(error));
		goto out;
	}

	error = sr_plugin_init_cb(session, &private_data);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_plugin_init_cb error");
		goto out;
	}

	signal(SIGINT, sigint_handler);
	signal(SIGPIPE, SIG_IGN);
	while (!exit_application) {
		sleep(1);
	}

out:
	sr_plugin_cleanup_cb(session, private_data);
	sr_disconnect(connection);

	pthread_exit(0);

	return error ? -1 : 0;
}

static void sigint_handler(__attribute__((unused)) int signum)
{
	SRPLG_LOG_INF(PLUGIN_NAME, "Sigint called, exiting...");
	exit_application = 1;
}

#endif
