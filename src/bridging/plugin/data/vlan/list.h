#ifndef BRIDGING_PLUGIN_DATA_VLAN_LIST_H
#define BRIDGING_PLUGIN_DATA_VLAN_LIST_H

#include "plugin/common.h"
#include "plugin/types.h"

#include <linux/if_bridge.h>

void bridge_vlan_list_init(bridge_vlan_list_element_t** head);
void bridge_vlan_list_free(bridge_vlan_list_element_t** head);

bridge_vlan_list_element_t* bridge_vlan_list_from_vids_list(char* vids_list, uint16_t port_flags);

char* vids_str_next_vlan(char* vids_str, struct bridge_vlan_info* vlan_info);

#endif // BRIDGING_PLUGIN_DATA_VLAN_LIST_H
