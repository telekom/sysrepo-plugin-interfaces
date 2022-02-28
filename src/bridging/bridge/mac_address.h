#ifndef BRIDGING_PLUGIN_BRIDGE_MAC_ADDRESS_H
#define BRIDGING_PLUGIN_BRIDGE_MAC_ADDRESS_H

typedef struct mac_address_s mac_address_t;

struct mac_address_s {
	union {
		unsigned char octets[6];
	} value;
};

#endif // BRIDGING_PLUGIN_BRIDGE_MAC_ADDRESS_H