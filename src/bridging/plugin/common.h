#ifndef BRIDGING_PLUGIN_COMMON_H
#define BRIDGING_PLUGIN_COMMON_H

#define PLUGIN_NAME "bridging-plugin"

#define BASE_YANG_MODEL "ieee802-dot1q-bridge"

// bridges container
#define BRIDGING_BRIDGES_CONTAINER_YANG_PATH "/" BASE_YANG_MODEL ":bridges"

// bridge list
#define BRIDGING_BRIDGE_LIST_YANG_PATH BRIDGING_BRIDGES_CONTAINER_YANG_PATH "/bridge"

// for bridge-port callbacks
#define INTERFACES_YANG_MODEL "ietf-interfaces"
#define INTERFACES_LIST_PATH "/ietf-interfaces:interfaces/interface"

#define BRIDGING_BRIDGE_COMPONENT_YANG_PATH BRIDGING_BRIDGE_LIST_YANG_PATH "/component"
#define BRIDGING_BRIDGE_COMPONENT_BRIDGE_VLAN_YANG_PATH BRIDGING_BRIDGE_COMPONENT_YANG_PATH "/bridge-vlan"
#define BRIDGING_BRIDGE_COMPONENT_FILTERING_DATABASE_YANG_PATH BRIDGING_BRIDGE_COMPONENT_YANG_PATH "/filtering-database"

// equivalent to '#define BR_MAX_PORTS (1<<BR_PORT_BITS)' whereas '#define BR_PORT_BITS 10' - see /net/bridge/br_private.h
#define BRIDGING_MAX_BR_PORTS (1<<10)

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

void mac_address_ly_to_nl(char *addr);
void mac_address_nl_to_ly(char *addr);

#endif // BRIDGING_PLUGIN_COMMON_H
