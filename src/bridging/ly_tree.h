#ifndef BRIDGING_PLUGIN_LY_TREE_H
#define BRIDGING_PLUGIN_LY_TREE_H

// libyang
#include <libyang/tree_data.h>

// libnl
#include <netlink/route/link.h>

// libyang tree creation/manipulation helpers for the bridging plugin
int bridging_ly_tree_add_bridges_container(const struct ly_ctx *ly_ctx, struct lyd_node *parent, struct lyd_node **bridges_node);
int bridging_ly_tree_add_bridge(const struct ly_ctx *ly_ctx, struct lyd_node *bridges_container, struct rtnl_link *bridge_link, struct lyd_node **bridge_node);
int bridging_ly_tree_add_bridge_address(const struct ly_ctx *ly_ctx, struct lyd_node *bridge_node, struct rtnl_link *bridge_link);
int bridging_ly_tree_add_bridge_type(const struct ly_ctx *ly_ctx, struct lyd_node *bridge_node, struct rtnl_link *bridge_link);

// operational
int bridging_ly_tree_add_bridge_ports(const struct ly_ctx *ly_ctx, struct lyd_node *bridge_node, struct rtnl_link *bridge_link, struct nl_cache *link_cache);
int bridging_ly_tree_add_bridge_components(const struct ly_ctx *ly_ctx, struct lyd_node *bridge_node, struct rtnl_link *bridge_link, struct nl_cache *link_cache);

// change - component
int bridging_ly_tree_add_bridge_component(const struct ly_ctx *ly_ctx, struct lyd_node *bridge_node, struct rtnl_link *component_link, struct lyd_node **component_node);
int bridging_ly_tree_add_bridge_component_id(const struct ly_ctx *ly_ctx, struct lyd_node *component_node, struct rtnl_link *component_link);
int bridging_ly_tree_add_bridge_component_type(const struct ly_ctx *ly_ctx, struct lyd_node *component_node, struct rtnl_link *component_link);
int bridging_ly_tree_add_bridge_component_address(const struct ly_ctx *ly_ctx, struct lyd_node *component_node, struct rtnl_link *component_link);

#endif // BRIDGING_PLUGIN_LY_TREE_H