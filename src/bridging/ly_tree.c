#include "ly_tree.h"
#include "netlink/cache.h"
#include "netlink/route/link.h"
#include "netlink/route/link/vlan.h"
#include <bridging/common.h>

#include <sysrepo.h>

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

	char mac_addr_buffer[100] = {0};
	char *tmp_ptr = mac_addr_buffer;

	// get mac address and convert libnl format to libyang format
	nl_addr2str(rtnl_link_get_addr(bridge_link), mac_addr_buffer, sizeof(mac_addr_buffer));
	tmp_ptr = mac_addr_buffer;
	while ((tmp_ptr = strchr(tmp_ptr, ':'))) {
		*tmp_ptr = '-';
	}

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
	int error = 0;
	LY_ERR ly_error = LY_SUCCESS;

	SRPLG_LOG_DBG(PLUGIN_NAME, "Bridge type = %s", "customer-vlan-bridge");
	ly_error = lyd_new_path(bridge_node, ly_ctx, "bridge-type", "customer-vlan-bridge", 0, NULL);
	if (ly_error != LY_SUCCESS) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "lyd_new_path() for \"type\" failed (%d): %s", ly_error, ly_errmsg(ly_ctx));
		return -1;
	}

	return error;
}

int bridging_ly_tree_add_bridge_ports(const struct ly_ctx *ly_ctx, struct lyd_node *bridge_node, struct rtnl_link *bridge_link, struct nl_cache *link_cache)
{
	LY_ERR ly_error = LY_SUCCESS;

	struct rtnl_link *link_iter = NULL;
	uint16_t ports = 0;

	// max 4096 = 4 chars + null terminator
	char str_buffer[5] = {0};

	// count number of interfaces whose master is the bridge link
	link_iter = (struct rtnl_link *) nl_cache_get_first(link_cache);
	while (link_iter) {
		if (rtnl_link_get_master(link_iter) == rtnl_link_get_ifindex(bridge_link)) {
			++ports;
		}
		link_iter = (struct rtnl_link *) nl_cache_get_next((struct nl_object *) link_iter);
	}

	snprintf(str_buffer, sizeof(str_buffer), "%d", ports);

	SRPLG_LOG_DBG(PLUGIN_NAME, "Bridge ports = %s", str_buffer);
	ly_error = lyd_new_path(bridge_node, ly_ctx, "ports", str_buffer, 0, NULL);
	if (ly_error != LY_SUCCESS) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "lyd_new_path() for \"ports\" failed (%d): %s", ly_error, ly_errmsg(ly_ctx));
		return -1;
	}

	return 0;
}

int bridging_ly_tree_add_bridge_components(const struct ly_ctx *ly_ctx, struct lyd_node *bridge_node, struct rtnl_link *bridge_link, struct nl_cache *link_cache)
{
	LY_ERR ly_error = LY_SUCCESS;

	struct rtnl_link *link_iter = NULL;
	uint16_t components = 0;

	// max 4096 = 4 chars + null terminator
	char str_buffer[5] = {0};

	// count number of interfaces whose master is the bridge link
	link_iter = (struct rtnl_link *) nl_cache_get_first(link_cache);
	while (link_iter) {
		if (rtnl_link_get_master(link_iter) == rtnl_link_get_ifindex(bridge_link) && rtnl_link_is_vlan(link_iter)) {
			++components;
		}
		link_iter = (struct rtnl_link *) nl_cache_get_next((struct nl_object *) link_iter);
	}

	snprintf(str_buffer, sizeof(str_buffer), "%d", components);

	SRPLG_LOG_DBG(PLUGIN_NAME, "Bridge ports = %s", str_buffer);
	ly_error = lyd_new_path(bridge_node, ly_ctx, "components", str_buffer, 0, NULL);
	if (ly_error != LY_SUCCESS) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "lyd_new_path() for \"components\" failed (%d): %s", ly_error, ly_errmsg(ly_ctx));
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
		SRPLG_LOG_ERR(PLUGIN_NAME, "lyd_new_path() for \"component\" failed (%d): %s", ly_error, ly_errmsg(ly_ctx));
		return -1;
	}

	return 0;
}