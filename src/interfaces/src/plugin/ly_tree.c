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

int interfaces_ly_tree_create_interfaces_interface_statistics(const struct ly_ctx* ly_ctx, struct lyd_node* interface_node, struct lyd_node** statistics_node)
{
    return srpc_ly_tree_create_container(ly_ctx, interface_node, statistics_node, "statistics");
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
