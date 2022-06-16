#ifndef BRIDGING_PLUGIN_BRIDGE_VLAN_LIST_H
#define BRIDGING_PLUGIN_BRIDGE_VLAN_LIST_H

#include <linux/if_bridge.h>

typedef struct bridge_vlan_list_element_s bridge_vlan_list_element_t;

// bridge_vlan_list contains a list of vlans defined by a vid (VLAN ID)
// and flags, without refering to specific bridge ports
struct bridge_vlan_list_element_s {
	struct bridge_vlan_info info;
	bridge_vlan_list_element_t *next;
};

void bridge_vlan_list_init(bridge_vlan_list_element_t **head);
void bridge_vlan_list_free(bridge_vlan_list_element_t **head);

bridge_vlan_list_element_t *bridge_vlan_list_from_vids_list(char *vids_list, uint16_t port_flags);

#endif // BRIDGING_PLUGIN_BRIDGE_VLAN_LIST_H