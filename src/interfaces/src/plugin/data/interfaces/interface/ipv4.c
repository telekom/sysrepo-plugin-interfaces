#include "ipv4.h"

int interfaces_interface_hash_element_ipv4_set_enabled(interfaces_interface_ipv4_t* ipv4, uint8_t enabled)
{
    ipv4->enabled = enabled;

    return 0;
}

int interfaces_interface_hash_element_ipv4_set_forwarding(interfaces_interface_ipv4_t* ipv4, uint8_t forwarding)
{
    ipv4->forwarding = forwarding;

    return 0;
}

int interfaces_interface_hash_element_ipv4_set_mtu(interfaces_interface_ipv4_t* ipv4, uint16_t mtu)
{
    ipv4->mtu = mtu;

    return 0;
}
