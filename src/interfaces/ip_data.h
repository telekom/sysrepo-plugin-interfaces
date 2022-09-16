/*
 * telekom / sysrepo-plugin-interfaces
 *
 * This program is made available under the terms of the
 * BSD 3-Clause license which is available at
 * https://opensource.org/licenses/BSD-3-Clause
 *
 * SPDX-FileCopyrightText: 2021 Deutsche Telekom AG
 * SPDX-FileContributor: Sartura Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IP_DATA_H_ONCE
#define IP_DATA_H_ONCE

#include <stdint.h>
#include <stdbool.h>

enum ip_subnet_type_e {
	ip_subnet_type_unknown = 0,
	ip_subnet_type_prefix_length,
	ip_subnet_type_netmask,
};

enum ip_address_origin_e {
	ip_address_origin_other = 0,
	ip_address_origin_static,
	ip_address_origin_dhcp,
	ip_address_origin_link_layer,
	ip_address_origin_random,
};

enum ip_neighbor_origin_e {
	ip_neighbor_origin_other = 0,
	ip_neighbor_origin_static,
	ip_neighbor_origin_dynamic,
};

typedef enum ip_subnet_type_e ip_subnet_type_t;
typedef enum ip_address_origin_e ip_address_origin_t;
typedef enum ip_neighbor_origin_e ip_neighbor_origin_t;
typedef struct ip_address_s ip_address_t;
typedef struct ip_address_list_s ip_address_list_t;
typedef struct ip_neighbor_s ip_neighbor_t;
typedef struct ip_neighbor_list_s ip_neighbor_list_t;
typedef struct ip_data_s ip_data_t;

struct ip_address_s {
	char *ip;
	uint8_t subnet;
	ip_subnet_type_t subnet_type;
	bool delete;
	ip_address_origin_t origin;
};

struct ip_address_list_s {
	ip_address_t *addr;
	uint32_t count;
};

struct ip_neighbor_s {
	char *ip;
	char *phys_addr;
	bool delete;
	ip_neighbor_origin_t origin;
};

struct ip_neighbor_list_s {
	ip_neighbor_t *nbor;
	uint32_t count;
};

struct ip_data_s {
	uint8_t enabled;
	uint8_t forwarding;
	uint16_t mtu;
	ip_address_list_t addr_list;
	ip_neighbor_list_t nbor_list;
};

void ip_data_init(ip_data_t *ip);
void ip_data_set_enabled(ip_data_t *ip, char *enabled);
void ip_data_set_forwarding(ip_data_t *ip, char *forwarding);
void ip_data_set_mtu(ip_data_t *ip, char *mtu);
void ip_data_add_address(ip_data_t *ip, char *addr, char *subnet, ip_subnet_type_t st, ip_address_origin_t origin);
void ip_data_add_neighbor(ip_data_t *ip, char *addr, char *phys_addr);
void ip_data_free(ip_data_t *ip);

void ip_address_init(ip_address_t *addr);
void ip_address_set_ip(ip_address_t *addr, char *ip);
void ip_address_set_delete(ip_address_list_t *addr_ls, char *ip);
void ip_address_set_subnet(ip_address_t *addr, char *subnet, ip_subnet_type_t st);
void ip_address_free(ip_address_t *addr);

void ip_address_list_init(ip_address_list_t *addr_ls);
void ip_address_list_add(ip_address_list_t *addr_ls, char *ip, char *subnet, ip_subnet_type_t st, ip_address_origin_t origin);
void ip_address_list_free(ip_address_list_t *addr_ls);

void ip_neighbor_init(ip_neighbor_t *n);
void ip_neighbor_set_ip(ip_neighbor_t *n, char *ip);
void ip_neighbor_set_delete(ip_neighbor_list_t *nbor_ls, char *ip);
void ip_neighbor_set_phys_addr(ip_neighbor_t *n, char *phys_addr);
void ip_neighbor_free(ip_neighbor_t *n);

void ip_neighbor_list_init(ip_neighbor_list_t *nbor_ls);
void ip_neighbor_list_add(ip_neighbor_list_t *nbor_ls, char *ip, char *phys_addr);
void ip_neighbor_list_free(ip_neighbor_list_t *nbor_ls);

#endif // IP_DATA_H_ONCE
