#ifndef BRIDGING_PLUGIN_COMMON_H
#define BRIDGING_PLUGIN_COMMON_H

#define PLUGIN_NAME "bridging-plugin"

#define BASE_YANG_MODEL "ieee802-dot1q-bridge"

// bridges container
#define BRIDGING_BRIDGES_CONTAINER_YANG_PATH "/" BASE_YANG_MODEL ":bridges"

// bridge list
#define BRIDGING_BRIDGE_LIST_YANG_PATH BRIDGING_BRIDGES_CONTAINER_YANG_PATH "/bridge"

#endif // BRIDGING_PLUGIN_COMMON_H