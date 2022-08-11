#ifndef INTERFACES_PLUGIN_LY_TREE_H
#define INTERFACES_PLUGIN_LY_TREE_H

#include <libyang/libyang.h>

int interfaces_ly_tree_create_interfaces(const struct ly_ctx* ly_ctx, struct lyd_node** interfaces_node);
int interfaces_ly_tree_create_interfaces_interface(const struct ly_ctx* ly_ctx, struct lyd_node* interfaces_node, struct lyd_node** interface_node, const char* name);
int interfaces_ly_tree_create_interfaces_interface_statistics(const struct ly_ctx* ly_ctx, struct lyd_node* interface_node, struct lyd_node** statistics_node);
int interfaces_ly_tree_create_interfaces_interface_statistics_out_errors(const struct ly_ctx* ly_ctx, struct lyd_node* statistics_node, const char* out_errors);
int interfaces_ly_tree_create_interfaces_interface_statistics_out_discards(const struct ly_ctx* ly_ctx, struct lyd_node* statistics_node, const char* out_discards);
int interfaces_ly_tree_create_interfaces_interface_statistics_out_multicast_pkts(const struct ly_ctx* ly_ctx, struct lyd_node* statistics_node, const char* out_multicast_pkts);
int interfaces_ly_tree_create_interfaces_interface_statistics_out_broadcast_pkts(const struct ly_ctx* ly_ctx, struct lyd_node* statistics_node, const char* out_broadcast_pkts);
int interfaces_ly_tree_create_interfaces_interface_statistics_out_unicast_pkts(const struct ly_ctx* ly_ctx, struct lyd_node* statistics_node, const char* out_unicast_pkts);
int interfaces_ly_tree_create_interfaces_interface_statistics_out_octets(const struct ly_ctx* ly_ctx, struct lyd_node* statistics_node, const char* out_octets);
int interfaces_ly_tree_create_interfaces_interface_statistics_in_unknown_protos(const struct ly_ctx* ly_ctx, struct lyd_node* statistics_node, const char* in_unknown_protos);
int interfaces_ly_tree_create_interfaces_interface_statistics_in_errors(const struct ly_ctx* ly_ctx, struct lyd_node* statistics_node, const char* in_errors);
int interfaces_ly_tree_create_interfaces_interface_statistics_in_discards(const struct ly_ctx* ly_ctx, struct lyd_node* statistics_node, const char* in_discards);
int interfaces_ly_tree_create_interfaces_interface_statistics_in_multicast_pkts(const struct ly_ctx* ly_ctx, struct lyd_node* statistics_node, const char* in_multicast_pkts);
int interfaces_ly_tree_create_interfaces_interface_statistics_in_broadcast_pkts(const struct ly_ctx* ly_ctx, struct lyd_node* statistics_node, const char* in_broadcast_pkts);
int interfaces_ly_tree_create_interfaces_interface_statistics_in_unicast_pkts(const struct ly_ctx* ly_ctx, struct lyd_node* statistics_node, const char* in_unicast_pkts);
int interfaces_ly_tree_create_interfaces_interface_statistics_in_octets(const struct ly_ctx* ly_ctx, struct lyd_node* statistics_node, const char* in_octets);
int interfaces_ly_tree_create_interfaces_interface_statistics_discontinuity_time(const struct ly_ctx* ly_ctx, struct lyd_node* statistics_node, const char* discontinuity_time);
int interfaces_ly_tree_create_interfaces_interface_speed(const struct ly_ctx* ly_ctx, struct lyd_node* interface_node, const char* speed);
int interfaces_ly_tree_create_interfaces_interface_lower_layer_if(const struct ly_ctx* ly_ctx, struct lyd_node* interface_node, const char* lower_layer_if);
int interfaces_ly_tree_create_interfaces_interface_higher_layer_if(const struct ly_ctx* ly_ctx, struct lyd_node* interface_node, const char* higher_layer_if);
int interfaces_ly_tree_create_interfaces_interface_phys_address(const struct ly_ctx* ly_ctx, struct lyd_node* interface_node, const char* phys_address);
int interfaces_ly_tree_create_interfaces_interface_if_index(const struct ly_ctx* ly_ctx, struct lyd_node* interface_node, const char* if_index);
int interfaces_ly_tree_create_interfaces_interface_last_change(const struct ly_ctx* ly_ctx, struct lyd_node* interface_node, const char* last_change);
int interfaces_ly_tree_create_interfaces_interface_oper_status(const struct ly_ctx* ly_ctx, struct lyd_node* interface_node, const char* oper_status);
int interfaces_ly_tree_create_interfaces_interface_admin_status(const struct ly_ctx* ly_ctx, struct lyd_node* interface_node, const char* admin_status);
int interfaces_ly_tree_create_interfaces_interface_link_up_down_trap_enable(const struct ly_ctx* ly_ctx, struct lyd_node* interface_node, const char* link_up_down_trap_enable);
int interfaces_ly_tree_create_interfaces_interface_enabled(const struct ly_ctx* ly_ctx, struct lyd_node* interface_node, const char* enabled);
int interfaces_ly_tree_create_interfaces_interface_type(const struct ly_ctx* ly_ctx, struct lyd_node* interface_node, const char* type);
int interfaces_ly_tree_create_interfaces_interface_description(const struct ly_ctx* ly_ctx, struct lyd_node* interface_node, const char* description);
int interfaces_ly_tree_create_interfaces_interface_name(const struct ly_ctx* ly_ctx, struct lyd_node* interface_node, const char* name);

#endif // INTERFACES_PLUGIN_LY_TREE_H