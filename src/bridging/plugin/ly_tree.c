#include <netlink/cache.h>
#include <netlink/route/link.h>
#include <netlink/route/link/vlan.h>

#include <sysrepo.h>

#include "common.h"
#include "ly_tree.h"

int bridging_ly_tree_add_bridges_container(const struct ly_ctx *ly_ctx, struct lyd_node *parent, struct lyd_node **bridges_node)
{
	LY_ERR ly_error = LY_SUCCESS;

	ly_error = lyd_new_path(parent, ly_ctx, BRIDGING_BRIDGES_CONTAINER_YANG_PATH, NULL, 0, bridges_node);
	if (ly_error != LY_SUCCESS) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "Unable to create the bridges container libyang node");
		return -1;
	}

	return 0;
}

int bridging_ly_tree_add_bridge(const struct ly_ctx *ly_ctx, struct lyd_node *bridges_container, struct rtnl_link *bridge_link, struct lyd_node **bridge_node)
{
	LY_ERR ly_error = LY_SUCCESS;
	int error = 0;
	char path_buffer[PATH_MAX] = {0};

	SRPLG_LOG_DBG(PLUGIN_NAME, "Bridge name = %s", rtnl_link_get_name(bridge_link));
	error = snprintf(path_buffer, sizeof(path_buffer), "bridge[name=\"%s\"]", rtnl_link_get_name(bridge_link));
	if (error < 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() failed (%d)", error);
		return -1;
	}

	ly_error = lyd_new_path(bridges_container, ly_ctx, path_buffer, rtnl_link_get_name(bridge_link), LYD_NEW_PATH_UPDATE, bridge_node);
	if (ly_error != LY_SUCCESS) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "lyd_new_path() for \"bridge\" failed (%d): %s", ly_error, ly_errmsg(ly_ctx));
		return -1;
	}

	return 0;
}

int bridging_ly_tree_add_bridge_address(const struct ly_ctx *ly_ctx, struct lyd_node *bridge_node, struct rtnl_link *bridge_link)
{
	LY_ERR ly_error = LY_SUCCESS;

	char mac_addr_buffer[18] = {0};

	// get mac address and convert libnl format to libyang format
	nl_addr2str(rtnl_link_get_addr(bridge_link), mac_addr_buffer, sizeof(mac_addr_buffer));
	mac_address_nl_to_ly(mac_addr_buffer);

	// add node
	SRPLG_LOG_DBG(PLUGIN_NAME, "Bridge address = %s", mac_addr_buffer);
	ly_error = lyd_new_path(bridge_node, ly_ctx, "address", mac_addr_buffer, 0, NULL);
	if (ly_error != LY_SUCCESS) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "lyd_new_path() for \"address\" failed (%d): %s", ly_error, ly_errmsg(ly_ctx));
		return -1;
	}

	return 0;
}

int bridging_ly_tree_add_bridge_type(const struct ly_ctx *ly_ctx, struct lyd_node *bridge_node, struct rtnl_link *bridge_link)
{
	LY_ERR ly_error = LY_SUCCESS;

	const char *type = "customer-vlan-bridge";

	SRPLG_LOG_DBG(PLUGIN_NAME, "Bridge type = %s", type);
	ly_error = lyd_new_path(bridge_node, ly_ctx, "bridge-type", type, 0, NULL);
	if (ly_error != LY_SUCCESS) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "lyd_new_path() for bridge:\"type\" failed (%d): %s", ly_error, ly_errmsg(ly_ctx));
		return -1;
	}

	return 0;
}

int bridging_ly_tree_add_bridge_ports(const struct ly_ctx *ly_ctx, struct lyd_node *bridge_node, struct rtnl_link *bridge_link, struct nl_cache *link_cache)
{
	int error = 0;
	LY_ERR ly_error = LY_SUCCESS;

	struct rtnl_link *link_iter = NULL;
	uint16_t ports = 0;

	// max 4095 = 4 chars + null terminator; but make sure UINT16_MAX (65535) + null terminator fits
	char str_buffer[6] = {0};

	// count number of interfaces whose master is the bridge link
	link_iter = (struct rtnl_link *) nl_cache_get_first(link_cache);
	while (link_iter) {
		if (rtnl_link_get_master(link_iter) == rtnl_link_get_ifindex(bridge_link)) {
			++ports;
		}
		link_iter = (struct rtnl_link *) nl_cache_get_next((struct nl_object *) link_iter);
	}

	if (ports == 0)
		return 0;

	error = snprintf(str_buffer, sizeof(str_buffer), "%hu", ports);
	if (error < 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() failed (%d)", error);
		return -1;
	}

	SRPLG_LOG_DBG(PLUGIN_NAME, "Bridge ports = %s", str_buffer);
	ly_error = lyd_new_path(bridge_node, ly_ctx, "ports", str_buffer, 0, NULL);
	if (ly_error != LY_SUCCESS) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "lyd_new_path() for bridge:\"ports\" failed (%d): %s", ly_error, ly_errmsg(ly_ctx));
		return -1;
	}

	return 0;
}

int bridging_ly_tree_add_bridge_components(const struct ly_ctx *ly_ctx, struct lyd_node *bridge_node, struct rtnl_link *bridge_link, struct nl_cache *link_cache)
{
	LY_ERR ly_error = LY_SUCCESS;
	int error = 0;

	struct rtnl_link *link_iter = NULL;
	uint32_t components = 0;

	// max 4294967295 = 10 chars + null terminator
	char str_buffer[11] = {0};

	// count number of interfaces whose master is the bridge link
	link_iter = (struct rtnl_link *) nl_cache_get_first(link_cache);
	while (link_iter) {
		if (rtnl_link_get_master(link_iter) == rtnl_link_get_ifindex(bridge_link) && rtnl_link_is_vlan(link_iter)) {
			++components;
		}
		link_iter = (struct rtnl_link *) nl_cache_get_next((struct nl_object *) link_iter);
	}

	error = snprintf(str_buffer, sizeof(str_buffer), "%u", components);
	if (error < 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() failed (%d)", error);
		return -1;
	}

	SRPLG_LOG_DBG(PLUGIN_NAME, "Bridge components = %s", str_buffer);
	ly_error = lyd_new_path(bridge_node, ly_ctx, "components", str_buffer, 0, NULL);
	if (ly_error != LY_SUCCESS) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "lyd_new_path() for bridge:\"components\" failed (%d): %s", ly_error, ly_errmsg(ly_ctx));
		return -1;
	}

	return 0;
}

int bridging_ly_tree_add_bridge_component(const struct ly_ctx *ly_ctx, struct lyd_node *bridge_node, struct rtnl_link *component_link, struct lyd_node **component_node)
{
	LY_ERR ly_error = LY_SUCCESS;
	int error = 0;
	char path_buffer[PATH_MAX] = {0};

	SRPLG_LOG_DBG(PLUGIN_NAME, "Component name = %s", rtnl_link_get_name(component_link));
	error = snprintf(path_buffer, sizeof(path_buffer), "component[name=\"%s\"]", rtnl_link_get_name(component_link));
	if (error < 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() failed (%d)", error);
		return -1;
	}

	ly_error = lyd_new_path(bridge_node, ly_ctx, path_buffer, rtnl_link_get_name(component_link), LYD_NEW_PATH_UPDATE, component_node);
	if (ly_error != LY_SUCCESS) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "lyd_new_path() for component:\"name\" failed (%d): %s", ly_error, ly_errmsg(ly_ctx));
		return -1;
	}

	return 0;
}

int bridging_ly_tree_add_bridge_component_id(const struct ly_ctx *ly_ctx, struct lyd_node *component_node, struct rtnl_link *component_link)
{
	int error = 0;
	LY_ERR ly_error = LY_SUCCESS;

	// max 4294967295 = 10 chars + null terminator
	char id_buffer[10] = {0};
	uint32_t component_id = (uint32_t) rtnl_link_vlan_get_id(component_link);

	error = snprintf(id_buffer, sizeof(id_buffer), "%u", component_id);
	if (error < 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() failed (%d)", error);
		return -1;
	}

	SRPLG_LOG_DBG(PLUGIN_NAME, "Bridge component id = %s", id_buffer);
	ly_error = lyd_new_path(component_node, ly_ctx, "id", id_buffer, 0, NULL);
	if (ly_error != LY_SUCCESS) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "lyd_new_path() for component:\"id\" failed (%d): %s", ly_error, ly_errmsg(ly_ctx));
		return -1;
	}

	return 0;
}

int bridging_ly_tree_add_bridge_component_type(const struct ly_ctx *ly_ctx, struct lyd_node *component_node, struct rtnl_link *component_link)
{
	LY_ERR ly_error = LY_SUCCESS;

	// TODO: configure type variable to match VLAN type
	const char *type = "c-vlan-component";

	SRPLG_LOG_DBG(PLUGIN_NAME, "Bridge component type = %s", type);
	ly_error = lyd_new_path(component_node, ly_ctx, "type", type, 0, NULL);
	if (ly_error != LY_SUCCESS) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "lyd_new_path() for component:\"type\" failed (%d): %s", ly_error, ly_errmsg(ly_ctx));
		return -1;
	}

	return 0;
}

int bridging_ly_tree_add_bridge_component_address(const struct ly_ctx *ly_ctx, struct lyd_node *component_node, struct rtnl_link *component_link)
{
	LY_ERR ly_error = LY_SUCCESS;

	char mac_addr_buffer[18] = {0};

	// get mac address and convert libnl format to libyang format
	nl_addr2str(rtnl_link_get_addr(component_link), mac_addr_buffer, sizeof(mac_addr_buffer));
	mac_address_nl_to_ly(mac_addr_buffer);

	// add node
	SRPLG_LOG_DBG(PLUGIN_NAME, "Bridge component address = %s", mac_addr_buffer);
	ly_error = lyd_new_path(component_node, ly_ctx, "address", mac_addr_buffer, 0, NULL);
	if (ly_error != LY_SUCCESS) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "lyd_new_path() for component:\"address\" failed (%d): %s", ly_error, ly_errmsg(ly_ctx));
		return -1;
	}

	return 0;
}

int bridging_ly_tree_add_general_bridge_vlan_info(const struct ly_ctx *ly_ctx, struct lyd_node *component_node)
{
	LY_ERR ly_error = LY_SUCCESS;

	// set bridge-vlan version to 2 because the Linux bridging implementation supports
	// MST (Multiple Spanning Tree), otherwise the version would be 1 (12.10.1.3 of IEEE Std 802.1Q-2018)
	ly_error = lyd_new_path(component_node, ly_ctx, "bridge-vlan/version", "2", 0, NULL);
	if (ly_error != LY_SUCCESS) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "lyd_new_path() failed (%d): %s", ly_error, ly_errmsg(ly_ctx));
		return -1;
	}
	ly_error = lyd_new_path(component_node, ly_ctx, "bridge-vlan/max-vids", "4094", 0, NULL);
	if (ly_error != LY_SUCCESS) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "lyd_new_path() failed (%d): %s", ly_error, ly_errmsg(ly_ctx));
		return -1;
	}
	ly_error = lyd_new_path(component_node, ly_ctx, "bridge-vlan/override-default-pvid", "true", 0, NULL);
	if (ly_error != LY_SUCCESS) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "lyd_new_path() failed (%d): %s", ly_error, ly_errmsg(ly_ctx));
		return -1;
	}
	return 0;
}
