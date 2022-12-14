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
#ifndef INTERFACES_PLUGIN_DATA_INTERFACES_INTERFACE_IPV6_H
#define INTERFACES_PLUGIN_DATA_INTERFACES_INTERFACE_IPV6_H

#include "plugin/types.h"

int interfaces_interface_hash_element_ipv6_set_enabled(interfaces_interface_ipv6_t* ipv6, uint8_t enabled);
int interfaces_interface_hash_element_ipv6_set_forwarding(interfaces_interface_ipv6_t* ipv6, uint8_t forwarding);
int interfaces_interface_hash_element_ipv6_set_mtu(interfaces_interface_ipv6_t* ipv6, uint16_t mtu);

#endif // INTERFACES_PLUGIN_DATA_INTERFACES_INTERFACE_IPV6_H