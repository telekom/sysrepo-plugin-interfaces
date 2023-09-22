#ifndef BRIDGING_PLUGIN_API_BRIDGE_NETLINK_H
#define BRIDGING_PLUGIN_API_BRIDGE_NETLINK_H

#include "plugin/types.h"

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <linux/if_bridge.h>

/**
 * Send a RTM_GETLINK request for an existing bridge, read the
 * IFLA_BR_VLAN_PROTOCOL and IFLA_BR_VLAN_FILTERING attributes
 * and save them in the provided vlan_info structure.
 */
int bridge_get_vlan_info(struct nl_sock* socket, struct rtnl_link* bridge_link, bridge_vlan_info_t* vlan_info);

/*
 * Find all VLANs used on link (bridge master or bridge port).
 * Save the list of VLANs in list_dest. Save number of VLANs on link in count.
 */
int bridge_get_vlan_list(struct nl_sock* socket, struct rtnl_link* link, struct bridge_vlan_info** list_dest, size_t* count);

/**
 * Set the IFLA_BR_VLAN_FILTERING and IFLA_BR_VLAN_PROTOCOL attributes
 * for an existing bridge.
 */
int bridge_set_vlan_config(struct nl_sock* socket, int bridge_link_idx, bridge_vlan_info_t* vlan_info);

int bridge_set_ageing_time(struct nl_sock* socket, int bridge_link_idx, unsigned ageing_time);

/*
 * Add vlan to bridge port or master (stored in link).
 * Possible flags:
 * - BRIDGE_VLAN_INFO_PVID - this VLAN (vid) is primary (assumed if no VLAN tag is present)
 * - BRIDGE_VLAN_INFO_UNTAGGED - remove VLAN tag on egress for this vid on link
 */
// int bridge_add_vlan(struct nl_sock *socket, struct rtnl_link *link, uint16_t vid, uint16_t flags);

/* for adding multiple vlans to a port in a single netlink command */
int bridge_vlan_msg_open(struct nl_msg** msg, struct rtnl_link* link, bool delete);
int bridge_vlan_msg_add_vlan(struct nl_msg* msg, struct bridge_vlan_info vinfo);
int bridge_vlan_msg_finalize_and_send(struct nl_sock* socket, struct nl_msg* msg);

#endif // BRIDGING_PLUGIN_API_BRIDGE_NETLINK_H
