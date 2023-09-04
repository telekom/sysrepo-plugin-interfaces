/*
 * telekom / sysrepo-plugin-interfaces
 *
 * This program is made available under the terms of the
 * BSD 3-Clause license which is available at
 * https://opensource.org/licenses/BSD-3-Clause
 *
 * SPDX-FileCopyrightText: 2023 Deutsche Telekom AG
 * SPDX-FileContributor: Sartura Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef BRIDGING_PLUGIN_TYPES_H
#define BRIDGING_PLUGIN_TYPES_H

#include <stdint.h>
#include <linux/if_bridge.h>

// typedefs
typedef struct bridge_s bridge_t;
typedef struct bridge_component_s bridge_component_t;
typedef struct bridge_component_list_element_s bridge_component_list_element_t;
typedef struct bridge_list_element_s bridge_list_element_t;
typedef struct bridge_vlan_info_s bridge_vlan_info_t;
typedef struct bridge_vlan_list_element_s bridge_vlan_list_element_t;

struct bridge_s {
	char *name;
	char *type;
	uint16_t ports;
	uint32_t up_time;
	uint32_t components;
	bridge_component_list_element_t *component_list;
};

struct bridge_component_s {
	char *name;
	uint32_t id;
	char *type;
};

struct bridge_component_list_element_s {
	bridge_component_t component;
	bridge_component_list_element_t *next;
};

struct bridge_list_element_s {
	bridge_t bridge;
	bridge_list_element_t *next;
};

struct bridge_vlan_info_s {
	uint16_t vlan_proto;
	uint8_t vlan_filtering;
};

// bridge_vlan_list_element_t contains a list of vlans defined by a vid (VLAN ID)
// and flags, without refering to specific bridge ports
struct bridge_vlan_list_element_s {
	struct bridge_vlan_info info;
	bridge_vlan_list_element_t *next;
};

#endif // BRIDGING_PLUGIN_TYPES_H
