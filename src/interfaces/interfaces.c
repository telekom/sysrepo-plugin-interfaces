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

#define BASE_YANG_MODEL "ietf-interfaces"
#define BASE_IP_YANG_MODEL "ietf-ip"

#define SYSREPOCFG_EMPTY_CHECK_COMMAND "sysrepocfg -X -d running -m " BASE_YANG_MODEL

// config data
#define INTERFACES_YANG_MODEL "/" BASE_YANG_MODEL ":interfaces"
#define INTERFACE_LIST_YANG_PATH INTERFACES_YANG_MODEL "/interface"

// other #defines
#define MAC_ADDR_MAX_LENGTH 18
#define MAX_DESCR_LEN 100
#define DATETIME_BUF_SIZE 30
#define CLASS_NET_LINE_LEN 1024
#define ADDR_STR_BUF_SIZE 45 // max ip string length (15 for ipv4 and 45 for ipv6)
#define MAX_IF_NAME_LEN IFNAMSIZ // 16 bytes
#define CMD_LEN 1024

// callbacks
static int interfaces_module_change_cb(sr_session_ctx_t *session, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data);
static int interfaces_state_data_cb(sr_session_ctx_t *session, const char *module_name, const char *path, const char *request_xpath, uint32_t request_id, struct lyd_node **parent, void *private_data);

// helper functions
static bool system_running_datastore_is_empty_check(void);
static int load_data(sr_session_ctx_t *session, link_data_list_t *ld);
static int load_startup(sr_session_ctx_t *session, link_data_list_t *ld);
static char *interfaces_xpath_get(const struct lyd_node *node);
static bool check_system_interface(const char *interface_name, bool *system_interface);
int set_config_value(const char *xpath, const char *value);
int add_interface_ipv4(link_data_t *ld, struct rtnl_link *old, struct rtnl_link *req, int if_idx);
static int remove_ipv4_address(ip_address_list_t *addr_list, struct nl_sock *socket, struct rtnl_link *old);
int add_interface_ipv6(link_data_t *ld, struct rtnl_link *old, struct rtnl_link *req, int if_idx);
static int remove_ipv6_address(ip_address_list_t *addr_list, struct nl_sock *socket, struct rtnl_link *old);
static int remove_neighbors(ip_neighbor_list_t *nbor_list, struct nl_sock *socket, int addr_ver, int if_index);
int write_to_proc_file(const char *dir_path, char *interface, const char *fn, int val);
static int read_from_proc_file(const char *dir_path, char *interface, const char *fn, int *val);
static int read_from_sys_file(const char *dir_path, char *interface, int *val);
int delete_config_value(const char *xpath, const char *value);
int update_link_info(link_data_list_t *ld, sr_change_oper_t operation);
static char *convert_ianaiftype(char *iana_if_type);
int add_existing_links(sr_session_ctx_t *session, link_data_list_t *ld);
static int get_interface_description(sr_session_ctx_t *session, char *name, char **description);
static int create_vlan_qinq(char *name, char *parent_interface, uint16_t outer_vlan_id, uint16_t second_vlan_id);
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
		SRP_LOG_ERRMSG("link_data_list_init error");
		goto out;
	}

	error = add_existing_links(session, &link_data_list);
	if (error != 0) {
		SRP_LOG_ERRMSG("add_existing_links error");
		goto out;
	}

	SRP_LOG_INF("start session to startup datastore");

	if_state_list_init(&if_state_changes);

	error = init_state_changes();
	if (error != 0) {
		SRP_LOG_ERRMSG("Error occurred while initializing threads to track interface changes... exiting");
		goto out;
	}

	connection = sr_session_get_connection(session);
	error = sr_session_start(connection, SR_DS_STARTUP, &startup_session);
	if (error) {
		SRP_LOG_ERR("sr_session_start error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	*private_data = startup_session;

	if (system_running_datastore_is_empty_check() == true) {
		SRP_LOG_INF("running DS is empty, loading data");

		error = load_data(session, &link_data_list);
		if (error) {
			SRP_LOG_ERRMSG("load_data error");
			goto error_out;
		}

		error = sr_copy_config(startup_session, BASE_YANG_MODEL, SR_DS_RUNNING, 0, 0);
		if (error) {
			SRP_LOG_ERR("sr_copy_config error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}
	}

	// load data from startup datastore to internal list
	error = load_startup(startup_session, &link_data_list);
	if (error != 0) {
		SRP_LOG_ERRMSG("load_startup error");
		goto error_out;
	}

	// apply what is present in the startup datastore
	error = update_link_info(&link_data_list, SR_OP_CREATED);
	if (error != 0) {
		SRP_LOG_ERRMSG("update_link_info error");
		goto error_out;
	}

	SRP_LOG_INFMSG("subscribing to module change");

	// sub to any module change - for now
	error = sr_module_change_subscribe(session, BASE_YANG_MODEL, "/" BASE_YANG_MODEL ":*//.", interfaces_module_change_cb, *private_data, 0, SR_SUBSCR_DEFAULT, &subscription);
	if (error) {
		SRP_LOG_ERR("sr_module_change_subscribe error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	error = sr_oper_get_items_subscribe(session, BASE_YANG_MODEL, INTERFACES_YANG_MODEL "/*", interfaces_state_data_cb, NULL, SR_SUBSCR_CTX_REUSE, &subscription);
	if (error) {
		SRP_LOG_ERR("sr_oper_get_items_subscribe error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	SRP_LOG_INFMSG("plugin init done");

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

static bool system_running_datastore_is_empty_check(void)
{
	FILE *sysrepocfg_DS_empty_check = NULL;
	bool is_empty = false;

	sysrepocfg_DS_empty_check = popen(SYSREPOCFG_EMPTY_CHECK_COMMAND, "r");
	if (sysrepocfg_DS_empty_check == NULL) {
		SRP_LOG_WRN("could not execute %s", SYSREPOCFG_EMPTY_CHECK_COMMAND);
		is_empty = true;
		goto out;
	}

	if (fgetc(sysrepocfg_DS_empty_check) == EOF) {
		is_empty = true;
	}

out:
	if (sysrepocfg_DS_empty_check) {
		pclose(sysrepocfg_DS_empty_check);
	}

	return is_empty;
}

static int load_data(sr_session_ctx_t *session, link_data_list_t *ld)
{
	int error = 0;
	char interface_path_buffer[PATH_MAX] = {0};
	char xpath_buffer[PATH_MAX] = {0};
	char tmp_buffer[PATH_MAX] = {0};

	for (int i = 0; i < ld->count; i++) {
		char *name = ld->links[i].name;
		char *type = ld->links[i].type;
		char *description = ld->links[i].description;
		char *enabled = ld->links[i].enabled;
		char *parent_interface = ld->links[i].extensions.parent_interface;

		snprintf(interface_path_buffer, sizeof(interface_path_buffer) / sizeof(char), "%s[name=\"%s\"]", INTERFACE_LIST_YANG_PATH, name);

		if (strcmp(type, "lo") == 0) {
			type = "iana-if-type:softwareLoopback";
		} else if (strcmp(type, "eth") == 0) {
			type = "iana-if-type:ethernetCsmacd";
		} else if (strcmp(type, "vlan") == 0) {
			type = "iana-if-type:l2vlan";
		} else if (strcmp(type, "dummy") == 0) {
			type = "iana-if-type:other"; // since dummy is not a real type
		} else {
			SRP_LOG_INF("load_data unsupported interface type %s", type);
			continue;
		}

		error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/name", interface_path_buffer);
		if (error < 0) {
			goto error_out;
		}

		error = sr_set_item_str(session, xpath_buffer, name, NULL, SR_EDIT_DEFAULT);
		if (error) {
			SRP_LOG_ERR("sr_set_item_str error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}

		error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/description", interface_path_buffer);
		if (error < 0) {
			goto error_out;
		}

		error = sr_set_item_str(session, xpath_buffer, description, NULL, SR_EDIT_DEFAULT);
		if (error) {
			SRP_LOG_ERR("sr_set_item_str error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}

		error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/type", interface_path_buffer);
		if (error < 0) {
			goto error_out;
		}

		error = sr_set_item_str(session, xpath_buffer, type, NULL, SR_EDIT_DEFAULT);
		if (error) {
			SRP_LOG_ERR("sr_set_item_str error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}

		error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/enabled", interface_path_buffer);
		if (error < 0) {
			goto error_out;
		}

		error = sr_set_item_str(session, xpath_buffer, enabled, NULL, SR_EDIT_DEFAULT);
		if (error) {
			SRP_LOG_ERR("sr_set_item_str error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}

		if (parent_interface != 0) {
			error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/ietf-if-extensions:parent-interface", interface_path_buffer);
			if (error < 0) {
				goto error_out;
			}

			error = sr_set_item_str(session, xpath_buffer, parent_interface, NULL, SR_EDIT_DEFAULT);
			if (error) {
				SRP_LOG_ERR("sr_set_item_str error (%d): %s", error, sr_strerror(error));
				goto error_out;
			}
		}

		// handle vlan interfaces


		// ietf-ip
		// TODO: refactor this!
		// list of ipv4 addresses
		if (strcmp(ld->links[i].name, name ) == 0) {
			// enabled
			// TODO

			// forwarding
			uint8_t ipv4_forwarding = ld->links[i].ipv4.forwarding;

			error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/ietf-ip:ipv4/forwarding", interface_path_buffer);
			if (error < 0) {
				goto error_out;
			}

			SRP_LOG_DBG("xpath_buffer: %s = %s", xpath_buffer, ipv4_forwarding == 0 ? "false" : "true");

			error = sr_set_item_str(session, xpath_buffer, ipv4_forwarding == 0 ? "false" : "true", NULL, SR_EDIT_DEFAULT);
			if (error) {
				SRP_LOG_ERR("sr_set_item_str error (%d): %s", error, sr_strerror(error));
				goto error_out;
			}

			uint32_t ipv4_addr_count = ld->links[i].ipv4.addr_list.count;

			for (uint32_t j = 0; j < ipv4_addr_count; j++) {
				if (ld->links[i].ipv4.addr_list.addr[j].ip != NULL) { // in case we deleted an ip address it will be NULL
					char *ip_addr = ld->links[i].ipv4.addr_list.addr[j].ip;

					int ipv4_mtu = ld->links[i].ipv4.mtu;
					if (ipv4_mtu > 0) {
						error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/ietf-ip:ipv4/mtu", interface_path_buffer);
						if (error < 0) {
							goto error_out;
						}

						snprintf(tmp_buffer, sizeof(tmp_buffer), "%u", ipv4_mtu);

						SRP_LOG_DBG("xpath_buffer: %s = %s", xpath_buffer, tmp_buffer);

						error = sr_set_item_str(session, xpath_buffer, tmp_buffer, NULL, SR_EDIT_DEFAULT);
						if (error) {
							SRP_LOG_ERR("sr_set_item_str error (%d): %s", error, sr_strerror(error));
							goto error_out;
						}
					}

					error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/ietf-ip:ipv4/address[ip='%s']/ip", interface_path_buffer, ip_addr);
					if (error < 0) {
						goto error_out;
					}

					// ip
					SRP_LOG_DBG("xpath_buffer: %s = %s", xpath_buffer, ld->links[i].ipv4.addr_list.addr[j].ip);
					error = sr_set_item_str(session, xpath_buffer, ld->links[i].ipv4.addr_list.addr[j].ip, NULL, SR_EDIT_DEFAULT);
					if (error) {
						SRP_LOG_ERR("sr_set_item_str error (%d): %s", error, sr_strerror(error));
						goto error_out;
					}

					// subnet
					snprintf(tmp_buffer, sizeof(tmp_buffer), "%u", ld->links[i].ipv4.addr_list.addr[j].subnet);

					error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/ietf-ip:ipv4/address[ip='%s']/prefix-length", interface_path_buffer, ip_addr);
					if (error < 0) {
						goto error_out;
					}

					SRP_LOG_DBG("xpath_buffer: %s = %s", xpath_buffer, tmp_buffer);

					error = sr_set_item_str(session, xpath_buffer, tmp_buffer, NULL, SR_EDIT_DEFAULT);
					if (error) {
						SRP_LOG_ERR("sr_set_item_str error (%d): %s", error, sr_strerror(error));
						goto error_out;
					}
				}
			}

			// list of ipv4 neighbors
			uint32_t ipv4_neigh_count = ld->links[i].ipv4.nbor_list.count;
			for (uint32_t j = 0; j < ipv4_neigh_count; j++) {
				char *ip_addr = ld->links[i].ipv4.nbor_list.nbor[j].ip;

				error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/ietf-ip:ipv4/neighbor[ip='%s']/ip", interface_path_buffer, ip_addr);
				if (error < 0) {
					goto error_out;
				}

				// ip
				SRP_LOG_DBG("xpath_buffer: %s = %s", xpath_buffer, ld->links[i].ipv4.nbor_list.nbor[j].ip);
				error = sr_set_item_str(session, xpath_buffer, ld->links[i].ipv4.nbor_list.nbor[j].ip, NULL, SR_EDIT_DEFAULT);
				if (error) {
					SRP_LOG_ERR("sr_set_item_str error (%d): %s", error, sr_strerror(error));
					goto error_out;
				}

				// link-layer-address
				error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/ietf-ip:ipv4/neighbor[ip='%s']/link-layer-address", interface_path_buffer, ip_addr);
				if (error < 0) {
					goto error_out;
				}

				SRP_LOG_DBG("xpath_buffer: %s = %s", xpath_buffer, ld->links[i].ipv4.nbor_list.nbor[j].phys_addr);

				error = sr_set_item_str(session, xpath_buffer, ld->links[i].ipv4.nbor_list.nbor[j].phys_addr, NULL, SR_EDIT_DEFAULT);
				if (error) {
					SRP_LOG_ERR("sr_set_item_str error (%d): %s", error, sr_strerror(error));
					goto error_out;
				}
			}

			// list of ipv6 addresses
			// enabled
			uint8_t ipv6_enabled = ld->links[i].ipv6.ip_data.enabled;

			error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/ietf-ip:ipv6/enabled", interface_path_buffer);
			if (error < 0) {
				goto error_out;
			}

			SRP_LOG_DBG("xpath_buffer: %s = %s", xpath_buffer, ipv6_enabled == 0 ? "false" : "true");

			error = sr_set_item_str(session, xpath_buffer, ipv6_enabled == 0 ? "false" : "true", NULL, SR_EDIT_DEFAULT);
			if (error) {
				SRP_LOG_ERR("sr_set_item_str error (%d): %s", error, sr_strerror(error));
				goto error_out;
			}

			// forwarding
			uint8_t ipv6_forwarding = ld->links[i].ipv6.ip_data.forwarding;

			error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/ietf-ip:ipv6/forwarding", interface_path_buffer);
			if (error < 0) {
				goto error_out;
			}

			SRP_LOG_DBG("xpath_buffer: %s = %s", xpath_buffer, ipv6_forwarding == 0 ? "false" : "true");

			error = sr_set_item_str(session, xpath_buffer, ipv6_forwarding == 0 ? "false" : "true", NULL, SR_EDIT_DEFAULT);
			if (error) {
				SRP_LOG_ERR("sr_set_item_str error (%d): %s", error, sr_strerror(error));
				goto error_out;
			}

			uint32_t ipv6_addr_count = ld->links[i].ipv6.ip_data.addr_list.count;

			for (uint32_t j = 0; j < ipv6_addr_count; j++) {
				if (ld->links[i].ipv6.ip_data.addr_list.addr[j].ip != NULL) { // in case we deleted an ip address it will be NULL
					char *ip_addr = ld->links[i].ipv6.ip_data.addr_list.addr[j].ip;
					int ipv6_mtu = ld->links[i].ipv6.ip_data.mtu;

					// mtu
					if (ipv6_mtu > 0 && ip_addr != NULL) {
						error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/ietf-ip:ipv6/mtu", interface_path_buffer);
						if (error < 0) {
							goto error_out;
						}
						snprintf(tmp_buffer, sizeof(tmp_buffer), "%u", ipv6_mtu);

						SRP_LOG_DBG("xpath_buffer: %s = %s", xpath_buffer, tmp_buffer);

						error = sr_set_item_str(session, xpath_buffer, tmp_buffer, NULL, SR_EDIT_DEFAULT);
						if (error) {
							SRP_LOG_ERR("sr_set_item_str error (%d): %s", error, sr_strerror(error));
							goto error_out;
						}
					}

					error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/ietf-ip:ipv6/address[ip='%s']/ip", interface_path_buffer, ip_addr);
					if (error < 0) {
						goto error_out;
					}

					// ip
					SRP_LOG_DBG("xpath_buffer: %s = %s", xpath_buffer, ld->links[i].ipv6.ip_data.addr_list.addr[j].ip);

					error = sr_set_item_str(session, xpath_buffer, ld->links[i].ipv6.ip_data.addr_list.addr[j].ip, NULL, SR_EDIT_DEFAULT);
					if (error) {
						SRP_LOG_ERR("sr_set_item_str error (%d): %s", error, sr_strerror(error));
						goto error_out;
					}

					// subnet
					snprintf(tmp_buffer, sizeof(tmp_buffer), "%u", ld->links[i].ipv6.ip_data.addr_list.addr[j].subnet);

					error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/ietf-ip:ipv6/address[ip='%s']/prefix-length", interface_path_buffer, ip_addr);
					if (error < 0) {
						goto error_out;
					}

					SRP_LOG_DBG("xpath_buffer: %s = %s", xpath_buffer, tmp_buffer);

					error = sr_set_item_str(session, xpath_buffer, tmp_buffer, NULL, SR_EDIT_DEFAULT);
					if (error) {
						SRP_LOG_ERR("sr_set_item_str error (%d): %s", error, sr_strerror(error));
						goto error_out;
					}
				}
			}

			// list of ipv6 neighbors
			uint32_t ipv6_neigh_count = ld->links[i].ipv6.ip_data.nbor_list.count;
			for (uint32_t j = 0; j < ipv6_neigh_count; j++) {
				char *ip_addr = ld->links[i].ipv6.ip_data.nbor_list.nbor[j].ip;

				error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/ietf-ip:ipv6/neighbor[ip='%s']/ip", interface_path_buffer, ip_addr);
				if (error < 0) {
					goto error_out;
				}

				// ip
				SRP_LOG_DBG("xpath_buffer: %s = %s", xpath_buffer, ld->links[i].ipv6.ip_data.nbor_list.nbor[j].ip);
				error = sr_set_item_str(session, xpath_buffer, ld->links[i].ipv6.ip_data.nbor_list.nbor[j].ip, NULL, SR_EDIT_DEFAULT);
				if (error) {
					SRP_LOG_ERR("sr_set_item_str error (%d): %s", error, sr_strerror(error));
					goto error_out;
				}

				// link-layer-address
				error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/ietf-ip:ipv6/neighbor[ip='%s']/link-layer-address", interface_path_buffer, ip_addr);
				if (error < 0) {
					goto error_out;
				}

				SRP_LOG_DBG("xpath_buffer: %s = %s", xpath_buffer, ld->links[i].ipv6.ip_data.nbor_list.nbor[j].phys_addr);

				error = sr_set_item_str(session, xpath_buffer, ld->links[i].ipv6.ip_data.nbor_list.nbor[j].phys_addr, NULL, SR_EDIT_DEFAULT);
				if (error) {
					SRP_LOG_ERR("sr_set_item_str error (%d): %s", error, sr_strerror(error));
					goto error_out;
				}
			}
		}
	}

	error = sr_apply_changes(session, 0, 0);
	if (error) {
		SRP_LOG_ERR("sr_apply_changes error (%d): %s", error, sr_strerror(error));
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

	error = sr_get_items(session, "/ietf-interfaces:interfaces//.", 0, 0, &vals, &val_count);
	if (error != SR_ERR_OK) {
		SRP_LOG_ERR("sr_get_items error (%d): %s", error, sr_strerror(error));
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
					SRP_LOG_ERRMSG("snprintf error");
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
			SRP_LOG_ERR("set_config_value error (%d)", error);
			goto error_out;
		}

		FREE_SAFE(xpath);
		FREE_SAFE(val);
	}

	return 0;

error_out:
	if (xpath != NULL) {
		FREE_SAFE(xpath);
	}
	if (val != NULL) {
		FREE_SAFE(val);
	}
	return -1;
}

void sr_plugin_cleanup_cb(sr_session_ctx_t *session, void *private_data)
{
	sr_session_ctx_t *startup_session = (sr_session_ctx_t *) private_data;

	// copy the running datastore to startup one, in case we reboot
	sr_copy_config(startup_session, BASE_YANG_MODEL, SR_DS_RUNNING, 0, 0);

	exit_application = 1;

	if (startup_session) {
		sr_session_stop(startup_session);
	}

	link_data_list_free(&link_data_list);
	if_state_list_free(&if_state_changes);
	nl_cache_mngr_free(link_manager);

	SRP_LOG_INF("plugin cleanup finished");
}

static int interfaces_module_change_cb(sr_session_ctx_t *session, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data)
{
	int error = 0;
	sr_change_iter_t *system_change_iter = NULL;
	sr_change_oper_t operation = SR_OP_CREATED;
	const struct lyd_node *node = NULL;
	const char *prev_value = NULL;
	const char *prev_list = NULL;
	bool prev_default = false;
	char *node_xpath = NULL;
	const char *node_value = NULL;
	struct lyd_node_leaf_list *node_leaf_list;
	struct lys_node_leaf *schema_node_leaf;

	SRP_LOG_INF("module_name: %s, xpath: %s, event: %d, request_id: %u", module_name, xpath, event, request_id);

	if (event == SR_EV_ABORT) {
		SRP_LOG_ERR("aborting changes for: %s", xpath);
		error = -1;
		goto error_out;
	}

	if (event == SR_EV_DONE) {
		error = sr_copy_config(session, BASE_YANG_MODEL, SR_DS_RUNNING, 0, 0);
		if (error) {
			SRP_LOG_ERR("sr_copy_config error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}
	}

	if (event == SR_EV_CHANGE) {
		error = sr_get_changes_iter(session, xpath, &system_change_iter);
		if (error) {
			SRP_LOG_ERR("sr_get_changes_iter error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}
		while (sr_get_change_tree_next(session, system_change_iter, &operation, &node, &prev_value, &prev_list, &prev_default) == SR_ERR_OK) {
			node_xpath = interfaces_xpath_get(node);

			if (node->schema->nodetype == LYS_LEAF || node->schema->nodetype == LYS_LEAFLIST) {
				node_leaf_list = (struct lyd_node_leaf_list *) node;
				node_value = node_leaf_list->value_str;
				if (node_value == NULL) {
						schema_node_leaf = (struct lys_node_leaf *) node_leaf_list->schema;
						node_value = schema_node_leaf->dflt ? schema_node_leaf->dflt : "";
				}
			}

			SRP_LOG_DBG("node_xpath: %s; prev_val: %s; node_val: %s; operation: %d", node_xpath, prev_value, node_value, operation);

			if (node->schema->nodetype == LYS_LEAF || node->schema->nodetype == LYS_LEAFLIST) {
				if (operation == SR_OP_CREATED || operation == SR_OP_MODIFIED) {
					error = set_config_value(node_xpath, node_value);
					if (error) {
						SRP_LOG_ERR("set_config_value error (%d)", error);
						goto error_out;
					}
				} else if (operation == SR_OP_DELETED) {
					// check if this is a system interface (e.g.: lo, wlan0, enp0s31f6 etc.)
					bool system_interface = false;
					error = check_system_interface(node_value, &system_interface);
					if (error) {
						SRP_LOG_ERRMSG("check_system_interface error");
						goto error_out;
					}

					// check if system interface but also
					// check if parent-interface node (virtual interfaces can have a system interface as parent)
					if (system_interface && !(strstr(node_xpath, "/ietf-if-extensions:parent-interface") != NULL)) {
						SRP_LOG_ERRMSG("Can't delete a system interface");
						FREE_SAFE(node_xpath);
						sr_free_change_iter(system_change_iter);
						return SR_ERR_INVAL_ARG;
					}

					error = delete_config_value(node_xpath, node_value);
					if (error) {
						SRP_LOG_ERR("delete_config_value error (%d)", error);
						goto error_out;
					}
				}
			}
			FREE_SAFE(node_xpath);
			node_value = NULL;
		}

		error = update_link_info(&link_data_list, operation);
		if (error) {
			error = SR_ERR_CALLBACK_FAILED;
			SRP_LOG_ERRMSG("update_link_info error");
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
		SRP_LOG_ERRMSG("sr_xpath_node_name error");
		error = SR_ERR_CALLBACK_FAILED;
		goto out;
	}

	interface_node_name = sr_xpath_key_value((char *) xpath, "interface", "name", &state);

	if (interface_node_name == NULL) {
		SRP_LOG_ERRMSG("sr_xpath_key_value error");
		error = SR_ERR_CALLBACK_FAILED;
		goto out;
	}

	error = link_data_list_add(&link_data_list, interface_node_name);
	if (error != 0) {
		SRP_LOG_ERRMSG("link_data_list_add error");
		error = SR_ERR_CALLBACK_FAILED;
		goto out;
	}

	if (strcmp(interface_node, "description") == 0) {
		// change desc
		error = link_data_list_set_description(&link_data_list, interface_node_name, (char *) value);
		if (error != 0) {
			SRP_LOG_ERRMSG("link_data_list_set_description error");
			error = SR_ERR_CALLBACK_FAILED;
			goto out;
		}
	} else if (strcmp(interface_node, "type") == 0) {
		// change type

		// convert the iana-if-type to a "real" interface type which libnl understands
		char *interface_type = convert_ianaiftype((char *) value);
		if (interface_type == NULL) {
			SRP_LOG_ERRMSG("convert_ianaiftype error");
			error = SR_ERR_CALLBACK_FAILED;
			goto out;
		}

		error = link_data_list_set_type(&link_data_list, interface_node_name, interface_type);
		if (error != 0) {
			SRP_LOG_ERRMSG("link_data_list_set_type error");
			error = SR_ERR_CALLBACK_FAILED;
			goto out;
		}
	} else if (strcmp(interface_node, "enabled") == 0 && strstr(xpath_cpy, "ietf-ip:") == 0) {
		// change enabled
		error = link_data_list_set_enabled(&link_data_list, interface_node_name, (char *) value);
		if (error != 0) {
			SRP_LOG_ERRMSG("link_data_list_set_enabled error");
			error = SR_ERR_CALLBACK_FAILED;
			goto out;
		}
	}
	else if (strstr(xpath_cpy, "ietf-ip:ipv4") != 0) {
		SRP_LOG_DBG("ietf-ip:ipv4 change: '%s' on interface '%s'", interface_node, interface_node_name);
		memset(&state, 0, sizeof(sr_xpath_ctx_t));
		// check if an ip address has been added
		address_node_ip = sr_xpath_key_value((char *) xpath_cpy, "address", "ip", &state);

		// check if a neighbor has been added
		neighbor_node_ip = sr_xpath_key_value((char *) xpath_cpy, "neighbor", "ip", &state);

		if (address_node_ip != NULL) {
			// address has been added -> check for interface node (last node of the path) -> (prefix-length || netmask)
			if (strcmp(interface_node, "prefix-length") == 0) {
				error = link_data_list_add_ipv4_address(&link_data_list, interface_node_name, address_node_ip, (char *) value, ip_subnet_type_prefix_length);
				if (error != 0) {
					SRP_LOG_ERR("link_data_list_add_ipv4_address error (%d) : %s", error, strerror(error));
					error = SR_ERR_CALLBACK_FAILED;
					goto out;
				}
			} else if (strcmp(interface_node, "netmask") == 0) {
				error = link_data_list_add_ipv4_address(&link_data_list, interface_node_name, address_node_ip, (char *) value, ip_subnet_type_netmask);
				if (error != 0) {
					SRP_LOG_ERR("link_data_list_add_ipv4_address error (%d) : %s", error, strerror(error));
					error = SR_ERR_CALLBACK_FAILED;
					goto out;
				}
			}
		} else if (neighbor_node_ip != NULL) {
			if (strcmp(interface_node, "link-layer-address") == 0) {
				error = link_data_list_add_ipv4_neighbor(&link_data_list, interface_node_name, neighbor_node_ip, (char *) value);
				if (error != 0) {
					SRP_LOG_ERR("link_data_list_add_ipv4_neighbor error (%d) : %s", error, strerror(error));
					error = SR_ERR_CALLBACK_FAILED;
					goto out;
				}
			}
		} else if (strcmp(interface_node, "enabled") == 0) {
			error = link_data_list_set_ipv4_enabled(&link_data_list, interface_node_name, (char *) value);
			if (error != 0) {
				SRP_LOG_ERR("link_data_list_set_ipv4_enabled error (%d) : %s", error, strerror(error));
				error = SR_ERR_CALLBACK_FAILED;
				goto out;
			}
		} else if (strcmp(interface_node, "forwarding") == 0) {
			error = link_data_list_set_ipv4_forwarding(&link_data_list, interface_node_name, (char *) value);
			if (error != 0) {
				SRP_LOG_ERR("link_data_list_set_ipv4_forwarding error (%d) : %s", error, strerror(error));
				error = SR_ERR_CALLBACK_FAILED;
				goto out;
			}
		} else if (strcmp(interface_node, "mtu") == 0) {
			error = link_data_list_set_ipv4_mtu(&link_data_list, interface_node_name, (char *) value);
			if (error != 0) {
				SRP_LOG_ERR("link_data_list_set_ipv4_mtu error (%d) : %s", error, strerror(error));
				error = SR_ERR_CALLBACK_FAILED;
				goto out;
			}
		}
	} else if (strstr(xpath_cpy, "ietf-ip:ipv6") != 0) {
		SRP_LOG_DBG("ietf-ip:ipv6 change: '%s' on interface '%s'", interface_node, interface_node_name);
		// check if an ip address has been added
		address_node_ip = sr_xpath_key_value((char *) xpath_cpy, "address", "ip", &state);
		// check if a neighbor has been added
		neighbor_node_ip = sr_xpath_key_value((char *) xpath_cpy, "neighbor", "ip", &state);

		if (address_node_ip != NULL) {
			// address has been added -> check for interface node (last node of the path) -> (prefix-length || netmask)
			if (strcmp(interface_node, "prefix-length") == 0) {
				error = link_data_list_add_ipv6_address(&link_data_list, interface_node_name, address_node_ip, (char *) value);
				if (error != 0) {
					SRP_LOG_ERR("link_data_list_add_ipv4_address error (%d) : %s", error, strerror(error));
					error = SR_ERR_CALLBACK_FAILED;
					goto out;
				}
			} else if (strcmp(interface_node, "netmask") == 0) {
				error = link_data_list_add_ipv6_address(&link_data_list, interface_node_name, address_node_ip, (char *) value);
				if (error != 0) {
					SRP_LOG_ERR("link_data_list_add_ipv4_address error (%d) : %s", error, strerror(error));
					error = SR_ERR_CALLBACK_FAILED;
					goto out;
				}
			}
		} else if (neighbor_node_ip != NULL) {
			if (strcmp(interface_node, "link-layer-address") == 0) {
				error = link_data_list_add_ipv6_neighbor(&link_data_list, interface_node_name, neighbor_node_ip, (char *) value);
				if (error != 0) {
					SRP_LOG_ERR("link_data_list_add_ipv4_neighbor error (%d) : %s", error, strerror(error));
					error = SR_ERR_CALLBACK_FAILED;
					goto out;
				}
			}
		} else if (strcmp(interface_node, "enabled") == 0) {
			error = link_data_list_set_ipv6_enabled(&link_data_list, interface_node_name, (char *) value);
			if (error != 0) {
				SRP_LOG_ERR("link_data_list_set_ipv4_enabled error (%d) : %s", error, strerror(error));
				error = SR_ERR_CALLBACK_FAILED;
				goto out;
			}
		} else if (strcmp(interface_node, "forwarding") == 0) {
			error = link_data_list_set_ipv6_forwarding(&link_data_list, interface_node_name, (char *) value);
			if (error != 0) {
				SRP_LOG_ERR("link_data_list_set_ipv4_forwarding error (%d) : %s", error, strerror(error));
				error = SR_ERR_CALLBACK_FAILED;
				goto out;
			}
		} else if (strcmp(interface_node, "mtu") == 0) {
			error = link_data_list_set_ipv6_mtu(&link_data_list, interface_node_name, (char *) value);
			if (error != 0) {
				SRP_LOG_ERR("link_data_list_set_ipv4_mtu error (%d) : %s", error, strerror(error));
				error = SR_ERR_CALLBACK_FAILED;
				goto out;
			}
		}
	} else if (strcmp(interface_node, "ietf-if-extensions:parent-interface") == 0) {
		// change parent-interface
		error = link_data_list_set_parent(&link_data_list, interface_node_name, (char *) value);
		if (error != 0) {
			SRP_LOG_ERRMSG("link_data_list_set_parent error");
			error = SR_ERR_CALLBACK_FAILED;
			goto out;
		}
	} else if (strstr(xpath_cpy, "ietf-if-extensions:encapsulation/ietf-if-vlan-encapsulation:dot1q-vlan/outer-tag/tag-type") != 0) {
		error = link_data_list_set_outer_tag_type(&link_data_list, interface_node_name, (char *)value);
		if (error != 0) {
			SRP_LOG_ERRMSG("link_data_list_set_parent error");
			error = SR_ERR_CALLBACK_FAILED;
			goto out;
		}
	} else if (strstr(xpath_cpy, "ietf-if-extensions:encapsulation/ietf-if-vlan-encapsulation:dot1q-vlan/outer-tag/vlan-id") != 0) {
		error = link_data_list_set_outer_vlan_id(&link_data_list, interface_node_name, (uint16_t) atoi(value));
		if (error != 0) {
			SRP_LOG_ERRMSG("link_data_list_set_parent error");
			error = SR_ERR_CALLBACK_FAILED;
			goto out;
		}
	} else if (strstr(xpath_cpy, "ietf-if-extensions:encapsulation/ietf-if-vlan-encapsulation:dot1q-vlan/second-tag/tag-type") != 0) {
		error = link_data_list_set_second_tag_type(&link_data_list, interface_node_name, (char *)value);
		if (error != 0) {
			SRP_LOG_ERRMSG("link_data_list_set_parent error");
			error = SR_ERR_CALLBACK_FAILED;
			goto out;
		}
	} else if (strstr(xpath_cpy, "ietf-if-extensions:encapsulation/ietf-if-vlan-encapsulation:dot1q-vlan/second-tag/vlan-id") != 0) {
		error = link_data_list_set_second_vlan_id(&link_data_list, interface_node_name, (uint16_t) atoi(value));
		if (error != 0) {
			SRP_LOG_ERRMSG("link_data_list_set_parent error");
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
		SRP_LOG_ERRMSG("sr_xpath_node_name error");
		error = SR_ERR_CALLBACK_FAILED;
		goto out;
	}

	interface_node_name = sr_xpath_key_value((char *) xpath, "interface", "name", &state);
	if (interface_node_name == NULL) {
		SRP_LOG_ERRMSG("sr_xpath_key_value error");
		error = SR_ERR_CALLBACK_FAILED;
		goto out;
	}

	if (strcmp(interface_node, "name") == 0) {
		// mark for deletion
		error = link_data_list_set_delete(&link_data_list, interface_node_name, true);
		if (error != 0) {
			SRP_LOG_ERRMSG("link_data_list_set_delete error");
			error = SR_ERR_CALLBACK_FAILED;
			goto out;
		}
	} else if (strcmp(interface_node, "description") == 0) {
		// set description to empty string
		error = link_data_list_set_description(&link_data_list, interface_node_name, "");
		if (error != 0) {
			SRP_LOG_ERRMSG("link_data_list_set_description error");
			error = SR_ERR_CALLBACK_FAILED;
			goto out;
		}
	} else if (strcmp(interface_node, "enabled") == 0) {
		// set enabled to false
		error = link_data_list_set_enabled(&link_data_list, interface_node_name, "");
		if (error != 0) {
			SRP_LOG_ERRMSG("link_data_list_set_enabled error");
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
					SRP_LOG_ERR("link_data_list_set_delete_ipv4_address error (%d) : %s", error, strerror(error));
					error = SR_ERR_CALLBACK_FAILED;
					goto out;
				}
			}
		} else if (neighbor_node_ip != NULL) {
			if (strcmp(interface_node, "ip") == 0) {
				error = link_data_list_set_delete_ipv4_neighbor(&link_data_list, interface_node_name, (char *) value);
				if (error != 0) {
					SRP_LOG_ERR("link_data_list_set_delete_ipv4_neighbor error (%d) : %s", error, strerror(error));
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
					SRP_LOG_ERR("link_data_list_set_delete_ipv6_address error (%d) : %s", error, strerror(error));
					error = SR_ERR_CALLBACK_FAILED;
					goto out;
				}
			}
		} else if (neighbor_node_ip != NULL) {
			if (strcmp(interface_node, "ip") == 0) {
				error = link_data_list_set_delete_ipv6_neighbor(&link_data_list, interface_node_name, (char *) value);
				if (error != 0) {
					SRP_LOG_ERR("link_data_list_set_delete_ipv6_neighbor error (%d) : %s", error, strerror(error));
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
		SRP_LOG_ERRMSG("nl_socket_alloc error: invalid socket");
		goto out;
	}

	if ((error = nl_connect(socket, NETLINK_ROUTE)) != 0) {
		SRP_LOG_ERR("nl_connect error (%d): %s", error, nl_geterror(error));
		goto out;
	}

	error = rtnl_link_alloc_cache(socket, AF_UNSPEC, &cache);
	if (error != 0) {
		SRP_LOG_ERR("rtnl_link_alloc_cache error (%d): %s", error, nl_geterror(error));
		goto out;
	}

	for (int i = 0; i < ld->count; i++) {
		char *name = ld->links[i].name;
		char *type = ld->links[i].type;
		char *enabled = ld->links[i].enabled;
		char *parent_interface = ld->links[i].extensions.parent_interface;
		uint16_t outer_vlan_id =  ld->links[i].extensions.encapsulation.dot1q_vlan.outer_vlan_id;
		uint16_t second_vlan_id =  ld->links[i].extensions.encapsulation.dot1q_vlan.second_vlan_id;
		char second_vlan_name[MAX_IF_NAME_LEN] = {0};
		bool delete = ld->links[i].delete;

		if (name == NULL ){//|| type == 0) {
			continue;
		}

		// handle vlan QinQ interfaces
		if (type != NULL && strcmp(type, "vlan") == 0 && second_vlan_id != 0) {
			error = snprintf(second_vlan_name, sizeof(second_vlan_name), "%s.%d", name, second_vlan_id);
			if (error < 0) {
				SRP_LOG_ERRMSG("snprintf error");
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
				SRP_LOG_ERR("rtnl_link_delete error (%d): %s", error, nl_geterror(error));
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
				SRP_LOG_ERR("remove_ipv4_address error (%d): %s", error, nl_geterror(error));
				goto out;
			}
		}

		// check if any ipv6 addresses need to be removed
		uint32_t ipv6_addr_count = ld->links[i].ipv6.ip_data.addr_list.count;

		if (ipv6_addr_count > 0) {
			error = remove_ipv6_address(&ld->links[i].ipv6.ip_data.addr_list, socket, old);
			if (error != 0) {
				SRP_LOG_ERR("remove_ipv6_address error (%d): %s", error, nl_geterror(error));
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
					SRP_LOG_ERRMSG("remove_neighbors error");
					goto out;
				}
			}

			// check if any ipv6 neighbors need to be removed
			uint32_t ipv6_neigh_count = ld->links[i].ipv6.ip_data.nbor_list.count;

			if (ipv6_neigh_count > 0) {
				error = remove_neighbors(&ld->links[i].ipv6.ip_data.nbor_list, socket, AF_INET6, index);
				if (error != 0) {
					SRP_LOG_ERRMSG("remove_neighbors error");
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
					create_vlan_qinq(name, parent_interface, outer_vlan_id, second_vlan_id);

				} else if (second_vlan_id == 0) {
					// normal vlan interface
					error = rtnl_link_set_type(request, type);
					if (error < 0) {
						SRP_LOG_ERR("rtnl_link_set_type error (%d): %s", error, nl_geterror(error));
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
					SRP_LOG_ERR("rtnl_link_set_type error (%d): %s", error, nl_geterror(error));
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
				SRP_LOG_ERRMSG("add_interface_ipv4 error");
				goto out;
			}

			error = add_interface_ipv6(&ld->links[i], old, request, rtnl_link_get_ifindex(old));
			if (error != 0) {
				SRP_LOG_ERRMSG("add_interface_ipv6 error");
				goto out;
			}

			// the interface with name already exists, change it
			error = rtnl_link_change(socket, old, request, 0);
			if (error != 0) {
				SRP_LOG_ERR("rtnl_link_change error (%d): %s", error, nl_geterror(error));
				goto out;
			}
		} else if (operation != SR_OP_DELETED) {
			// the interface doesn't exist

			// check if the interface is a system interface
			// non-virtual interfaces can't be created
			bool system_interface = false;
			error = check_system_interface(name, &system_interface);
			if (error) {
				SRP_LOG_ERRMSG("check_system_interface error");
				error = -1;
				goto out;
			}

			if (system_interface || strcmp(type, "eth") == 0 || strcmp(type, "lo") == 0) {
				SRP_LOG_ERR("Can't create non-virtual interface %s of type: %s", name, type);
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
					SRP_LOG_ERR("rtnl_link_add error (%d): %s", error, nl_geterror(error));
					goto out;
				}
			}

			// in order to add ipv4/ipv6 options we first have to create the interface
			// and then get its interface index, because we need it inside add_interface_ipv4/ipv6
			// and don't want to set it manually...
			nl_cache_free(cache);
			error = rtnl_link_alloc_cache(socket, AF_UNSPEC, &cache);
			if (error != 0) {
				SRP_LOG_ERR("rtnl_link_alloc_cache error (%d): %s", error, nl_geterror(error));
				goto out;
			}

			old = rtnl_link_get_by_name(cache, name);
			old_vlan_qinq = rtnl_link_get_by_name(cache, second_vlan_name);

			if (old != NULL) {
				error = add_interface_ipv4(&ld->links[i], old, request, rtnl_link_get_ifindex(old));
				if (error != 0) {
					SRP_LOG_ERRMSG("add_interface_ipv4 error");
					goto out;
				}

				error = add_interface_ipv6(&ld->links[i], old, request, rtnl_link_get_ifindex(old));
				if (error != 0) {
					SRP_LOG_ERRMSG("add_interface_ipv6 error");
					goto out;
				}

				error = rtnl_link_change(socket, old, request, 0);
				if (error != 0) {
					SRP_LOG_ERR("rtnl_link_change error (%d): %s", error, nl_geterror(error));
					goto out;
				}
			}

			if (old_vlan_qinq != NULL) {
				error = add_interface_ipv4(&ld->links[i], old_vlan_qinq, request, rtnl_link_get_ifindex(old_vlan_qinq));
				if (error != 0) {
					SRP_LOG_ERRMSG("add_interface_ipv4 error");
					goto out;
				}

				error = add_interface_ipv6(&ld->links[i], old_vlan_qinq, request, rtnl_link_get_ifindex(old_vlan_qinq));
				if (error != 0) {
					SRP_LOG_ERRMSG("add_interface_ipv6 error");
					goto out;
				}

				error = rtnl_link_change(socket, old_vlan_qinq, request, 0);
				if (error != 0) {
					SRP_LOG_ERR("rtnl_link_change error (%d): %s", error, nl_geterror(error));
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
			struct nl_addr *local_addr = nl_addr_alloc(32);

			error = nl_addr_parse(addr_list->addr[j].ip, AF_INET, &local_addr);
			if (error != 0) {
				SRP_LOG_ERR("nl_addr_parse error (%d): %s", error, nl_geterror(error));
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
				SRP_LOG_ERR("rtnl_addr_delete error (%d): %s", error, nl_geterror(error));
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
	if (strcmp(if_name, "lo") != 0) {
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
	}

	// set mtu
	if (ipv4->mtu != 0) {
		rtnl_link_set_mtu(req, ipv4->mtu);
	}

	// address list
	socket = nl_socket_alloc();
	if (socket == NULL) {
		SRP_LOG_ERRMSG("nl_socket_alloc error: invalid socket");
		goto out;
	}

	if ((error = nl_connect(socket, NETLINK_ROUTE)) != 0) {
		SRP_LOG_ERR("nl_connect error (%d): %s", error, nl_geterror(error));
		goto out;
	}

	for (uint i = 0; i < addr_ls->count; i++) {
		if (addr_ls->addr[i].ip == NULL) { // ip was deleted
			continue;
		}
		r_addr = rtnl_addr_alloc();
		local_addr = nl_addr_alloc(32);
		error = nl_addr_parse(addr_ls->addr[i].ip, AF_INET, &local_addr);
		if (error != 0) {
			rtnl_addr_put(r_addr);
			nl_addr_put(local_addr);
			SRP_LOG_ERR("nl_addr_parse error (%d): %s", error, nl_geterror(error));
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
		local_addr = nl_addr_alloc(32);
		ll_addr = nl_addr_alloc(32);

		error = nl_addr_parse(neigh_ls->nbor[i].ip, AF_INET, &local_addr);
		if (error != 0) {
			nl_addr_put(ll_addr);
			nl_addr_put(local_addr);
			rtnl_neigh_put(neigh);
			SRP_LOG_ERR("nl_addr_parse error (%d): %s", error, nl_geterror(error));
			goto out;
		}

		error = nl_addr_parse(neigh_ls->nbor[i].phys_addr, AF_LLC, &ll_addr);
		if (error != 0) {
			nl_addr_put(ll_addr);
			nl_addr_put(local_addr);
			rtnl_neigh_put(neigh);
			SRP_LOG_ERR("nl_addr_parse error (%d): %s", error, nl_geterror(error));
			goto out;
		}

		rtnl_neigh_set_ifindex(neigh, if_idx);

		// set dst and ll addr
		rtnl_neigh_set_lladdr(neigh, ll_addr);
		rtnl_neigh_set_dst(neigh, local_addr);

		error = rtnl_link_alloc_cache(socket, AF_UNSPEC, &cache);
		if (error != 0) {
			SRP_LOG_ERR("rtnl_link_alloc_cache error (%d): %s", error, nl_geterror(error));
			nl_addr_put(ll_addr);
			nl_addr_put(local_addr);
			rtnl_neigh_put(neigh);
			goto out;
		}

		struct rtnl_neigh *tmp_neigh = rtnl_neigh_get(cache, if_idx, local_addr);
		int neigh_oper = NLM_F_CREATE;

		if (tmp_neigh != NULL) {
			// if the neighbor already exists, replace it
			// otherwise create it (NLM_F_CREATE)
			neigh_oper = NLM_F_REPLACE;
		}

		error = rtnl_neigh_add(socket, neigh, neigh_oper);
		if (error != 0) {
			SRP_LOG_ERR("rtnl_neigh_add error (%d): %s", error, nl_geterror(error));
			nl_addr_put(ll_addr);
			nl_addr_put(local_addr);
			rtnl_neigh_put(neigh);
			goto out;
		}

		nl_addr_put(ll_addr);
		nl_addr_put(local_addr);
		rtnl_neigh_put(neigh);
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
				struct nl_addr *local_addr = nl_addr_alloc(32);

				error = nl_addr_parse(addr_list->addr[j].ip, AF_INET6, &local_addr);
				if (error != 0) {
					SRP_LOG_ERR("nl_addr_parse error (%d): %s", error, nl_geterror(error));
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
					SRP_LOG_ERR("rtnl_addr_delete error (%d): %s", error, nl_geterror(error));
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
		error = write_to_proc_file(ipv6_base, if_name, "mtu", ipv6->ip_data.mtu);
		if (error != 0) {
			goto out;
		}
	}

	// address list
	socket = nl_socket_alloc();
	if (socket == NULL) {
		SRP_LOG_ERRMSG("nl_socket_alloc error: invalid socket");
		goto out;
	}

	if ((error = nl_connect(socket, NETLINK_ROUTE)) != 0) {
		SRP_LOG_ERR("nl_connect error (%d): %s", error, nl_geterror(error));
		goto out;
	}

	for (uint i = 0; i < addr_ls->count; i++) {
		if (addr_ls->addr[i].ip == NULL) { // ip was deleted
			continue;
		}

		r_addr = rtnl_addr_alloc();
		local_addr = nl_addr_alloc(32);

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
		local_addr = nl_addr_alloc(32);
		ll_addr = nl_addr_alloc(32);

		error = nl_addr_parse(neigh_ls->nbor[i].ip, AF_INET6, &local_addr);
		if (error != 0) {
			nl_addr_put(ll_addr);
			nl_addr_put(local_addr);
			rtnl_neigh_put(neigh);
			goto out;
		}

		error = nl_addr_parse(neigh_ls->nbor[i].phys_addr, AF_LLC, &ll_addr);
		if (error != 0) {
			nl_addr_put(ll_addr);
			nl_addr_put(local_addr);
			rtnl_neigh_put(neigh);
			goto out;
		}

		rtnl_neigh_set_ifindex(neigh, if_idx);

		// set dst and ll addr
		rtnl_neigh_set_lladdr(neigh, ll_addr);
		rtnl_neigh_set_dst(neigh, local_addr);

		error = rtnl_link_alloc_cache(socket, AF_UNSPEC, &cache);
		if (error != 0) {
			SRP_LOG_ERR("rtnl_link_alloc_cache error (%d): %s", error, nl_geterror(error));
			nl_addr_put(ll_addr);
			nl_addr_put(local_addr);
			rtnl_neigh_put(neigh);
			goto out;
		}

		struct rtnl_neigh *tmp_neigh = rtnl_neigh_get(cache, if_idx, local_addr);
		int neigh_oper = NLM_F_CREATE;

		if (tmp_neigh != NULL) {
			// if the neighbor already exists, replace it
			// otherwise create it (NLM_F_CREATE)
			neigh_oper = NLM_F_REPLACE;
		}

		error = rtnl_neigh_add(socket, neigh, neigh_oper);
		if (error != 0) {
			SRP_LOG_ERR("rtnl_neigh_add error (%d): %s", error, nl_geterror(error));
			nl_addr_put(ll_addr);
			nl_addr_put(local_addr);
			rtnl_neigh_put(neigh);
			goto out;
		}

		nl_addr_put(ll_addr);
		nl_addr_put(local_addr);
		rtnl_neigh_put(neigh);
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
			struct nl_addr *dst_addr = nl_addr_alloc(32);
			struct rtnl_neigh *neigh = rtnl_neigh_alloc();

			error = nl_addr_parse(nbor_list->nbor[i].ip, addr_ver, &dst_addr);
			if (error != 0) {
				SRP_LOG_ERR("nl_addr_parse error (%d): %s", error, nl_geterror(error));
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
				SRP_LOG_ERR("rtnl_neigh_delete error (%d): %s", error, nl_geterror(error));
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

static int create_vlan_qinq(char *name, char *parent_interface, uint16_t outer_vlan_id, uint16_t second_vlan_id)
{
	int error = 0;
	char cmd[CMD_LEN] = {0};

	// e.g.: # ip link add link eth0 name eth0.10 type vlan id 10 protocol 802.1ad
	error = snprintf(cmd, sizeof(cmd), "ip link add link %s name %s type vlan id %d protocol 802.1ad", parent_interface, name, outer_vlan_id);
	if (error < 0) {
		SRP_LOG_ERRMSG("snprintf error");
		return -1;
	}

	error = system(cmd);

	// e.g.: # ip link add link eth0.10 name eth0.10.20 type vlan id 20
	error = snprintf(cmd, sizeof(cmd), "ip link add link %s name %s.%d type vlan id %d", name, name, second_vlan_id, second_vlan_id);
	if (error < 0) {
		SRP_LOG_ERRMSG("snprintf error");
		return -1;
	}

	error = system(cmd);

	return 0;
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
		SRP_LOG_WRN("could not execute %s", all_devices_cmd);
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
		SRP_LOG_WRN("could not execute %s", check_system_devices_cmd);
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
		SRP_LOG_ERRMSG("snprintf failed");
		goto out;
	}

	// snprintf returns return the number of bytes that are written
	// reset error to 0
	error = 0;

	fptr = fopen((const char *) tmp_buffer, "w");

	if (fptr != NULL) {
		fprintf(fptr, "%d", val);
		fclose(fptr);
	} else {
		SRP_LOG_ERR("failed to open %s: %s", tmp_buffer, strerror(errno));
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
	char tmp_val[2] = {0};

	error = snprintf(tmp_buffer, sizeof(tmp_buffer), "%s/%s/%s", dir_path, interface, fn);
	if (error < 0) {
		// snprintf error
		SRP_LOG_ERRMSG("snprintf failed");
		goto out;
	}

	// snprintf returns return the number of bytes that are written
	// reset error to 0
	error = 0;

	fptr = fopen((const char *) tmp_buffer, "r");

	if (fptr != NULL) {
		fgets(tmp_val, sizeof(tmp_val), fptr);

		*val = atoi(tmp_val);

		fclose(fptr);
	} else {
		SRP_LOG_ERR("failed to open %s: %s", tmp_buffer, strerror(errno));
		error = -1;
		goto out;
	}

out:
	return error;
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
		SRP_LOG_ERRMSG("snprintf failed");
		goto out;
	}

	// snprintf returns return the number of bytes that are written
	// reset error to 0
	error = 0;

	fptr = fopen((const char *) tmp_buffer, "r");

	if (fptr != NULL) {
		fgets(tmp_val, sizeof(tmp_val), fptr);

		*val = atoi(tmp_val);

		fclose(fptr);
	} else {
		SRP_LOG_ERR("failed to open %s: %s", tmp_buffer, strerror(errno));
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

	socket = nl_socket_alloc();
	if (socket == NULL) {
		SRP_LOG_ERRMSG("nl_socket_alloc error: invalid socket");
		goto error_out;
	}

	error = nl_connect(socket, NETLINK_ROUTE);
	if (error != 0) {
		SRP_LOG_ERR("nl_connect error (%d): %s", error, nl_geterror(error));
		goto error_out;
	}

	error = rtnl_link_alloc_cache(socket, AF_UNSPEC, &cache);
	if (error != 0) {
		SRP_LOG_ERR("rtnl_link_alloc_cache error (%d): %s", error, nl_geterror(error));
		goto error_out;
	}

	link = (struct rtnl_link *) nl_cache_get_first(cache);

	while (link != NULL) {
		name = rtnl_link_get_name(link);
		if (name == NULL) {
			SRP_LOG_ERRMSG("rtnl_link_get_name error");
			goto error_out;
		}

		error = get_interface_description(session, name, &description);
		if (error != 0) {
			SRP_LOG_ERRMSG("get_interface_description error");
			// don't return in case of error
			// some interfaces may not have a description already set (wlan0, etc.)
		}

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
				SRP_LOG_ERRMSG("read_from_sys_file error");
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
		} else if (IF_OPER_DOWN == tmp_enabled ) {
			enabled = "false";
		}

		// mtu
		mtu = rtnl_link_get_mtu(link);

		snprintf(tmp_buffer, sizeof(tmp_buffer), "%u", mtu);

		// vlan
		if (rtnl_link_is_vlan(link)) {
			// parent interface
			int parent_index = rtnl_link_get_link(link);
			parent_interface = rtnl_link_i2name(cache, parent_index, parent_buffer, MAX_IF_NAME_LEN);

			// outer vlan id
			vlan_id = (uint16_t)rtnl_link_vlan_get_id(link);
			if (vlan_id <= 0) {
				SRP_LOG_ERRMSG("couldn't get vlan ID");
				goto error_out;
			}

			// check if vlan_id in name, if it is this is the QinQ interface, skip it
			char *first = NULL;
			char *second = NULL;

			first = strchr(name, '.');
			second = strchr(first+1, '.');

			if (second != 0) {
				link = (struct rtnl_link *) nl_cache_get_next((struct nl_object *) link);
				continue;
			}
		}

		error = link_data_list_add(ld, name);
		if (error != 0) {
			SRP_LOG_ERRMSG("link_data_list_add error");
			goto error_out;
		}

		if (description != NULL) {
			error = link_data_list_set_description(ld, name, description);
			if (error != 0) {
				SRP_LOG_ERRMSG("link_data_list_set_description error");
				goto error_out;
			}
		}

		if (type != NULL) {
			error = link_data_list_set_type(ld, name, type);
			if (error != 0) {
				SRP_LOG_ERRMSG("link_data_list_set_type error");
				goto error_out;
			}
		}

		error = link_data_list_set_enabled(ld, name, enabled);
		if (error != 0) {
			SRP_LOG_ERRMSG("link_data_list_set_enabled error");
			goto error_out;
		}

		if (parent_interface != 0) {
			error = link_data_list_set_parent(ld, name, parent_interface);
			if (error != 0) {
				SRP_LOG_ERRMSG("link_data_list_set_parent error");
				goto error_out;
			}
		}

		if (vlan_id != 0) {
			error = link_data_list_set_outer_vlan_id(ld, name, vlan_id);
			if (error != 0) {
				SRP_LOG_ERRMSG("link_data_list_set_outer_vlan_id error");
				goto error_out;
			}
		}

		int if_index = rtnl_link_get_ifindex(link);

		// neighbors
		error = rtnl_neigh_alloc_cache(socket, &neigh_cache);
		if (error != 0) {
			SRP_LOG_ERR("rtnl_neigh_alloc_cache error (%d): %s", error, nl_geterror(error));
			goto error_out;
		}

		int neigh_count = nl_cache_nitems(neigh_cache);

		struct nl_object *nl_neigh_object;
		nl_neigh_object = nl_cache_get_first(neigh_cache);

		for (int i=0; i < neigh_count; i++) {
			struct nl_addr *nl_dst_addr = rtnl_neigh_get_dst((struct rtnl_neigh *) nl_neigh_object);

			char *dst_addr = nl_addr2str(nl_dst_addr, dst_addr_str, sizeof(dst_addr_str));
			if (dst_addr == NULL) {
				SRP_LOG_ERRMSG("nl_addr2str error");
				goto error_out;
			}

			struct rtnl_neigh *neigh = rtnl_neigh_get(neigh_cache, if_index, nl_dst_addr);

			if (neigh != NULL) {
				// get neigh state
				int neigh_state = rtnl_neigh_get_state(neigh);

				// skip neighs with no arp state
				if (NUD_NOARP == neigh_state) {
					nl_neigh_object = nl_cache_get_next(nl_neigh_object);
					continue;
				}

				int cur_neigh_index = rtnl_neigh_get_ifindex(neigh);

				if (if_index != cur_neigh_index) {
					nl_neigh_object = nl_cache_get_next(nl_neigh_object);
					continue;
				}

				struct nl_addr *ll_addr = rtnl_neigh_get_lladdr(neigh);

				char *ll_addr_s = nl_addr2str(ll_addr, ll_addr_str, sizeof(ll_addr_str));
				if (NULL == ll_addr_s) {
					SRP_LOG_ERRMSG("nl_addr2str error");
					goto error_out;
				}

				// check if ipv4 or ipv6
				addr_family = rtnl_neigh_get_family(neigh);

				if (addr_family == AF_INET) {
					error = link_data_list_add_ipv4_neighbor(&link_data_list, name, dst_addr, ll_addr_s);
					if (error != 0) {
						SRP_LOG_ERR("link_data_list_add_ipv4_neighbor error (%d) : %s", error, strerror(error));
						goto error_out;
					}
				} else if (addr_family == AF_INET6) {
					error = link_data_list_add_ipv6_neighbor(&link_data_list, name, dst_addr, ll_addr_s);
					if (error != 0) {
						SRP_LOG_ERR("link_data_list_add_ipv6_neighbor error (%d) : %s", error, strerror(error));
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
			SRP_LOG_ERR("rtnl_addr_alloc_cache error (%d): %s", error, nl_geterror(error));
			goto error_out;
		}

		// get ipv4 and ipv6 addresses
		int addr_count = nl_cache_nitems(addr_cache);

		struct nl_object *nl_object;
		nl_object = nl_cache_get_first(addr_cache);

		addr = (struct rtnl_addr *) nl_object;

		for (int i=0; i < addr_count; i++) {
			struct nl_addr *nl_addr_local = rtnl_addr_get_local(addr);
			if (nl_addr_local == NULL) {
				SRP_LOG_ERRMSG("rtnl_addr_get_local error");
				goto error_out;
			}

			int cur_if_index = rtnl_addr_get_ifindex(addr);

			if (if_index != cur_if_index) {
				nl_object = nl_cache_get_next(nl_object);
				addr = (struct rtnl_addr *) nl_object;
				continue;
			}

			const char*addr_s = nl_addr2str(nl_addr_local, addr_str, sizeof(addr_str));
			if (NULL == addr_s) {
				SRP_LOG_ERRMSG("nl_addr2str error");
				goto error_out;
			}

			char *str = xstrdup(addr_s);

			// get address
			char *token = strtok(str, "/");
			if (token == NULL) {
				SRP_LOG_ERRMSG("couldn't parse ip address");

				FREE_SAFE(str);
				goto error_out;
			}

			char *address = xstrdup(token);

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
				error = link_data_list_add_ipv4_address(&link_data_list, name, address, subnet, ip_subnet_type_prefix_length);
				if (error != 0) {
					SRP_LOG_ERR("link_data_list_add_ipv4_address error (%d) : %s", error, strerror(error));

					FREE_SAFE(str);
					FREE_SAFE(address);
					FREE_SAFE(subnet);
					goto error_out;
				}

				if (mtu > 0) {
					error = link_data_list_set_ipv4_mtu(&link_data_list, name, tmp_buffer);
					if (error != 0) {
						SRP_LOG_ERR("link_data_list_set_ipv4_mtu error (%d) : %s", error, strerror(error));

						FREE_SAFE(str);
						FREE_SAFE(address);
						FREE_SAFE(subnet);
						goto error_out;
					}
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
					SRP_LOG_ERR("link_data_list_set_ipv4_forwarding error (%d) : %s", error, strerror(error));

					FREE_SAFE(str);
					FREE_SAFE(address);
					FREE_SAFE(subnet);
					goto error_out;
				}

			} else if (addr_family == AF_INET6) {
				// ipv6
				error = link_data_list_add_ipv6_address(&link_data_list, name, address, subnet);
				if (error != 0) {
					SRP_LOG_ERR("link_data_list_add_ipv6_address error (%d) : %s", error, strerror(error));

					FREE_SAFE(str);
					FREE_SAFE(address);
					FREE_SAFE(subnet);
					goto error_out;
				}

				if (mtu > 0) {
					error = link_data_list_set_ipv6_mtu(&link_data_list, name, tmp_buffer);
					if (error != 0) {
						SRP_LOG_ERR("link_data_list_set_ipv6_mtu error (%d) : %s", error, strerror(error));

						FREE_SAFE(str);
						FREE_SAFE(address);
						FREE_SAFE(subnet);
						goto error_out;
					}
				}

				// enabled
				const char *ipv6_base = "/proc/sys/net/ipv6/conf";

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
					SRP_LOG_ERR("link_data_list_set_ipv6_enabled error (%d) : %s", error, strerror(error));

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
					SRP_LOG_ERR("link_data_list_set_ipv6_forwarding error (%d) : %s", error, strerror(error));

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
	sr_val_t *val = {0};

	// conjure description path for this interface
	// /ietf-interfaces:interfaces/interface[name='test_interface']/description
	error = snprintf(path_buffer, sizeof(path_buffer) / sizeof(char), "%s[name=\"%s\"]/description", INTERFACE_LIST_YANG_PATH, name);
	if (error < 0) {
		SRP_LOG_ERRMSG("snprintf error");
		goto error_out;
	}

	// get the interface description value 
	error = sr_get_item(session, path_buffer, 0, &val);
	if (error != SR_ERR_OK) {
		SRP_LOG_ERR("sr_get_item error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	if (strlen(val->data.string_val) > 0) {
		*description = val->data.string_val;
	}

	return 0;

error_out:
	return -1;
}

static int interfaces_state_data_cb(sr_session_ctx_t *session, const char *module_name, const char *path, const char *request_xpath, uint32_t request_id, struct lyd_node **parent, void *private_data)
{
	int error = SR_ERR_OK;
	const struct ly_ctx *ly_ctx = NULL;
	struct nl_sock *socket = NULL;
	struct nl_cache *cache = NULL;
	struct rtnl_link *link = NULL;
	struct nl_addr *addr = NULL;
	struct rtnl_tc *tc = NULL;
	struct rtnl_qdisc *qdisc = NULL;

	int32_t tmp_if_index = 0;
	uint64_t tmp_len = 0;
	struct rtnl_link *tmp_link = NULL;

	char tmp_buffer[PATH_MAX] = {0};
	char xpath_buffer[PATH_MAX] = {0};
	char interface_path_buffer[PATH_MAX] = {0};

	if_state_t *tmp_ifs = NULL;

	unsigned int mtu = 0;

	struct {
		char *name;
		char *description;
		char *type;
		char *enabled;
		char *link_up_down_trap_enable;
		char *admin_status;
		const char *oper_status;
		struct tm *last_change;
		int32_t if_index;
		char *phys_address;
		struct {
			char *masters[LD_MAX_LINKS];
			uint32_t count;
		} higher_layer_if;
		uint64_t speed;
		struct {
			char *discontinuity_time;
			uint64_t in_octets;
			uint64_t in_unicast_pkts;
			uint64_t in_broadcast_pkts;
			uint64_t in_multicast_pkts;
			uint32_t in_discards;
			uint32_t in_errors;
			uint32_t in_unknown_protos;
			uint64_t out_octets;
			uint64_t out_unicast_pkts;
			uint64_t out_broadcast_pkts;
			uint64_t out_multicast_pkts;
			uint32_t out_discards;
			uint32_t out_errors;
		} statistics;
	} interface_data = {0};

	typedef struct {
		char *slave_name;
		char *master_names[LD_MAX_LINKS];
		uint32_t count;
	} master_t;

	typedef struct {
		master_t masters[LD_MAX_LINKS];
		uint32_t count;
	} master_list_t;

	master_list_t master_list = {0};

	typedef struct {
		char *master_name;
		char *slave_names[LD_MAX_LINKS];
		uint32_t count;
	} slave_t;

	typedef struct {
		slave_t slaves[LD_MAX_LINKS];
		uint32_t count;
	} slave_list_t;

	slave_list_t slave_list = {0};

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
		ly_ctx = sr_get_context(sr_session_get_connection(session));
		if (ly_ctx == NULL) {
			error = SR_ERR_CALLBACK_FAILED;
			goto error_out;
		}
		*parent = lyd_new_path(NULL, ly_ctx, request_xpath, NULL, 0, 0);
	}

	socket = nl_socket_alloc();
	if (socket == NULL) {
		SRP_LOG_ERRMSG("nl_socket_alloc error: invalid socket");
		goto error_out;
	}

	if ((error = nl_connect(socket, NETLINK_ROUTE)) != 0) {
		SRP_LOG_ERR("nl_connect error (%d): %s", error, nl_geterror(error));
		goto error_out;
	}

	error = rtnl_link_alloc_cache(socket, AF_UNSPEC, &cache);
	if (error != 0) {
		SRP_LOG_ERR("rtnl_link_alloc_cache error (%d): %s", error, nl_geterror(error));
		goto error_out;
	}

	// collect all master interfaces
	link = (struct rtnl_link *) nl_cache_get_first(cache);

	while (link != NULL) {
		char *slave_name = rtnl_link_get_name(link);

		// higher-layer-if
		tmp_if_index = rtnl_link_get_master(link);
		while (tmp_if_index) {
			tmp_link = rtnl_link_get(cache, tmp_if_index);

			char *master_name = rtnl_link_get_name(tmp_link);

			tmp_len = strlen(master_name);

			interface_data.higher_layer_if.masters[interface_data.higher_layer_if.count] = xstrndup(master_name, tmp_len);

			interface_data.higher_layer_if.count++;

			tmp_if_index = rtnl_link_get_master(tmp_link);
		}

		if (interface_data.higher_layer_if.count > 0) {
			for (uint64_t i = 0; i < interface_data.higher_layer_if.count; i++) {
				char *master_name = interface_data.higher_layer_if.masters[i];

				tmp_len = strlen(slave_name);
				master_list.masters[master_list.count].slave_name = xstrndup(slave_name, tmp_len);

				tmp_len = strlen(master_name);
				master_list.masters[master_list.count].master_names[i] = xstrndup(master_name, tmp_len);
			}

			master_list.masters[master_list.count].count = interface_data.higher_layer_if.count;
			master_list.count++;
		}

		// continue to next link node
		link = (struct rtnl_link *) nl_cache_get_next((struct nl_object *) link);
	}

	// collect all slave interfaces
	link = (struct rtnl_link *) nl_cache_get_first(cache);

	while (link != NULL) {
		// lower-layer-if
		char *if_name = rtnl_link_get_name(link);

		bool break_out = false;
		for (uint64_t i = 0; i < master_list.count; i++) {
			for (uint64_t j = 0; j < master_list.masters[i].count; j++) {
				if (strcmp(master_list.masters[i].slave_name, master_list.masters[i].master_names[j]) == 0) {
					continue;
				}

				if (strcmp(master_list.masters[i].master_names[j], if_name) == 0) {
					SRP_LOG_DBG("Slave of interface %s: %s", if_name, master_list.masters[i].slave_name);

					tmp_len = strlen(if_name);
					slave_list.slaves[slave_list.count].master_name = xstrndup(if_name, tmp_len);

					tmp_len = strlen(master_list.masters[i].slave_name);
					slave_list.slaves[slave_list.count].slave_names[i] = xstrndup(master_list.masters[i].slave_name, tmp_len);

					slave_list.slaves[slave_list.count].count++;

					break_out = true;
					break;
				}
			}
			if (break_out) {
				slave_list.count++;
				break;
			}
		}
		// continue to next link node
		link = (struct rtnl_link *) nl_cache_get_next((struct nl_object *) link);
	}

	link = (struct rtnl_link *) nl_cache_get_first(cache);
	qdisc = rtnl_qdisc_alloc();

	while (link != NULL) {
		// get tc and set the link
		tc = TC_CAST(qdisc);
		rtnl_tc_set_link(tc, link);

		interface_data.name = rtnl_link_get_name(link);

		link_data_t *l = data_list_get_by_name(&link_data_list, interface_data.name);
		interface_data.description = l->description;

		interface_data.type = rtnl_link_get_type(link);
		interface_data.enabled = rtnl_link_get_operstate(link) == IF_OPER_UP ? "enabled" : "disabled";
		// interface_data.link_up_down_trap_enable = ?
		// interface_data.admin_status = ?
		interface_data.oper_status = OPER_STRING_MAP[rtnl_link_get_operstate(link)];
		interface_data.if_index = rtnl_link_get_ifindex(link);

		// last-change field
		tmp_ifs = if_state_list_get_by_if_name(&if_state_changes, interface_data.name);
		interface_data.last_change = (tmp_ifs->last_change != 0) ? localtime(&tmp_ifs->last_change) : NULL;

		// get_system_boot_time will change the struct tm which is held in interface_data.last_change if it's not NULL
		char system_time[DATETIME_BUF_SIZE] = {0};
		if (interface_data.last_change != NULL) {
			// convert it to human readable format here
			strftime(system_time, sizeof system_time, "%FT%TZ", interface_data.last_change);
		}

		// mac address
		addr = rtnl_link_get_addr(link);
		interface_data.phys_address = xmalloc(sizeof(char) * (MAC_ADDR_MAX_LENGTH + 1));
		nl_addr2str(addr, interface_data.phys_address, MAC_ADDR_MAX_LENGTH);
		interface_data.phys_address[MAC_ADDR_MAX_LENGTH] = 0;

		interface_data.speed = rtnl_tc_get_stat(tc, RTNL_TC_RATE_BPS);

		// stats:
		char system_boot_time[DATETIME_BUF_SIZE] = {0};
		error = get_system_boot_time(system_boot_time);
		if (error != 0) {
			SRP_LOG_ERR("get_system_boot_time error: %s", strerror(errno));
			goto error_out;
		}
		interface_data.statistics.discontinuity_time = system_boot_time;

		// gather interface statistics that are not accessable via netlink
		nic_stats_t nic_stats = {0};
		error = get_nic_stats(interface_data.name, &nic_stats);
		if (error != 0) {
			SRP_LOG_ERR("get_nic_stats error: %s", strerror(errno));
		}

		// Rx
		interface_data.statistics.in_octets = rtnl_link_get_stat(link, RTNL_LINK_RX_BYTES);
		interface_data.statistics.in_broadcast_pkts = nic_stats.rx_broadcast;
		interface_data.statistics.in_multicast_pkts = rtnl_link_get_stat(link, RTNL_LINK_MULTICAST);
		interface_data.statistics.in_unicast_pkts = nic_stats.rx_packets - nic_stats.rx_broadcast - interface_data.statistics.in_multicast_pkts;

		interface_data.statistics.in_discards = (uint32_t) rtnl_link_get_stat(link, RTNL_LINK_RX_DROPPED);
		interface_data.statistics.in_errors = (uint32_t) rtnl_link_get_stat(link, RTNL_LINK_RX_ERRORS);
		interface_data.statistics.in_unknown_protos = (uint32_t) rtnl_link_get_stat(link, RTNL_LINK_IP6_INUNKNOWNPROTOS);

		// Tx
		interface_data.statistics.out_octets = rtnl_link_get_stat(link, RTNL_LINK_TX_BYTES);
		interface_data.statistics.out_broadcast_pkts = nic_stats.tx_broadcast;
		interface_data.statistics.out_multicast_pkts = nic_stats.tx_multicast;
		interface_data.statistics.out_unicast_pkts = nic_stats.tx_packets - nic_stats.tx_broadcast - nic_stats.tx_multicast;

		interface_data.statistics.out_discards = (uint32_t) rtnl_link_get_stat(link, RTNL_LINK_TX_DROPPED);
		interface_data.statistics.out_errors = (uint32_t) rtnl_link_get_stat(link, RTNL_LINK_TX_ERRORS);

		snprintf(interface_path_buffer, sizeof(interface_path_buffer) / sizeof(char), "%s[name=\"%s\"]", INTERFACE_LIST_YANG_PATH, rtnl_link_get_name(link));

		// name
		error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/name", interface_path_buffer);
		if (error < 0) {
			goto error_out;
		}
		SRP_LOG_DBG("%s = %s", xpath_buffer, interface_data.name);
		lyd_new_path(*parent, ly_ctx, xpath_buffer, interface_data.name, LYD_ANYDATA_STRING, 0);

		// description
		error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/description", interface_path_buffer);
		if (error < 0) {
			goto error_out;
		}
		SRP_LOG_DBG("%s = %s", xpath_buffer, interface_data.description);
		lyd_new_path(*parent, ly_ctx, xpath_buffer, interface_data.description, LYD_ANYDATA_STRING, 0);

		// type
		error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/type", interface_path_buffer);
		if (error < 0) {
			goto error_out;
		}
		SRP_LOG_DBG("%s = %s", xpath_buffer, interface_data.type);
		lyd_new_path(*parent, ly_ctx, xpath_buffer, interface_data.type, LYD_ANYDATA_STRING, 0);

		// oper-status
		error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/oper-status", interface_path_buffer);
		if (error < 0) {
			goto error_out;
		}
		SRP_LOG_DBG("%s = %s", xpath_buffer, interface_data.oper_status);
		lyd_new_path(*parent, ly_ctx, xpath_buffer, (char *) interface_data.oper_status, LYD_ANYDATA_STRING, 0);

		// last-change -> only if changed at one point
		if (interface_data.last_change != NULL) {
			error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/last-change", interface_path_buffer);
			if (error < 0) {
				goto error_out;
			}
			SRP_LOG_DBG("%s = %s", xpath_buffer, interface_data.type);
			lyd_new_path(*parent, ly_ctx, xpath_buffer, system_time, LYD_ANYDATA_STRING, 0);
		} else {
			// default value of last-change should be system boot time
			error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/last-change", interface_path_buffer);
			if (error < 0) {
				goto error_out;
			}
			SRP_LOG_DBG("%s = %s", xpath_buffer, interface_data.type);
			lyd_new_path(*parent, ly_ctx, xpath_buffer, system_boot_time, LYD_ANYDATA_STRING, 0);
		}

		// if-index
		error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/if-index", interface_path_buffer);
		if (error < 0) {
			goto error_out;
		}
		SRP_LOG_DBG("%s = %d", xpath_buffer, interface_data.if_index);
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%u", interface_data.if_index);
		lyd_new_path(*parent, ly_ctx, xpath_buffer, tmp_buffer, LYD_ANYDATA_STRING, 0);

		// phys-address
		error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/phys-address", interface_path_buffer);
		if (error < 0) {
			goto error_out;
		}
		SRP_LOG_DBG("%s = %s", xpath_buffer, interface_data.phys_address);
		lyd_new_path(*parent, ly_ctx, xpath_buffer, interface_data.phys_address, LYD_ANYDATA_STRING, 0);

		// speed
		error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/speed", interface_path_buffer);
		if (error < 0) {
			goto error_out;
		}
		SRP_LOG_DBG("%s = %s", xpath_buffer, interface_data.speed);
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%lu", interface_data.speed);
		lyd_new_path(*parent, ly_ctx, xpath_buffer, tmp_buffer, LYD_ANYDATA_STRING, 0);

		// higher-layer-if
		for (uint64_t i = 0; i < master_list.count; i++) {
			if (strcmp(interface_data.name, master_list.masters[i].slave_name) == 0) {
				for (uint64_t j = 0; j < master_list.masters[i].count; j++) {

					error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/higher-layer-if", interface_path_buffer);
					if (error < 0) {
						goto error_out;
					}

					SRP_LOG_DBG("%s += %s", xpath_buffer, master_list.masters[i].master_names[j]);
					lyd_new_path(*parent, ly_ctx, xpath_buffer, master_list.masters[i].master_names[j], LYD_ANYDATA_STRING, 0);

					FREE_SAFE(interface_data.higher_layer_if.masters[i]);
				}
			}
		}

		// lower-layer-if
		for (uint64_t i = 0; i < slave_list.count; i++) {
			if (strcmp(interface_data.name, slave_list.slaves[i].master_name) == 0) {
				for (uint64_t j = 0; j < slave_list.slaves[i].count; j++) {

					error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/lower-layer-if", interface_path_buffer);
					if (error < 0) {
						goto error_out;
					}

					SRP_LOG_DBG("%s += %s", xpath_buffer, slave_list.slaves[i].slave_names[j]);
					lyd_new_path(*parent, ly_ctx, xpath_buffer, slave_list.slaves[i].slave_names[j], LYD_ANYDATA_STRING, 0);
				}
			}
		}

		// ietf-ip
		// mtu
		mtu = rtnl_link_get_mtu(link);

		// list of ipv4 addresses
		for (uint32_t i = 0; i < link_data_list.count; i++) {
			if (link_data_list.links[i].name != NULL) { // in case we deleted a link it will be NULL
				if (strcmp(link_data_list.links[i].name, interface_data.name ) == 0) {

					// enabled
					// TODO

					// forwarding
					uint8_t ipv4_forwarding = link_data_list.links[i].ipv4.forwarding;

					error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/ietf-ip:ipv4/forwarding", interface_path_buffer);
					if (error < 0) {
						goto error_out;
					}

					SRP_LOG_DBG("%s = %d", xpath_buffer, ipv4_forwarding);
					lyd_new_path(*parent, ly_ctx, xpath_buffer, ipv4_forwarding == 0 ? "false" : "true", LYD_ANYDATA_STRING, 0);

					uint32_t ipv4_addr_count = link_data_list.links[i].ipv4.addr_list.count;

					for (uint32_t j = 0; j < ipv4_addr_count; j++) {
						if (link_data_list.links[i].ipv4.addr_list.addr[j].ip != NULL) { // in case we deleted an ip address it will be NULL
							char *ip_addr = link_data_list.links[i].ipv4.addr_list.addr[j].ip;

							if (mtu > 0) {
								error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/ietf-ip:ipv4/mtu", interface_path_buffer);
								if (error < 0) {
									goto error_out;
								}
								snprintf(tmp_buffer, sizeof(tmp_buffer), "%u", mtu);
								SRP_LOG_DBG("%s = %s", xpath_buffer, tmp_buffer);
								lyd_new_path(*parent, ly_ctx, xpath_buffer, tmp_buffer, LYD_ANYDATA_STRING, 0);
							}

							error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/ietf-ip:ipv4/address[ip='%s']/ip", interface_path_buffer, ip_addr);
							if (error < 0) {
								goto error_out;
							}
							// ip
							SRP_LOG_DBG("%s = %s", xpath_buffer, link_data_list.links[i].ipv4.addr_list.addr[j].ip);
							lyd_new_path(*parent, ly_ctx, xpath_buffer, link_data_list.links[i].ipv4.addr_list.addr[j].ip, LYD_ANYDATA_STRING, 0);

							// subnet
							snprintf(tmp_buffer, sizeof(tmp_buffer), "%u", link_data_list.links[i].ipv4.addr_list.addr[j].subnet);

							error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/ietf-ip:ipv4/address[ip='%s']/prefix-length", interface_path_buffer, ip_addr);
							if (error < 0) {
								goto error_out;
							}

							SRP_LOG_DBG("%s = %s", xpath_buffer, tmp_buffer);
							lyd_new_path(*parent, ly_ctx, xpath_buffer, tmp_buffer, LYD_ANYDATA_STRING, 0);
						}
					}

					// neighbors
					uint32_t ipv4_neigh_count = link_data_list.links[i].ipv4.nbor_list.count;

					for (uint32_t j = 0; j < ipv4_neigh_count; j++) {
						if (link_data_list.links[i].ipv4.nbor_list.nbor[j].ip != NULL) { // in case we deleted an ip address it will be NULL
							char *ip_addr = link_data_list.links[i].ipv4.nbor_list.nbor[j].ip;

							error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/ietf-ip:ipv4/neighbor[ip='%s']/ip", interface_path_buffer, ip_addr);
							if (error < 0) {
								goto error_out;
							}
							// ip
							SRP_LOG_DBG("%s = %s", xpath_buffer, link_data_list.links[i].ipv4.nbor_list.nbor[j].ip);
							lyd_new_path(*parent, ly_ctx, xpath_buffer, link_data_list.links[i].ipv4.nbor_list.nbor[j].ip, LYD_ANYDATA_STRING, 0);

							// link-layer-address
							error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/ietf-ip:ipv4/neighbor[ip='%s']/link-layer-address", interface_path_buffer, ip_addr);
							if (error < 0) {
								goto error_out;
							}

							SRP_LOG_DBG("%s = %s", xpath_buffer, link_data_list.links[i].ipv4.nbor_list.nbor[j].phys_addr);
							lyd_new_path(*parent, ly_ctx, xpath_buffer, link_data_list.links[i].ipv4.nbor_list.nbor[j].phys_addr, LYD_ANYDATA_STRING, 0);
						}
					}
				}
			}
		}

		// list of ipv6 addresses
		for (uint32_t i = 0; i < link_data_list.count; i++) {
			if (link_data_list.links[i].name != NULL) { // in case we deleted a link it will be NULL
				if (strcmp(link_data_list.links[i].name, interface_data.name ) == 0) {

					// enabled
					uint8_t ipv6_enabled = link_data_list.links[i].ipv6.ip_data.enabled;

					error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/ietf-ip:ipv6/enabled", interface_path_buffer);
					if (error < 0) {
						goto error_out;
					}

					SRP_LOG_DBG("%s = %d", xpath_buffer, ipv6_enabled);
					lyd_new_path(*parent, ly_ctx, xpath_buffer, ipv6_enabled == 0 ? "false" : "true", LYD_ANYDATA_STRING, 0);

					// forwarding
					uint8_t ipv6_forwarding = link_data_list.links[i].ipv6.ip_data.forwarding;

					error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/ietf-ip:ipv6/forwarding", interface_path_buffer);
					if (error < 0) {
						goto error_out;
					}

					SRP_LOG_DBG("%s = %d", xpath_buffer, ipv6_forwarding);
					lyd_new_path(*parent, ly_ctx, xpath_buffer, ipv6_forwarding == 0 ? "false" : "true", LYD_ANYDATA_STRING, 0);

					uint32_t ipv6_addr_count = link_data_list.links[i].ipv6.ip_data.addr_list.count;

					for (uint32_t j = 0; j < ipv6_addr_count; j++) {
						if (link_data_list.links[i].ipv6.ip_data.addr_list.addr[j].ip != NULL) { // in case we deleted an ip address it will be NULL
							char *ip_addr = link_data_list.links[i].ipv6.ip_data.addr_list.addr[j].ip;

							// mtu
							if (mtu > 0 && ip_addr != NULL) {
								error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/ietf-ip:ipv6/mtu", interface_path_buffer);
								if (error < 0) {
									goto error_out;
								}
								snprintf(tmp_buffer, sizeof(tmp_buffer), "%u", mtu);
								SRP_LOG_DBG("%s = %s", xpath_buffer, tmp_buffer);
								lyd_new_path(*parent, ly_ctx, xpath_buffer, tmp_buffer, LYD_ANYDATA_STRING, 0);
							}

							error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/ietf-ip:ipv6/address[ip='%s']/ip", interface_path_buffer, ip_addr);
							if (error < 0) {
								goto error_out;
							}
							// ip
							SRP_LOG_DBG("%s = %s", xpath_buffer, link_data_list.links[i].ipv6.ip_data.addr_list.addr[j].ip);
							lyd_new_path(*parent, ly_ctx, xpath_buffer, link_data_list.links[i].ipv6.ip_data.addr_list.addr[j].ip, LYD_ANYDATA_STRING, 0);

							// subnet
							snprintf(tmp_buffer, sizeof(tmp_buffer), "%u", link_data_list.links[i].ipv6.ip_data.addr_list.addr[j].subnet);

							error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/ietf-ip:ipv6/address[ip='%s']/prefix-length", interface_path_buffer, ip_addr);
							if (error < 0) {
								goto error_out;
							}

							SRP_LOG_DBG("%s = %s", xpath_buffer, tmp_buffer);
							lyd_new_path(*parent, ly_ctx, xpath_buffer, tmp_buffer, LYD_ANYDATA_STRING, 0);
						}
					}

					// neighbors
					uint32_t ipv6_neigh_count = link_data_list.links[i].ipv6.ip_data.nbor_list.count;

					for (uint32_t j = 0; j < ipv6_neigh_count; j++) {
						if (link_data_list.links[i].ipv6.ip_data.nbor_list.nbor[j].ip != NULL) { // in case we deleted an ip address it will be NULL
							char *ip_addr = link_data_list.links[i].ipv6.ip_data.nbor_list.nbor[j].ip;

							error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/ietf-ip:ipv6/neighbor[ip='%s']/ip", interface_path_buffer, ip_addr);
							if (error < 0) {
								goto error_out;
							}
							// ip
							SRP_LOG_DBG("%s = %s", xpath_buffer, link_data_list.links[i].ipv6.ip_data.nbor_list.nbor[j].ip);
							lyd_new_path(*parent, ly_ctx, xpath_buffer, link_data_list.links[i].ipv6.ip_data.nbor_list.nbor[j].ip, LYD_ANYDATA_STRING, 0);

							// link-layer-address
							error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/ietf-ip:ipv6/neighbor[ip='%s']/link-layer-address", interface_path_buffer, ip_addr);
							if (error < 0) {
								goto error_out;
							}

							SRP_LOG_DBG("%s = %s", xpath_buffer, link_data_list.links[i].ipv6.ip_data.nbor_list.nbor[j].phys_addr);
							lyd_new_path(*parent, ly_ctx, xpath_buffer, link_data_list.links[i].ipv6.ip_data.nbor_list.nbor[j].phys_addr, LYD_ANYDATA_STRING, 0);
						}
					}
				}
			}
		}

		// stats:
		// discontinuity-time
		error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/statistics/discontinuity-time", interface_path_buffer);
		if (error < 0) {
			goto error_out;
		}
		SRP_LOG_DBG("%s = %s", xpath_buffer, interface_data.statistics.discontinuity_time);
		lyd_new_path(*parent, ly_ctx, xpath_buffer, interface_data.statistics.discontinuity_time, LYD_ANYDATA_STRING, 0);

		// in-octets
		error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/statistics/in-octets", interface_path_buffer);
		if (error < 0) {
			goto error_out;
		}
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%lu", interface_data.statistics.in_octets);
		lyd_new_path(*parent, ly_ctx, xpath_buffer, tmp_buffer, LYD_ANYDATA_STRING, 0);

		// in-unicast-pkts
		error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/statistics/in-unicast-pkts", interface_path_buffer);
		if (error < 0) {
			goto error_out;
		}
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%lu", interface_data.statistics.in_unicast_pkts);
		lyd_new_path(*parent, ly_ctx, xpath_buffer, tmp_buffer, LYD_ANYDATA_STRING, 0);

		// in-broadcast-pkts
		error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/statistics/in-broadcast-pkts", interface_path_buffer);
		if (error < 0) {
			goto error_out;
		}
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%lu", interface_data.statistics.in_broadcast_pkts);
		lyd_new_path(*parent, ly_ctx, xpath_buffer, tmp_buffer, LYD_ANYDATA_STRING, 0);

		// in-multicast-pkts
		error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/statistics/in-multicast-pkts", interface_path_buffer);
		if (error < 0) {
			goto error_out;
		}
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%lu", interface_data.statistics.in_multicast_pkts);
		lyd_new_path(*parent, ly_ctx, xpath_buffer, tmp_buffer, LYD_ANYDATA_STRING, 0);

		// in-discards
		error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/statistics/in-discards", interface_path_buffer);
		if (error < 0) {
			goto error_out;
		}
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%u", interface_data.statistics.in_discards);
		lyd_new_path(*parent, ly_ctx, xpath_buffer, tmp_buffer, LYD_ANYDATA_STRING, 0);

		// in-errors
		error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/statistics/in-errors", interface_path_buffer);
		if (error < 0) {
			goto error_out;
		}
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%u", interface_data.statistics.in_errors);
		lyd_new_path(*parent, ly_ctx, xpath_buffer, tmp_buffer, LYD_ANYDATA_STRING, 0);

		// in-unknown-protos
		error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/statistics/in-unknown-protos", interface_path_buffer);
		if (error < 0) {
			goto error_out;
		}
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%u", interface_data.statistics.in_unknown_protos);
		lyd_new_path(*parent, ly_ctx, xpath_buffer, tmp_buffer, LYD_ANYDATA_STRING, 0);

		// out-octets
		error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/statistics/out-octets", interface_path_buffer);
		if (error < 0) {
			goto error_out;
		}
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%lu", interface_data.statistics.out_octets);
		lyd_new_path(*parent, ly_ctx, xpath_buffer, tmp_buffer, LYD_ANYDATA_STRING, 0);

		// out-unicast-pkts
		error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/statistics/out-unicast-pkts", interface_path_buffer);
		if (error < 0) {
			goto error_out;
		}
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%lu", interface_data.statistics.out_unicast_pkts);
		lyd_new_path(*parent, ly_ctx, xpath_buffer, tmp_buffer, LYD_ANYDATA_STRING, 0);

		// out-broadcast-pkts
		error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/statistics/out-broadcast-pkts", interface_path_buffer);
		if (error < 0) {
			goto error_out;
		}
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%lu", interface_data.statistics.out_broadcast_pkts);
		lyd_new_path(*parent, ly_ctx, xpath_buffer, tmp_buffer, LYD_ANYDATA_STRING, 0);

		// out-multicast-pkts
		error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/statistics/out-multicast-pkts", interface_path_buffer);
		if (error < 0) {
			goto error_out;
		}
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%lu", interface_data.statistics.out_multicast_pkts);
		lyd_new_path(*parent, ly_ctx, xpath_buffer, tmp_buffer, LYD_ANYDATA_STRING, 0);

		// out-discards
		error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/statistics/out-discards", interface_path_buffer);
		if (error < 0) {
			goto error_out;
		}
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%u", interface_data.statistics.out_discards);
		lyd_new_path(*parent, ly_ctx, xpath_buffer, tmp_buffer, LYD_ANYDATA_STRING, 0);

		// out-errors
		error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/statistics/out-errors", interface_path_buffer);
		if (error < 0) {
			goto error_out;
		}
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%u", interface_data.statistics.out_errors);
		lyd_new_path(*parent, ly_ctx, xpath_buffer, tmp_buffer, LYD_ANYDATA_STRING, 0);

		// free all allocated data
		FREE_SAFE(interface_data.phys_address);

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
	for (uint64_t i = 0; i < master_list.count; i++) {
		for (uint64_t j = 0; j < master_list.masters[i].count; j++) {
			FREE_SAFE(master_list.masters[i].master_names[j]);
		}
		FREE_SAFE(master_list.masters[i].slave_name);
	}

	for (uint64_t i = 0; i < slave_list.count; i++) {
		for (uint64_t j = 0; j < slave_list.slaves[i].count; j++) {
			FREE_SAFE(slave_list.slaves[i].slave_names[j]);
		}
		FREE_SAFE(slave_list.slaves[i].master_name);
	}

	nl_socket_free(socket);
	return error ? SR_ERR_CALLBACK_FAILED : SR_ERR_OK;
}

static char *interfaces_xpath_get(const struct lyd_node *node)
{
	char *xpath_node = NULL;
	char *xpath_leaflist_open_bracket = NULL;
	size_t xpath_trimed_size = 0;
	char *xpath_trimed = NULL;

	if (node->schema->nodetype == LYS_LEAFLIST) {
			xpath_node = lyd_path(node);
			xpath_leaflist_open_bracket = strrchr(xpath_node, '[');
			if (xpath_leaflist_open_bracket == NULL) {
					return xpath_node;
			}

			xpath_trimed_size = (size_t) xpath_leaflist_open_bracket - (size_t) xpath_node + 1;
			xpath_trimed = xcalloc(1, xpath_trimed_size);
			strncpy(xpath_trimed, xpath_node, xpath_trimed_size - 1);
			xpath_trimed[xpath_trimed_size - 1] = '\0';

			FREE_SAFE(xpath_node);

			return xpath_trimed;
	} else {
			return lyd_path(node);
	}
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

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, 1);

	socket = nl_socket_alloc();
	if (socket == NULL) {
		SRP_LOG_ERRMSG("nl_socket_alloc error: invalid socket");
		return -1;
	}

	if ((error = nl_connect(socket, NETLINK_ROUTE)) != 0) {
		SRP_LOG_ERR("nl_connect error (%d): %s", error, nl_geterror(error));
		goto error_out;
	}

	error = rtnl_link_alloc_cache(socket, AF_UNSPEC, &cache);
	if (error != 0) {
		SRP_LOG_ERR("rtnl_link_alloc_cache error (%d): %s", error, nl_geterror(error));
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
		SRP_LOG_ERR("nl_cache_mngr_alloc failed (%d): %s", error, nl_geterror(error));
		goto error_out;
	}

	error = nl_cache_mngr_add(link_manager, "route/link", cache_change_cb, NULL, &link_cache);
	if (error != 0) {
		SRP_LOG_ERR("nl_cache_mngr_add failed (%d): %s", error, nl_geterror(error));
		goto error_out;
	}

	pthread_create(&manager_thread, NULL, manager_thread_cb, 0);

	pthread_detach(manager_thread);

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

	SRP_LOG_DBGMSG("entered cb function for a link manager");

	link = (struct rtnl_link *) nl_cache_get_first(cache);

	while (link != NULL) {
		name = rtnl_link_get_name(link);
		tmp_st = if_state_list_get_by_if_name(&if_state_changes, name);
		tmp_state = rtnl_link_get_operstate(link);

		if (tmp_state != tmp_st->state) {
			SRP_LOG_DBG("Interface %s changed operstate from %d to %d", name, tmp_st->state, tmp_state);
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
		SRP_LOG_ERR("sr_connect error (%d): %s", error, sr_strerror(error));
		goto out;
	}

	error = sr_session_start(connection, SR_DS_RUNNING, &session);
	if (error) {
		SRP_LOG_ERR("sr_session_start error (%d): %s", error, sr_strerror(error));
		goto out;
	}

	error = sr_plugin_init_cb(session, &private_data);
	if (error) {
		SRP_LOG_ERRMSG("sr_plugin_init_cb error");
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
	SRP_LOG_INF("Sigint called, exiting...");
	exit_application = 1;
}

#endif
