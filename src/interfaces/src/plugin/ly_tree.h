#ifndef INTERFACES_PLUGIN_LY_TREE_H
#define INTERFACES_PLUGIN_LY_TREE_H

#include <libyang/libyang.h>

int interfaces_ly_tree_create_interfaces(const struct ly_ctx* ly_ctx, struct lyd_node** interfaces_node);
int interfaces_ly_tree_create_interfaces_interface(const struct ly_ctx* ly_ctx, struct lyd_node* interfaces_node, struct lyd_node** interface_node, const char* name);
int interfaces_ly_tree_create_interfaces_interface_parent_interface(const struct ly_ctx* ly_ctx, struct lyd_node* interface_node, const char* parent_interface);
int interfaces_ly_tree_create_interfaces_interface_forwarding_mode(const struct ly_ctx* ly_ctx, struct lyd_node* interface_node, const char* forwarding_mode);
int interfaces_ly_tree_create_interfaces_interface_max_frame_size(const struct ly_ctx* ly_ctx, struct lyd_node* interface_node, const char* max_frame_size);
int interfaces_ly_tree_create_interfaces_interface_loopback(const struct ly_ctx* ly_ctx, struct lyd_node* interface_node, const char* loopback);
int interfaces_ly_tree_create_interfaces_interface_encapsulation(const struct ly_ctx* ly_ctx, struct lyd_node* interface_node, struct lyd_node** encapsulation_node);
int interfaces_ly_tree_create_interfaces_interface_encapsulation_dot1q_vlan(const struct ly_ctx* ly_ctx, struct lyd_node* encapsulation_node, struct lyd_node** dot1q_vlan_node);
int interfaces_ly_tree_create_interfaces_interface_encapsulation_dot1q_vlan_second_tag(const struct ly_ctx* ly_ctx, struct lyd_node* dot1q_vlan_node, struct lyd_node** second_tag_node);
int interfaces_ly_tree_create_interfaces_interface_encapsulation_dot1q_vlan_second_tag_vlan_id(const struct ly_ctx* ly_ctx, struct lyd_node* second_tag_node, const char* vlan_id);
int interfaces_ly_tree_create_interfaces_interface_encapsulation_dot1q_vlan_second_tag_tag_type(const struct ly_ctx* ly_ctx, struct lyd_node* second_tag_node, const char* tag_type);
int interfaces_ly_tree_create_interfaces_interface_encapsulation_dot1q_vlan_outer_tag(const struct ly_ctx* ly_ctx, struct lyd_node* dot1q_vlan_node, struct lyd_node** outer_tag_node);
int interfaces_ly_tree_create_interfaces_interface_encapsulation_dot1q_vlan_outer_tag_vlan_id(const struct ly_ctx* ly_ctx, struct lyd_node* outer_tag_node, const char* vlan_id);
int interfaces_ly_tree_create_interfaces_interface_encapsulation_dot1q_vlan_outer_tag_tag_type(const struct ly_ctx* ly_ctx, struct lyd_node* outer_tag_node, const char* tag_type);
int interfaces_ly_tree_create_interfaces_interface_dampening(const struct ly_ctx* ly_ctx, struct lyd_node* interface_node, struct lyd_node** dampening_node);
int interfaces_ly_tree_create_interfaces_interface_dampening_time_remaining(const struct ly_ctx* ly_ctx, struct lyd_node* dampening_node, const char* time_remaining);
int interfaces_ly_tree_create_interfaces_interface_dampening_suppressed(const struct ly_ctx* ly_ctx, struct lyd_node* dampening_node, const char* suppressed);
int interfaces_ly_tree_create_interfaces_interface_dampening_penalty(const struct ly_ctx* ly_ctx, struct lyd_node* dampening_node, const char* penalty);
int interfaces_ly_tree_create_interfaces_interface_dampening_max_suppress_time(const struct ly_ctx* ly_ctx, struct lyd_node* dampening_node, const char* max_suppress_time);
int interfaces_ly_tree_create_interfaces_interface_dampening_suppress(const struct ly_ctx* ly_ctx, struct lyd_node* dampening_node, const char* suppress);
int interfaces_ly_tree_create_interfaces_interface_dampening_reuse(const struct ly_ctx* ly_ctx, struct lyd_node* dampening_node, const char* reuse);
int interfaces_ly_tree_create_interfaces_interface_dampening_half_life(const struct ly_ctx* ly_ctx, struct lyd_node* dampening_node, const char* half_life);
int interfaces_ly_tree_create_interfaces_interface_carrier_delay(const struct ly_ctx* ly_ctx, struct lyd_node* interface_node, struct lyd_node** carrier_delay_node);
int interfaces_ly_tree_create_interfaces_interface_carrier_delay_timer_running(const struct ly_ctx* ly_ctx, struct lyd_node* carrier_delay_node, const char* timer_running);
int interfaces_ly_tree_create_interfaces_interface_carrier_delay_carrier_transitions(const struct ly_ctx* ly_ctx, struct lyd_node* carrier_delay_node, const char* carrier_transitions);
int interfaces_ly_tree_create_interfaces_interface_carrier_delay_up(const struct ly_ctx* ly_ctx, struct lyd_node* carrier_delay_node, const char* up);
int interfaces_ly_tree_create_interfaces_interface_carrier_delay_down(const struct ly_ctx* ly_ctx, struct lyd_node* carrier_delay_node, const char* down);
int interfaces_ly_tree_create_interfaces_interface_statistics(const struct ly_ctx* ly_ctx, struct lyd_node* interface_node, struct lyd_node** statistics_node);
int interfaces_ly_tree_create_interfaces_interface_statistics_in_discard_unknown_encaps(const struct ly_ctx* ly_ctx, struct lyd_node* statistics_node, const char* in_discard_unknown_encaps);
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