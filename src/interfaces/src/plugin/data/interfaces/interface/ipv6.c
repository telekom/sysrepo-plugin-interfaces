/*
 * telekom / sysrepo-plugin-system
 *
 * This program is made available under the terms of the
 * BSD 3-Clause license which is available at
 * https://opensource.org/licenses/BSD-3-Clause
 *
 * SPDX-FileCopyrightText: 2022 Deutsche Telekom AG
 * SPDX-FileContributor: Sartura Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "ipv6.h"

int interfaces_interface_hash_element_ipv6_set_enabled(interfaces_interface_ipv6_t* ipv6, uint8_t enabled)
{
    ipv6->enabled = enabled;

    return 0;
}

int interfaces_interface_hash_element_ipv6_set_forwarding(interfaces_interface_ipv6_t* ipv6, uint8_t forwarding)
{
    ipv6->forwarding = forwarding;

    return 0;
}

int interfaces_interface_hash_element_ipv6_set_mtu(interfaces_interface_ipv6_t* ipv6, uint16_t mtu)
{
    ipv6->mtu = mtu;

    return 0;
}
