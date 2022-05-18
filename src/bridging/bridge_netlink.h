#ifndef BRIDGING_PLUGIN_BRIDGE_NETLINK_H
#define BRIDGING_PLUGIN_BRIDGE_NETLINK_H

#include <stdint.h>

typedef struct bridge_vlan_info_s bridge_vlan_info_t;

struct bridge_vlan_info_s {
        uint16_t vlan_proto;
        uint8_t vlan_filtering;
};

/**
 * Send a RTM_GETLINK request for an existing bridge, read the 
 * IFLA_BR_VLAN_PROTOCOL and IFLA_BR_VLAN_FILTERING attributes
 * and save them in the provided vlan_info structure.
 */
int bridge_get_vlan_info(struct nl_sock *socket, struct rtnl_link *bridge_link, bridge_vlan_info_t *vlan_info);

#endif // BRIDGING_PLUGIN_BRIDGE_NETLINK_H