#include "ly_tree.h"
#include "common.h"

#include <srpc.h>

int interfaces_ly_tree_create_interfaces(const struct ly_ctx* ly_ctx, struct lyd_node** interfaces_node)
{
    return srpc_ly_tree_create_container(ly_ctx, NULL, interfaces_node, "interfaces");
}

int interfaces_ly_tree_create_interfaces_interface(const struct ly_ctx* ly_ctx, struct lyd_node* interfaces_node, struct lyd_node** interface_node, const char* name)
{
    // TODO: fix this for multiple keys with SRPC library
    return srpc_ly_tree_create_list(ly_ctx, interfaces_node, interface_node, "interface", "name", name);
}

int interfaces_ly_tree_create_interfaces_interface_ipv6(const struct ly_ctx* ly_ctx, struct lyd_node* interface_node, struct lyd_node** ipv6_node)
{
    return srpc_ly_tree_create_container(ly_ctx, interface_node, ipv6_node, "ipv6");
}

int interfaces_ly_tree_create_interfaces_interface_ipv6_autoconf(const struct ly_ctx* ly_ctx, struct lyd_node* ipv6_node, struct lyd_node** autoconf_node)
{
    return srpc_ly_tree_create_container(ly_ctx, ipv6_node, autoconf_node, "autoconf");
}

int interfaces_ly_tree_create_interfaces_interface_ipv6_autoconf_temporary_preferred_lifetime(const struct ly_ctx* ly_ctx, struct lyd_node* autoconf_node, const char* temporary_preferred_lifetime)
{
    return srpc_ly_tree_create_leaf(ly_ctx, autoconf_node, NULL, "temporary-preferred-lifetime", temporary_preferred_lifetime);
}

int interfaces_ly_tree_create_interfaces_interface_ipv6_autoconf_temporary_valid_lifetime(const struct ly_ctx* ly_ctx, struct lyd_node* autoconf_node, const char* temporary_valid_lifetime)
{
    return srpc_ly_tree_create_leaf(ly_ctx, autoconf_node, NULL, "temporary-valid-lifetime", temporary_valid_lifetime);
}

int interfaces_ly_tree_create_interfaces_interface_ipv6_autoconf_create_temporary_addresses(const struct ly_ctx* ly_ctx, struct lyd_node* autoconf_node, const char* create_temporary_addresses)
{
    return srpc_ly_tree_create_leaf(ly_ctx, autoconf_node, NULL, "create-temporary-addresses", create_temporary_addresses);
}

int interfaces_ly_tree_create_interfaces_interface_ipv6_autoconf_create_global_addresses(const struct ly_ctx* ly_ctx, struct lyd_node* autoconf_node, const char* create_global_addresses)
{
    return srpc_ly_tree_create_leaf(ly_ctx, autoconf_node, NULL, "create-global-addresses", create_global_addresses);
}

int interfaces_ly_tree_create_interfaces_interface_ipv6_dup_addr_detect_transmits(const struct ly_ctx* ly_ctx, struct lyd_node* ipv6_node, const char* dup_addr_detect_transmits)
{
    return srpc_ly_tree_create_leaf(ly_ctx, ipv6_node, NULL, "dup-addr-detect-transmits", dup_addr_detect_transmits);
}

int interfaces_ly_tree_create_interfaces_interface_ipv6_neighbor(const struct ly_ctx* ly_ctx, struct lyd_node* ipv6_node, struct lyd_node** neighbor_node, const char* ip)
{
    // TODO: fix this for multiple keys with SRPC library
    return srpc_ly_tree_create_list(ly_ctx, ipv6_node, neighbor_node, "neighbor", "ip", ip);
}

int interfaces_ly_tree_create_interfaces_interface_ipv6_neighbor_state(const struct ly_ctx* ly_ctx, struct lyd_node* neighbor_node, const char* state)
{
    return srpc_ly_tree_create_leaf(ly_ctx, neighbor_node, NULL, "state", state);
}

int interfaces_ly_tree_create_interfaces_interface_ipv6_neighbor_is_router(const struct ly_ctx* ly_ctx, struct lyd_node* neighbor_node, const char* is_router)
{
    return srpc_ly_tree_create_leaf(ly_ctx, neighbor_node, NULL, "is-router", is_router);
}

int interfaces_ly_tree_create_interfaces_interface_ipv6_neighbor_origin(const struct ly_ctx* ly_ctx, struct lyd_node* neighbor_node, const char* origin)
{
    return srpc_ly_tree_create_leaf(ly_ctx, neighbor_node, NULL, "origin", origin);
}

int interfaces_ly_tree_create_interfaces_interface_ipv6_neighbor_link_layer_address(const struct ly_ctx* ly_ctx, struct lyd_node* neighbor_node, const char* link_layer_address)
{
    return srpc_ly_tree_create_leaf(ly_ctx, neighbor_node, NULL, "link-layer-address", link_layer_address);
}

int interfaces_ly_tree_create_interfaces_interface_ipv6_neighbor_ip(const struct ly_ctx* ly_ctx, struct lyd_node* neighbor_node, const char* ip)
{
    return srpc_ly_tree_create_leaf(ly_ctx, neighbor_node, NULL, "ip", ip);
}

int interfaces_ly_tree_create_interfaces_interface_ipv6_address(const struct ly_ctx* ly_ctx, struct lyd_node* ipv6_node, struct lyd_node** address_node, const char* ip)
{
    // TODO: fix this for multiple keys with SRPC library
    return srpc_ly_tree_create_list(ly_ctx, ipv6_node, address_node, "address", "ip", ip);
}

int interfaces_ly_tree_create_interfaces_interface_ipv6_address_status(const struct ly_ctx* ly_ctx, struct lyd_node* address_node, const char* status)
{
    return srpc_ly_tree_create_leaf(ly_ctx, address_node, NULL, "status", status);
}

int interfaces_ly_tree_create_interfaces_interface_ipv6_address_origin(const struct ly_ctx* ly_ctx, struct lyd_node* address_node, const char* origin)
{
    return srpc_ly_tree_create_leaf(ly_ctx, address_node, NULL, "origin", origin);
}

int interfaces_ly_tree_create_interfaces_interface_ipv6_address_prefix_length(const struct ly_ctx* ly_ctx, struct lyd_node* address_node, const char* prefix_length)
{
    return srpc_ly_tree_create_leaf(ly_ctx, address_node, NULL, "prefix-length", prefix_length);
}

int interfaces_ly_tree_create_interfaces_interface_ipv6_address_ip(const struct ly_ctx* ly_ctx, struct lyd_node* address_node, const char* ip)
{
    return srpc_ly_tree_create_leaf(ly_ctx, address_node, NULL, "ip", ip);
}

int interfaces_ly_tree_create_interfaces_interface_ipv6_mtu(const struct ly_ctx* ly_ctx, struct lyd_node* ipv6_node, const char* mtu)
{
    return srpc_ly_tree_create_leaf(ly_ctx, ipv6_node, NULL, "mtu", mtu);
}

int interfaces_ly_tree_create_interfaces_interface_ipv6_forwarding(const struct ly_ctx* ly_ctx, struct lyd_node* ipv6_node, const char* forwarding)
{
    return srpc_ly_tree_create_leaf(ly_ctx, ipv6_node, NULL, "forwarding", forwarding);
}

int interfaces_ly_tree_create_interfaces_interface_ipv6_enabled(const struct ly_ctx* ly_ctx, struct lyd_node* ipv6_node, const char* enabled)
{
    return srpc_ly_tree_create_leaf(ly_ctx, ipv6_node, NULL, "enabled", enabled);
}

int interfaces_ly_tree_create_interfaces_interface_ipv4(const struct ly_ctx* ly_ctx, struct lyd_node* interface_node, struct lyd_node** ipv4_node)
{
    return srpc_ly_tree_create_container(ly_ctx, interface_node, ipv4_node, "ipv4");
}

int interfaces_ly_tree_create_interfaces_interface_ipv4_neighbor(const struct ly_ctx* ly_ctx, struct lyd_node* ipv4_node, struct lyd_node** neighbor_node, const char* ip)
{
    // TODO: fix this for multiple keys with SRPC library
    return srpc_ly_tree_create_list(ly_ctx, ipv4_node, neighbor_node, "neighbor", "ip", ip);
}

int interfaces_ly_tree_create_interfaces_interface_ipv4_neighbor_origin(const struct ly_ctx* ly_ctx, struct lyd_node* neighbor_node, const char* origin)
{
    return srpc_ly_tree_create_leaf(ly_ctx, neighbor_node, NULL, "origin", origin);
}

int interfaces_ly_tree_create_interfaces_interface_ipv4_neighbor_link_layer_address(const struct ly_ctx* ly_ctx, struct lyd_node* neighbor_node, const char* link_layer_address)
{
    return srpc_ly_tree_create_leaf(ly_ctx, neighbor_node, NULL, "link-layer-address", link_layer_address);
}

int interfaces_ly_tree_create_interfaces_interface_ipv4_neighbor_ip(const struct ly_ctx* ly_ctx, struct lyd_node* neighbor_node, const char* ip)
{
    return srpc_ly_tree_create_leaf(ly_ctx, neighbor_node, NULL, "ip", ip);
}

int interfaces_ly_tree_create_interfaces_interface_ipv4_address(const struct ly_ctx* ly_ctx, struct lyd_node* ipv4_node, struct lyd_node** address_node, const char* ip)
{
    // TODO: fix this for multiple keys with SRPC library
    return srpc_ly_tree_create_list(ly_ctx, ipv4_node, address_node, "address", "ip", ip);
}

int interfaces_ly_tree_create_interfaces_interface_ipv4_address_origin(const struct ly_ctx* ly_ctx, struct lyd_node* address_node, const char* origin)
{
    return srpc_ly_tree_create_leaf(ly_ctx, address_node, NULL, "origin", origin);
}

int interfaces_ly_tree_create_interfaces_interface_ipv4_address_netmask(const struct ly_ctx* ly_ctx, struct lyd_node* address_node, const char* netmask)
{
    return srpc_ly_tree_create_leaf(ly_ctx, address_node, NULL, "netmask", netmask);
}

int interfaces_ly_tree_create_interfaces_interface_ipv4_address_prefix_length(const struct ly_ctx* ly_ctx, struct lyd_node* address_node, const char* prefix_length)
{
    return srpc_ly_tree_create_leaf(ly_ctx, address_node, NULL, "prefix-length", prefix_length);
}

int interfaces_ly_tree_create_interfaces_interface_ipv4_address_ip(const struct ly_ctx* ly_ctx, struct lyd_node* address_node, const char* ip)
{
    return srpc_ly_tree_create_leaf(ly_ctx, address_node, NULL, "ip", ip);
}

int interfaces_ly_tree_create_interfaces_interface_ipv4_mtu(const struct ly_ctx* ly_ctx, struct lyd_node* ipv4_node, const char* mtu)
{
    return srpc_ly_tree_create_leaf(ly_ctx, ipv4_node, NULL, "mtu", mtu);
}

int interfaces_ly_tree_create_interfaces_interface_ipv4_forwarding(const struct ly_ctx* ly_ctx, struct lyd_node* ipv4_node, const char* forwarding)
{
    return srpc_ly_tree_create_leaf(ly_ctx, ipv4_node, NULL, "forwarding", forwarding);
}

int interfaces_ly_tree_create_interfaces_interface_ipv4_enabled(const struct ly_ctx* ly_ctx, struct lyd_node* ipv4_node, const char* enabled)
{
    return srpc_ly_tree_create_leaf(ly_ctx, ipv4_node, NULL, "enabled", enabled);
}

int interfaces_ly_tree_create_interfaces_interface_parent_interface(const struct ly_ctx* ly_ctx, struct lyd_node* interface_node, const char* parent_interface)
{
    return srpc_ly_tree_create_leaf(ly_ctx, interface_node, NULL, "parent-interface", parent_interface);
}

int interfaces_ly_tree_create_interfaces_interface_forwarding_mode(const struct ly_ctx* ly_ctx, struct lyd_node* interface_node, const char* forwarding_mode)
{
    return srpc_ly_tree_create_leaf(ly_ctx, interface_node, NULL, "forwarding-mode", forwarding_mode);
}

int interfaces_ly_tree_create_interfaces_interface_max_frame_size(const struct ly_ctx* ly_ctx, struct lyd_node* interface_node, const char* max_frame_size)
{
    return srpc_ly_tree_create_leaf(ly_ctx, interface_node, NULL, "max-frame-size", max_frame_size);
}

int interfaces_ly_tree_create_interfaces_interface_loopback(const struct ly_ctx* ly_ctx, struct lyd_node* interface_node, const char* loopback)
{
    return srpc_ly_tree_create_leaf(ly_ctx, interface_node, NULL, "loopback", loopback);
}

int interfaces_ly_tree_create_interfaces_interface_encapsulation(const struct ly_ctx* ly_ctx, struct lyd_node* interface_node, struct lyd_node** encapsulation_node)
{
    return srpc_ly_tree_create_container(ly_ctx, interface_node, encapsulation_node, "encapsulation");
}

int interfaces_ly_tree_create_interfaces_interface_encapsulation_dot1q_vlan(const struct ly_ctx* ly_ctx, struct lyd_node* encapsulation_node, struct lyd_node** dot1q_vlan_node)
{
    return srpc_ly_tree_create_container(ly_ctx, encapsulation_node, dot1q_vlan_node, "dot1q-vlan");
}

int interfaces_ly_tree_create_interfaces_interface_encapsulation_dot1q_vlan_second_tag(const struct ly_ctx* ly_ctx, struct lyd_node* dot1q_vlan_node, struct lyd_node** second_tag_node)
{
    return srpc_ly_tree_create_container(ly_ctx, dot1q_vlan_node, second_tag_node, "second-tag");
}

int interfaces_ly_tree_create_interfaces_interface_encapsulation_dot1q_vlan_second_tag_vlan_id(const struct ly_ctx* ly_ctx, struct lyd_node* second_tag_node, const char* vlan_id)
{
    return srpc_ly_tree_create_leaf(ly_ctx, second_tag_node, NULL, "vlan-id", vlan_id);
}

int interfaces_ly_tree_create_interfaces_interface_encapsulation_dot1q_vlan_second_tag_tag_type(const struct ly_ctx* ly_ctx, struct lyd_node* second_tag_node, const char* tag_type)
{
    return srpc_ly_tree_create_leaf(ly_ctx, second_tag_node, NULL, "tag-type", tag_type);
}

int interfaces_ly_tree_create_interfaces_interface_encapsulation_dot1q_vlan_outer_tag(const struct ly_ctx* ly_ctx, struct lyd_node* dot1q_vlan_node, struct lyd_node** outer_tag_node)
{
    return srpc_ly_tree_create_container(ly_ctx, dot1q_vlan_node, outer_tag_node, "outer-tag");
}

int interfaces_ly_tree_create_interfaces_interface_encapsulation_dot1q_vlan_outer_tag_vlan_id(const struct ly_ctx* ly_ctx, struct lyd_node* outer_tag_node, const char* vlan_id)
{
    return srpc_ly_tree_create_leaf(ly_ctx, outer_tag_node, NULL, "vlan-id", vlan_id);
}

int interfaces_ly_tree_create_interfaces_interface_encapsulation_dot1q_vlan_outer_tag_tag_type(const struct ly_ctx* ly_ctx, struct lyd_node* outer_tag_node, const char* tag_type)
{
    return srpc_ly_tree_create_leaf(ly_ctx, outer_tag_node, NULL, "tag-type", tag_type);
}

int interfaces_ly_tree_create_interfaces_interface_dampening(const struct ly_ctx* ly_ctx, struct lyd_node* interface_node, struct lyd_node** dampening_node)
{
    return srpc_ly_tree_create_container(ly_ctx, interface_node, dampening_node, "dampening");
}

int interfaces_ly_tree_create_interfaces_interface_dampening_time_remaining(const struct ly_ctx* ly_ctx, struct lyd_node* dampening_node, const char* time_remaining)
{
    return srpc_ly_tree_create_leaf(ly_ctx, dampening_node, NULL, "time-remaining", time_remaining);
}

int interfaces_ly_tree_create_interfaces_interface_dampening_suppressed(const struct ly_ctx* ly_ctx, struct lyd_node* dampening_node, const char* suppressed)
{
    return srpc_ly_tree_create_leaf(ly_ctx, dampening_node, NULL, "suppressed", suppressed);
}

int interfaces_ly_tree_create_interfaces_interface_dampening_penalty(const struct ly_ctx* ly_ctx, struct lyd_node* dampening_node, const char* penalty)
{
    return srpc_ly_tree_create_leaf(ly_ctx, dampening_node, NULL, "penalty", penalty);
}

int interfaces_ly_tree_create_interfaces_interface_dampening_max_suppress_time(const struct ly_ctx* ly_ctx, struct lyd_node* dampening_node, const char* max_suppress_time)
{
    return srpc_ly_tree_create_leaf(ly_ctx, dampening_node, NULL, "max-suppress-time", max_suppress_time);
}

int interfaces_ly_tree_create_interfaces_interface_dampening_suppress(const struct ly_ctx* ly_ctx, struct lyd_node* dampening_node, const char* suppress)
{
    return srpc_ly_tree_create_leaf(ly_ctx, dampening_node, NULL, "suppress", suppress);
}

int interfaces_ly_tree_create_interfaces_interface_dampening_reuse(const struct ly_ctx* ly_ctx, struct lyd_node* dampening_node, const char* reuse)
{
    return srpc_ly_tree_create_leaf(ly_ctx, dampening_node, NULL, "reuse", reuse);
}

int interfaces_ly_tree_create_interfaces_interface_dampening_half_life(const struct ly_ctx* ly_ctx, struct lyd_node* dampening_node, const char* half_life)
{
    return srpc_ly_tree_create_leaf(ly_ctx, dampening_node, NULL, "half-life", half_life);
}

int interfaces_ly_tree_create_interfaces_interface_carrier_delay(const struct ly_ctx* ly_ctx, struct lyd_node* interface_node, struct lyd_node** carrier_delay_node)
{
    return srpc_ly_tree_create_container(ly_ctx, interface_node, carrier_delay_node, "carrier-delay");
}

int interfaces_ly_tree_create_interfaces_interface_carrier_delay_timer_running(const struct ly_ctx* ly_ctx, struct lyd_node* carrier_delay_node, const char* timer_running)
{
    return srpc_ly_tree_create_leaf(ly_ctx, carrier_delay_node, NULL, "timer-running", timer_running);
}

int interfaces_ly_tree_create_interfaces_interface_carrier_delay_carrier_transitions(const struct ly_ctx* ly_ctx, struct lyd_node* carrier_delay_node, const char* carrier_transitions)
{
    return srpc_ly_tree_create_leaf(ly_ctx, carrier_delay_node, NULL, "carrier-transitions", carrier_transitions);
}

int interfaces_ly_tree_create_interfaces_interface_carrier_delay_up(const struct ly_ctx* ly_ctx, struct lyd_node* carrier_delay_node, const char* up)
{
    return srpc_ly_tree_create_leaf(ly_ctx, carrier_delay_node, NULL, "up", up);
}

int interfaces_ly_tree_create_interfaces_interface_carrier_delay_down(const struct ly_ctx* ly_ctx, struct lyd_node* carrier_delay_node, const char* down)
{
    return srpc_ly_tree_create_leaf(ly_ctx, carrier_delay_node, NULL, "down", down);
}

int interfaces_ly_tree_create_interfaces_interface_statistics(const struct ly_ctx* ly_ctx, struct lyd_node* interface_node, struct lyd_node** statistics_node)
{
    return srpc_ly_tree_create_container(ly_ctx, interface_node, statistics_node, "statistics");
}

int interfaces_ly_tree_create_interfaces_interface_statistics_in_discard_unknown_encaps(const struct ly_ctx* ly_ctx, struct lyd_node* statistics_node, const char* in_discard_unknown_encaps)
{
    return srpc_ly_tree_create_leaf(ly_ctx, statistics_node, NULL, "in-discard-unknown-encaps", in_discard_unknown_encaps);
}

int interfaces_ly_tree_create_interfaces_interface_statistics_out_errors(const struct ly_ctx* ly_ctx, struct lyd_node* statistics_node, const char* out_errors)
{
    return srpc_ly_tree_create_leaf(ly_ctx, statistics_node, NULL, "out-errors", out_errors);
}

int interfaces_ly_tree_create_interfaces_interface_statistics_out_discards(const struct ly_ctx* ly_ctx, struct lyd_node* statistics_node, const char* out_discards)
{
    return srpc_ly_tree_create_leaf(ly_ctx, statistics_node, NULL, "out-discards", out_discards);
}

int interfaces_ly_tree_create_interfaces_interface_statistics_out_multicast_pkts(const struct ly_ctx* ly_ctx, struct lyd_node* statistics_node, const char* out_multicast_pkts)
{
    return srpc_ly_tree_create_leaf(ly_ctx, statistics_node, NULL, "out-multicast-pkts", out_multicast_pkts);
}

int interfaces_ly_tree_create_interfaces_interface_statistics_out_broadcast_pkts(const struct ly_ctx* ly_ctx, struct lyd_node* statistics_node, const char* out_broadcast_pkts)
{
    return srpc_ly_tree_create_leaf(ly_ctx, statistics_node, NULL, "out-broadcast-pkts", out_broadcast_pkts);
}

int interfaces_ly_tree_create_interfaces_interface_statistics_out_unicast_pkts(const struct ly_ctx* ly_ctx, struct lyd_node* statistics_node, const char* out_unicast_pkts)
{
    return srpc_ly_tree_create_leaf(ly_ctx, statistics_node, NULL, "out-unicast-pkts", out_unicast_pkts);
}

int interfaces_ly_tree_create_interfaces_interface_statistics_out_octets(const struct ly_ctx* ly_ctx, struct lyd_node* statistics_node, const char* out_octets)
{
    return srpc_ly_tree_create_leaf(ly_ctx, statistics_node, NULL, "out-octets", out_octets);
}

int interfaces_ly_tree_create_interfaces_interface_statistics_in_unknown_protos(const struct ly_ctx* ly_ctx, struct lyd_node* statistics_node, const char* in_unknown_protos)
{
    return srpc_ly_tree_create_leaf(ly_ctx, statistics_node, NULL, "in-unknown-protos", in_unknown_protos);
}

int interfaces_ly_tree_create_interfaces_interface_statistics_in_errors(const struct ly_ctx* ly_ctx, struct lyd_node* statistics_node, const char* in_errors)
{
    return srpc_ly_tree_create_leaf(ly_ctx, statistics_node, NULL, "in-errors", in_errors);
}

int interfaces_ly_tree_create_interfaces_interface_statistics_in_discards(const struct ly_ctx* ly_ctx, struct lyd_node* statistics_node, const char* in_discards)
{
    return srpc_ly_tree_create_leaf(ly_ctx, statistics_node, NULL, "in-discards", in_discards);
}

int interfaces_ly_tree_create_interfaces_interface_statistics_in_multicast_pkts(const struct ly_ctx* ly_ctx, struct lyd_node* statistics_node, const char* in_multicast_pkts)
{
    return srpc_ly_tree_create_leaf(ly_ctx, statistics_node, NULL, "in-multicast-pkts", in_multicast_pkts);
}

int interfaces_ly_tree_create_interfaces_interface_statistics_in_broadcast_pkts(const struct ly_ctx* ly_ctx, struct lyd_node* statistics_node, const char* in_broadcast_pkts)
{
    return srpc_ly_tree_create_leaf(ly_ctx, statistics_node, NULL, "in-broadcast-pkts", in_broadcast_pkts);
}

int interfaces_ly_tree_create_interfaces_interface_statistics_in_unicast_pkts(const struct ly_ctx* ly_ctx, struct lyd_node* statistics_node, const char* in_unicast_pkts)
{
    return srpc_ly_tree_create_leaf(ly_ctx, statistics_node, NULL, "in-unicast-pkts", in_unicast_pkts);
}

int interfaces_ly_tree_create_interfaces_interface_statistics_in_octets(const struct ly_ctx* ly_ctx, struct lyd_node* statistics_node, const char* in_octets)
{
    return srpc_ly_tree_create_leaf(ly_ctx, statistics_node, NULL, "in-octets", in_octets);
}

int interfaces_ly_tree_create_interfaces_interface_statistics_discontinuity_time(const struct ly_ctx* ly_ctx, struct lyd_node* statistics_node, const char* discontinuity_time)
{
    return srpc_ly_tree_create_leaf(ly_ctx, statistics_node, NULL, "discontinuity-time", discontinuity_time);
}

int interfaces_ly_tree_create_interfaces_interface_speed(const struct ly_ctx* ly_ctx, struct lyd_node* interface_node, const char* speed)
{
    return srpc_ly_tree_create_leaf(ly_ctx, interface_node, NULL, "speed", speed);
}

int interfaces_ly_tree_create_interfaces_interface_lower_layer_if(const struct ly_ctx* ly_ctx, struct lyd_node* interface_node, const char* lower_layer_if)
{
    return srpc_ly_tree_append_leaf_list(ly_ctx, interface_node, NULL, "lower-layer-if", lower_layer_if);
}

int interfaces_ly_tree_create_interfaces_interface_higher_layer_if(const struct ly_ctx* ly_ctx, struct lyd_node* interface_node, const char* higher_layer_if)
{
    return srpc_ly_tree_append_leaf_list(ly_ctx, interface_node, NULL, "higher-layer-if", higher_layer_if);
}

int interfaces_ly_tree_create_interfaces_interface_phys_address(const struct ly_ctx* ly_ctx, struct lyd_node* interface_node, const char* phys_address)
{
    return srpc_ly_tree_create_leaf(ly_ctx, interface_node, NULL, "phys-address", phys_address);
}

int interfaces_ly_tree_create_interfaces_interface_if_index(const struct ly_ctx* ly_ctx, struct lyd_node* interface_node, const char* if_index)
{
    return srpc_ly_tree_create_leaf(ly_ctx, interface_node, NULL, "if-index", if_index);
}

int interfaces_ly_tree_create_interfaces_interface_last_change(const struct ly_ctx* ly_ctx, struct lyd_node* interface_node, const char* last_change)
{
    return srpc_ly_tree_create_leaf(ly_ctx, interface_node, NULL, "last-change", last_change);
}

int interfaces_ly_tree_create_interfaces_interface_oper_status(const struct ly_ctx* ly_ctx, struct lyd_node* interface_node, const char* oper_status)
{
    return srpc_ly_tree_create_leaf(ly_ctx, interface_node, NULL, "oper-status", oper_status);
}

int interfaces_ly_tree_create_interfaces_interface_admin_status(const struct ly_ctx* ly_ctx, struct lyd_node* interface_node, const char* admin_status)
{
    return srpc_ly_tree_create_leaf(ly_ctx, interface_node, NULL, "admin-status", admin_status);
}

int interfaces_ly_tree_create_interfaces_interface_link_up_down_trap_enable(const struct ly_ctx* ly_ctx, struct lyd_node* interface_node, const char* link_up_down_trap_enable)
{
    return srpc_ly_tree_create_leaf(ly_ctx, interface_node, NULL, "link-up-down-trap-enable", link_up_down_trap_enable);
}

int interfaces_ly_tree_create_interfaces_interface_enabled(const struct ly_ctx* ly_ctx, struct lyd_node* interface_node, const char* enabled)
{
    return srpc_ly_tree_create_leaf(ly_ctx, interface_node, NULL, "enabled", enabled);
}

int interfaces_ly_tree_create_interfaces_interface_type(const struct ly_ctx* ly_ctx, struct lyd_node* interface_node, const char* type)
{
    return srpc_ly_tree_create_leaf(ly_ctx, interface_node, NULL, "type", type);
}

int interfaces_ly_tree_create_interfaces_interface_description(const struct ly_ctx* ly_ctx, struct lyd_node* interface_node, const char* description)
{
    return srpc_ly_tree_create_leaf(ly_ctx, interface_node, NULL, "description", description);
}

int interfaces_ly_tree_create_interfaces_interface_name(const struct ly_ctx* ly_ctx, struct lyd_node* interface_node, const char* name)
{
    return srpc_ly_tree_create_leaf(ly_ctx, interface_node, NULL, "name", name);
}
