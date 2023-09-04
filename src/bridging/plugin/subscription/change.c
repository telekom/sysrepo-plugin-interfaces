#include <linux/if_ether.h>
#include <linux/if_bridge.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <netlink/addr.h>
#include <netlink/cache.h>
#include <netlink/errno.h>
#include <netlink/route/link/bridge.h>
#include <netlink/route/neighbour.h>
#include <sysrepo_types.h>

#include <sysrepo.h>
#include <sysrepo/xpath.h>
#include <netlink/socket.h>
#include <netlink/route/link.h>
#include <utlist.h>

#include "change.h"
#include "plugin/common.h"
#include "plugin/context.h"
#include "plugin/api/bridge_netlink.h"
#include "plugin/data/vlan/list.h"

// change callback type
typedef int (*bridging_change_cb)(bridging_ctx_t *ctx, sr_session_ctx_t *session, struct nl_sock *socket, const struct lyd_node *node, sr_change_oper_t operation);

int bridge_port_component_name_change_cb(bridging_ctx_t *ctx, sr_session_ctx_t *session, struct nl_sock *socket, const struct lyd_node *node, sr_change_oper_t operation);

// common functionality for iterating changes specified by the given xpath - calls callback function for each iterated node
int apply_change(bridging_ctx_t *ctx, sr_session_ctx_t *session, const char *xpath, bridging_change_cb cb);

int apply_bridge_list_config_changes(bridging_ctx_t *ctx, sr_session_ctx_t *session, sr_change_iter_t *changes_iterator);
int modify_bridge_component(struct nl_sock *socket, char change_path[PATH_MAX], const char *node_name, const char *node_value, sr_change_oper_t operation);
int modify_filtering_database(struct nl_sock *socket, char change_path[PATH_MAX], const char *node_name, const char *node_value, sr_change_oper_t operation);
int modify_filtering_entries(struct nl_sock *socket, char change_path[PATH_MAX], const char *node_name, const char *node_value, sr_change_oper_t operation);
int modify_port_vlan_entries(struct nl_sock *socket, char change_path[PATH_MAX], const char *node_name, const char *node_value, sr_change_oper_t operation);


int bridging_bridge_list_change_cb(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data)
{
	int error = 0;

	// context
	bridging_ctx_t *ctx = (bridging_ctx_t *) private_data;
	sr_change_iter_t *changes_iterator = NULL;

	SRPLG_LOG_INF(PLUGIN_NAME, "Module Name: %s; XPath: %s; Event: %d, Request ID: %u", module_name, xpath, event, request_id);

	if (event == SR_EV_ABORT) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "aborting changes for: %s", xpath);
		goto error_out;
	} else if (event == SR_EV_DONE) {
		error = sr_copy_config(ctx->startup_session, BASE_YANG_MODEL, SR_DS_RUNNING, 0);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "sr_copy_config error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}
	} else if (event == SR_EV_CHANGE) {
		// apply bridge configuration changes to system
		error = sr_get_changes_iter(session, BRIDGING_BRIDGE_LIST_YANG_PATH "//*", &changes_iterator);
		if (error != SR_ERR_OK) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "sr_get_changes_iter() failed (%d): %s", error, sr_strerror(error));
			goto error_out;
		}
		error = apply_bridge_list_config_changes(ctx, session, changes_iterator);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "apply_bridge_list_config_changes() failed (%d)", error);
		}
	}
	goto out;

error_out:
	SRPLG_LOG_ERR(PLUGIN_NAME, "error applying bridge list module changes");
	error = -1;

out:
	return error ? SR_ERR_CALLBACK_FAILED : SR_ERR_OK;
}

int get_changed_node_info(const struct lyd_node *node, char change_path[PATH_MAX], const char **node_name, const char **node_value, sr_change_oper_t operation)
{
	int error = 0;
	error = (lyd_path(node, LYD_PATH_STD, change_path, PATH_MAX) == NULL);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "lyd_path() error: %d", error);
		goto out;
	}

	*node_name = sr_xpath_node_name(change_path);
	*node_value = lyd_get_value(node);

	SRPLG_LOG_DBG(PLUGIN_NAME, "Node Path: %s", change_path);
	char *operation_str = NULL;
	switch (operation) {
		case SR_OP_CREATED:
			operation_str = "created";
			break;
		case SR_OP_DELETED:
			operation_str = "deleted";
			break;
		case SR_OP_MODIFIED:
			operation_str = "modified";
			break;
		case SR_OP_MOVED:
			operation_str = "moved";
			break;
	}
	SRPLG_LOG_DBG(PLUGIN_NAME, "Node name: %s, Value: %s; Operation: %s", *node_name, *node_value, operation_str);
out:
	return error;
}

int apply_bridge_list_config_changes(bridging_ctx_t *ctx, sr_session_ctx_t *session, sr_change_iter_t *changes_iterator)
{
	int error = 0;

	// for iterating over changed nodes
	sr_change_oper_t operation = SR_OP_CREATED;
	const char *prev_value = NULL, *prev_list = NULL;
	int prev_default = 0;
	const struct lyd_node *node = NULL;
	char change_path[PATH_MAX] = {0};
	const char *node_name = NULL;
	const char *node_value = NULL;

	// open netlink socket for applying configuration
	struct nl_sock *socket = nl_socket_alloc();
	if (socket == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "unable to init nl_sock struct...");
		goto error_out;
	}
	error = nl_connect(socket, NETLINK_ROUTE);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nl_connect() failed (%d): %s", error, nl_geterror(error));
		goto error_out;
	}

	SRPLG_LOG_DBG(PLUGIN_NAME, "** Processing node changes... **");
	while (sr_get_change_tree_next(session, changes_iterator, &operation, &node, &prev_value, &prev_list, &prev_default) == SR_ERR_OK) {
		error = get_changed_node_info(node, change_path, &node_name, &node_value, operation);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "get_changed_node_info() failed (%d)", error);
			goto error_out;
		}

		if (node_value == NULL) {
			continue; // skip container nodes
		}

		if (strstr(change_path, "component")) {
			if (strstr(change_path, "filtering-database")) {
				// modify filtering database for a bridge component (frame forwarding and port VLANs)
				error = modify_filtering_database(socket, change_path, node_name, node_value, operation);
				if (error) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "modify_filtering_database() failed (%d)", error);
					goto error_out;
				}
			} else if (strstr(change_path, "permanent-database")) {
				// ignore
			} else if (strstr(change_path, "bridge-vlan")) {
				// ignore
			} else if (strstr(change_path, "bridge-mst")) {
				// ignore
			} else {
				// modify general bridge component configuration
				error = modify_bridge_component(socket, change_path, node_name, node_value, operation);
				if (error) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "modify_bridge_component() failed (%d)", error);
					goto error_out;
				}
			}
		}
	}

	goto out;

error_out:
	error = -1;
out:
	if (socket)
		nl_socket_free(socket);
	sr_free_change_iter(changes_iterator);

	return error;
}

/*
 * A bridge component is a Linux bridge, eg. a c-vlan-component is a
 * bridge with VLAN filtering enabled and with ETH_P_8021Q set as the VLAN protocol.
 * This function modifies the name, address or type of a bridge component.
 */
int modify_bridge_component(struct nl_sock *socket, char change_path[PATH_MAX], const char *node_name, const char *node_value, sr_change_oper_t operation)
{
	int error = 0;
	struct rtnl_link *bridge = NULL;
	struct rtnl_link *bridge_changes = NULL;
	struct nl_addr *address = NULL;
	sr_xpath_ctx_t xpath_ctx = {0};
	char *br_component_name = sr_xpath_key_value(change_path, "component", "name", &xpath_ctx);
	char *address_str = NULL;

	if (strcmp(node_name, "name") == 0) {
		switch (operation) {
			case SR_OP_CREATED:
				// create new bridge interface
				bridge = rtnl_link_bridge_alloc();
				rtnl_link_set_name(bridge, br_component_name);

				error = rtnl_link_add(socket, bridge, NLM_F_CREATE);
				if (error) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_add() failed (%d): %s", error, nl_geterror(error));
					goto error_out;
				}
				break;
			case SR_OP_MODIFIED:
				// name cannot be changed
				break;
			case SR_OP_DELETED:
				// delete bridge whose name matches the given node value
				bridge = rtnl_link_bridge_alloc();
				rtnl_link_set_name(bridge, node_value);

				// delete bridge
				error = rtnl_link_delete(socket, bridge);
				if (error) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_delete() failed (%d): %s", error, nl_geterror(error));
					goto error_out;
				}
				break;
			case SR_OP_MOVED:
				break;
		}
	} else if (strcmp(node_name, "type") == 0) {
		switch (operation) {
			case SR_OP_CREATED:
				// change type of created bridge - same as modified
			case SR_OP_MODIFIED:
				// get bridge interface and set vlan config depending on type
				error = rtnl_link_get_kernel(socket, 0, br_component_name, &bridge);
				if (error) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_get_by_name() failed (%d): %s", error, nl_geterror(error));
					goto error_out;
				}
				int bridge_link_idx = rtnl_link_get_ifindex(bridge);
				bridge_vlan_info_t vlan_config = {0}; // vlan_filtering off (0) by default
				if (strcmp(node_value, "ieee802-dot1q-bridge:c-vlan-component") == 0) {
					vlan_config.vlan_filtering = 1;
					vlan_config.vlan_proto = htons(ETH_P_8021Q);
				} else if (strcmp(node_value, "ieee802-dot1q-bridge:s-vlan-component") == 0) {
					vlan_config.vlan_filtering = 1;
					vlan_config.vlan_proto = htons(ETH_P_8021AD);
				}
				error = bridge_set_vlan_config(socket, bridge_link_idx, &vlan_config);
				if (error) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "bridge_set_vlan_config() failed");
					goto error_out;
				}
				break;
			case SR_OP_DELETED:
				// type cannot be deleted
			case SR_OP_MOVED:
				break;
		}
	} else if (strcmp(node_name, "address") == 0) {
		switch (operation) {
			case SR_OP_CREATED:
				// change address of the interface - same as modified
			case SR_OP_MODIFIED:
				// get bridge interface and set its address
				error = rtnl_link_get_kernel(socket, 0, br_component_name, &bridge);
				if (error) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_get_kernel() failed (%d): %s", error, nl_geterror(error));
					goto error_out;
				}

				// convert to libnl mac address format
				SRPC_SAFE_CALL_PTR(address_str, strdup(node_value), error_out);
				mac_address_ly_to_nl(address_str);
				error = nl_addr_parse(address_str, AF_LLC, &address);
				if (error < 0) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "nl_addr_parse() failed (%d): %s", error, nl_geterror(error));
					goto error_out;
				}

				// configure link for changes
				bridge_changes = rtnl_link_bridge_alloc();
				rtnl_link_set_name(bridge_changes, br_component_name);
				rtnl_link_set_addr(bridge_changes, address);

				error = rtnl_link_change(socket, bridge, bridge_changes, 0);
				if (error < 0) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_change() failed (%d): %s", error, nl_geterror(error));
					goto error_out;
				}
				break;
			case SR_OP_DELETED:
				// address cannot be deleted
			case SR_OP_MOVED:
				break;
		}
	}

error_out:
	if (bridge)
		rtnl_link_put(bridge);
	if (bridge_changes)
		rtnl_link_put(bridge_changes);
	if (address)
		nl_addr_put(address);
	if (address_str)
		free(address_str);
	return error;
}

int modify_filtering_database(struct nl_sock *socket, char change_path[PATH_MAX], const char *node_name, const char *node_value, sr_change_oper_t operation)
{
	int error = 0;
	struct rtnl_link *bridge = NULL;
	sr_xpath_ctx_t xpath_ctx = {0};
	char *bridge_component_name = NULL;
	uint32_t seconds = 0;
	int bridge_link_idx = 0;

	if (strcmp(node_name, "aging-time") == 0) {
		switch (operation) {
			case SR_OP_CREATED:
			case SR_OP_MODIFIED:
				bridge_component_name = sr_xpath_key_value(change_path, "component", "name", &xpath_ctx);
				error = rtnl_link_get_kernel(socket, 0, bridge_component_name, &bridge);
				if (error) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_get_kernel() error: (%d) %s", error, nl_geterror(error));
					goto error_out;
				}
				bridge_link_idx = rtnl_link_get_ifindex(bridge);

				seconds = (uint32_t) strtoul(node_value, NULL, 10);
				error = bridge_set_ageing_time(socket, bridge_link_idx, seconds);
				if (error) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "bridge_set_ageing_time() failed (%d)", error);
					goto error_out;
				}
				break;
			case SR_OP_DELETED:
				break;
			case SR_OP_MOVED:
				break;
		}
	} else if (strstr(change_path, "vlan-registration-entry")) {
		error = modify_port_vlan_entries(socket, change_path, node_name, node_value, operation);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "modify_port_vlan_entries() failed (%d)", error);
			goto error_out;
		}
	} else if (strstr(change_path, "filtering-entry")) {
		error = modify_filtering_entries(socket, change_path, node_name, node_value, operation);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "modify_filtering_entries() failed (%d)", error);
			goto error_out;
		}
	}

error_out:
	if (bridge)
		rtnl_link_put(bridge);
	return error;
}

/*
 * A `vlan-registration-entry` list element specifies how one or more VLANs
 * should be used on a bridge component. The `vids` key contains a list of VLAN IDs
 * for this entry, eg.: `2,5,10-15`. Each entry has a `port-map` list which specifies
 * the ports (`port-ref` nodes) that should process frames from VLANs defined in this entry.
 * The `vlan-transmitted` node defines whether frames should be VLAN tagged or untagged when
 * leaving a specific port.
 */
int modify_port_vlan_entries(struct nl_sock *socket, char change_path[PATH_MAX], const char *node_name, const char *node_value, sr_change_oper_t operation)
{
	int error = 0;
	struct rtnl_link *bridge_port = NULL;
	sr_xpath_ctx_t xpath_ctx = {0};
	uint16_t port_vlan_flags = 0;
	char *vids_str = NULL;
	char *bridge_port_str = NULL;
	int bridge_port_idx = 0;
	struct nl_msg *msg = NULL;
	struct bridge_vlan_info vlan_info = {0};
	bool should_delete = false;

	if (strcmp(node_name, "port-ref") == 0 || strcmp(node_name, "vlan-transmitted") == 0) {
		// Modify VLAN entries on a bridge port.
		// Use xpath keys to determine which port and VLAN IDs to use.
		// NOTE: change_path is modified by sr_xpath_key_value (string termination added after key value)
		bridge_port_str = sr_xpath_key_value(change_path, "port-map", "port-ref", &xpath_ctx);
		bridge_port_idx = (int) strtol(bridge_port_str, NULL, 10);
		error = rtnl_link_get_kernel(socket, bridge_port_idx, NULL, &bridge_port);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_get_kernel() error: (%d) %s", error, nl_geterror(error));
			goto error_out;
		}

		// vids contains comma separted list of vlan ids and ranges, eg.: `1,3,5-10`
		vids_str = sr_xpath_key_value(change_path, "vlan-registration-entry", "vids", &xpath_ctx);
		if (strcmp(node_name, "vlan-transmitted") == 0 && strcmp(node_value, "untagged") == 0) {
			port_vlan_flags |= BRIDGE_VLAN_INFO_UNTAGGED;
		}
		switch (operation) {
			case SR_OP_DELETED:
				if (strcmp(node_name, "port-ref")) {
					break; // delete VLANs only if a `port-ref` node was deleted
				}
				should_delete = true;
				// fall through
			case SR_OP_CREATED:
			case SR_OP_MODIFIED:
				error = bridge_vlan_msg_open(&msg, bridge_port, should_delete);
				if (error) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "bridge_vlan_msg_open() failed (%d)", error);
					goto error_out;
				}
				// for each VLAN ID in vids_str, save it in vlan_info
				// and then add it to the netlink config update message
				while (vids_str) {
					vids_str = vids_str_next_vlan(vids_str, &vlan_info);
					vlan_info.flags |= port_vlan_flags;
					error = bridge_vlan_msg_add_vlan(msg, vlan_info);
					if (error) {
						SRPLG_LOG_ERR(PLUGIN_NAME, "bridge_vlan_msg_add_vlan() failed (%d)", error);
						goto error_out;
					}
				};
				error = bridge_vlan_msg_finalize_and_send(socket, msg);
				if (error) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "bridge_vlan_msg_finalize_and_send() failed (%d)", error);
					goto error_out;
				}
				break;
			case SR_OP_MOVED:
				break;
		}
	}

error_out:
	if (bridge_port)
		rtnl_link_put(bridge_port);
	// free msg?
	return error;
}

/*
 * A `filtering-entry` list element specifies on which port (`port-ref` node)
 * a frame should be forwarded on, based on its destination address and VLAN ID.
 * The `vids` key contains a list of VLAN IDs for this entry, eg.: `2,5,10-15`.
 * The `control-element` node defines whether frames should be forwarded or filtered.
 */
int modify_filtering_entries(struct nl_sock *socket, char change_path[PATH_MAX], const char *node_name, const char *node_value, sr_change_oper_t operation)
{
	int error = 0;
	struct rtnl_neigh *fdb_entry = NULL;
	struct nl_addr *lladdr = NULL;
	sr_xpath_ctx_t xpath_ctx = {0};
	char *vids_str = NULL;
	char *lladdr_str = NULL;
	char *bridge_port_str = NULL;
	int bridge_port_idx = 0;
	struct bridge_vlan_info vlan_info = {0};
	bool should_delete = false;
	bool should_add = false;

	if (strstr(change_path, "static-filtering-entries") && strcmp(node_name, "control-element") == 0) {
		// Modify filtering entry.
		// Use xpath keys to determine bridge port, destination address and VLAN IDs.
		// NOTE: change_path is modified by sr_xpath_key_value (string termination added after found key value)
		bridge_port_str = sr_xpath_key_value(change_path, "port-map", "port-ref", &xpath_ctx);
		bridge_port_idx = (int) strtol(bridge_port_str, NULL, 10);
		lladdr_str = sr_xpath_key_value(change_path, "filtering-entry", "address", &xpath_ctx);
		mac_address_ly_to_nl(lladdr_str);
		// vids contains comma separted list of vlan ids and ranges, eg.: `1,3,5-10`
		vids_str = sr_xpath_key_value(change_path, "filtering-entry", "vids", &xpath_ctx);

		switch (operation) {
			case SR_OP_DELETED:
				if (strcmp(node_value, "forward") == 0) {
					should_delete = true;
				}
				break;
			case SR_OP_CREATED:
				// same as modified
			case SR_OP_MODIFIED:
				if (strcmp(node_value, "forward") == 0) {
					should_add = true;
				} else {
					should_delete = true;
				}
				break;
			case SR_OP_MOVED:
				break;
		}
		if (!should_delete && !should_add) {
			goto error_out; // nothing to do
		}

		fdb_entry = rtnl_neigh_alloc();
		if (fdb_entry == NULL) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_neigh_alloc() failed");
			error = -1;
			goto error_out;
		}
		rtnl_neigh_set_family(fdb_entry, AF_BRIDGE);
		rtnl_neigh_set_ifindex(fdb_entry, bridge_port_idx);

		error = nl_addr_parse(lladdr_str, AF_LLC, &lladdr);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "nl_addr_parse() error: (%d) %s", error, nl_geterror(error));
			goto error_out;
		}
		rtnl_neigh_set_lladdr(fdb_entry, lladdr);

		// TODO: research possible state flags
		rtnl_neigh_set_state(fdb_entry, NUD_NOARP);
		rtnl_neigh_set_flags(fdb_entry, NTF_MASTER); // add neigh to bridge master FDB

		// add or delete a filtering entry for each VLAN ID in vids_str
		// TODO: if there are ranges in vids_str, add every ID in range manually
		while (vids_str) {
			vids_str = vids_str_next_vlan(vids_str, &vlan_info);
			rtnl_neigh_set_vlan(fdb_entry, vlan_info.vid);
			if (should_add) {
				error = rtnl_neigh_add(socket, fdb_entry, NLM_F_CREATE);
				if (error) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_neigh_add() failed (%d) %s", error, nl_geterror(error));
					goto error_out;
				}
			} else {
				error = rtnl_neigh_delete(socket, fdb_entry, 0);
				if (error) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_neigh_delete() failed (%d) %s", error, nl_geterror(error));
					goto error_out;
				}
			}
		}
	}

error_out:
	if (fdb_entry)
		rtnl_neigh_put(fdb_entry);

	if (lladdr)
		nl_addr_put(lladdr);
	return error;
}

int bridge_port_change_cb(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data)
{
	SRPLG_LOG_DBG(PLUGIN_NAME, "in bridge_port_change_cb");
	int error = 0;

	// context
	bridging_ctx_t *ctx = (bridging_ctx_t *) private_data;

	// xpath buffer
	char xpath_buffer[PATH_MAX] = {0};

	SRPLG_LOG_INF(PLUGIN_NAME, "Module Name: %s; XPath: %s; Event: %d, Request ID: %u", module_name, xpath, event, request_id);

	if (event == SR_EV_ABORT) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "aborting changes for: %s", xpath);
		goto error_out;
	} else if (event == SR_EV_DONE) {
		error = sr_copy_config(ctx->startup_session, INTERFACES_YANG_MODEL, SR_DS_RUNNING, 0);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "sr_copy_config error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}
	} else if (event == SR_EV_CHANGE) {
		// component name change
		SRPC_SAFE_CALL_ERR_COND(error, error < 0, snprintf(xpath_buffer, sizeof(xpath_buffer), "%s//bridge-port/component-name", xpath), error_out);

		error = apply_change(ctx, session, xpath_buffer, bridge_port_component_name_change_cb);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "apply_change() for bridge port component name failed: %d", error);
			goto error_out;
		}
	}
	goto out;

error_out:
	SRPLG_LOG_ERR(PLUGIN_NAME, "error applying bridge port module changes");
	error = -1;
out:
	return error ? SR_ERR_CALLBACK_FAILED : SR_ERR_OK;
}

int apply_change(bridging_ctx_t *ctx, sr_session_ctx_t *session, const char *xpath, bridging_change_cb cb)
{
	int error = 0;

	// sysrepo
	sr_change_iter_t *changes_iterator = NULL;
	sr_change_oper_t operation = SR_OP_CREATED;
	const char *prev_value = NULL, *prev_list = NULL;
	int prev_default;

	struct nl_sock *socket = NULL;

	// libyang
	const struct lyd_node *node = NULL;

	SRPLG_LOG_DBG(PLUGIN_NAME, "Getting changes for xpath %s", xpath);

	// connect to libnl

	socket = nl_socket_alloc();
	if (socket == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "unable to init nl_sock struct...");
		goto error_out;
	}

	error = nl_connect(socket, NETLINK_ROUTE);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nl_connect() failed (%d): %s", error, nl_geterror(error));
		goto error_out;
	}

	error = sr_get_changes_iter(session, xpath, &changes_iterator);
	if (error != SR_ERR_OK) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_get_changes_iter() failed (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	while (sr_get_change_tree_next(session, changes_iterator, &operation, &node, &prev_value, &prev_list, &prev_default) == SR_ERR_OK) {
		error = cb(ctx, session, socket, node, operation);
		if (error) {
			goto error_out;
		}
	}

	goto out;

error_out:
	error = -1;
out:
	// libnl
	if (socket)
		nl_socket_free(socket);

	// iterator
	sr_free_change_iter(changes_iterator);

	return error;
}

int bridge_port_component_name_change_cb(bridging_ctx_t *ctx, sr_session_ctx_t *session, struct nl_sock *socket, const struct lyd_node *node, sr_change_oper_t operation)
{
	int error = 0;

	char change_path[PATH_MAX] = {0};
	char tmp_xpath[PATH_MAX] = {0};
	const char *node_name = NULL;
	const char *node_value = NULL;
	const char *interface_name = NULL;

	// libnl
	struct rtnl_link *bridge = NULL;
	struct rtnl_link *bridge_port = NULL;

	sr_xpath_ctx_t xpath_ctx = {0};

	error = (lyd_path(node, LYD_PATH_STD, change_path, sizeof(change_path)) == NULL);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "lyd_path() error: %d", error);
		goto error_out;
	}
	// use temp buffer for xpath operations (sr_xpath_key_value modifies the xpath argument)
	memcpy(tmp_xpath, change_path, sizeof(change_path));

	node_name = sr_xpath_node_name(change_path);
	node_value = lyd_get_value(node);
	interface_name = sr_xpath_key_value(tmp_xpath, "interface", "name", &xpath_ctx);

	assert(strcmp(node_name, "component-name") == 0);

	SRPLG_LOG_DBG(PLUGIN_NAME, "Node Path: %s", change_path);
	SRPLG_LOG_DBG(PLUGIN_NAME, "Node Name: %s", node_name);
	SRPLG_LOG_DBG(PLUGIN_NAME, "Value: %s; Operation: %d", node_value, operation);

	error = rtnl_link_get_kernel(socket, 0, interface_name, &bridge_port);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_get_kernel() error: %d (for bridge-port)", error);
		goto error_out;
	}
	error = rtnl_link_get_kernel(socket, 0, node_value, &bridge);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_get_kernel() error: %d (for bridge)", error);
		goto error_out;
	}

	switch (operation) {
		case SR_OP_CREATED:
			// make the interface a port of the bridge component
			error = rtnl_link_enslave(socket, bridge, bridge_port);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_enslave() failed (%d): %s", error, nl_geterror(error));
				goto error_out;
			}
			break;
		case SR_OP_MODIFIED:
			// release from old bridge and set as port of new bridge component
			error = rtnl_link_release(socket, bridge_port);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_release() failed (%d): %s", error, nl_geterror(error));
				goto error_out;
			}
			error = rtnl_link_enslave(socket, bridge, bridge_port);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_enslave() failed (%d): %s", error, nl_geterror(error));
				goto error_out;
			}
			break;
		case SR_OP_DELETED:
			// release from bridge
			error = rtnl_link_release(socket, bridge_port);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_release() failed (%d): %s", error, nl_geterror(error));
				goto error_out;
			}
			break;
		case SR_OP_MOVED:
			break;
	}

	goto out;

error_out:
	error = -1;

out:
	if (bridge)
		rtnl_link_put(bridge);
	if (bridge_port)
		rtnl_link_put(bridge_port);
	return error;
}
