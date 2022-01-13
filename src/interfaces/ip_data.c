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

#include "ip_data.h"
#include "utils/memory.h"
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

void ip_data_init(ip_data_t *ip)
{
	ip->enabled = 0;
	ip->forwarding = 0;
	ip->mtu = 0;
	ip_address_list_init(&ip->addr_list);
	ip_neighbor_list_init(&ip->nbor_list);
}

void ip_data_set_enabled(ip_data_t *ip, char *enabled)
{
	ip->enabled = (strcmp(enabled, "true") == 0) ? 1 : 0;
}

void ip_data_set_forwarding(ip_data_t *ip, char *forwarding)
{
	ip->forwarding = (strcmp(forwarding, "true") == 0) ? 1 : 0;
}

void ip_data_set_mtu(ip_data_t *ip, char *mtu)
{
	ip->mtu = (uint16_t) atoi(mtu);
}

void ip_data_add_address(ip_data_t *ip, char *addr, char *subnet, ip_subnet_type_t st)
{
	ip_address_list_add(&ip->addr_list, addr, subnet, st);
}

void ip_data_add_neighbor(ip_data_t *ip, char *addr, char *phys_addr)
{
	ip_neighbor_list_add(&ip->nbor_list, addr, phys_addr);
}

void ip_data_free(ip_data_t *ip)
{
	ip_address_list_free(&ip->addr_list);
	ip_neighbor_list_free(&ip->nbor_list);
	ip_data_init(ip);
}

void ip_address_init(ip_address_t *addr)
{
	addr->ip = NULL;
	addr->subnet_type = ip_subnet_type_unknown;
	addr->delete = false;
}

void ip_address_set_ip(ip_address_t *addr, char *ip)
{
	addr->ip = xstrdup(ip);
}

void ip_address_set_delete(ip_address_list_t *addr_ls, char *ip)
{
	if (addr_ls->count > 0) {
		for (uint32_t i = 0; i < addr_ls->count; i++) {
			if (addr_ls->addr[i].ip != NULL) {
				if (strcmp(addr_ls->addr[i].ip, ip) == 0) {
					addr_ls->addr[i].delete = true;
				}
			}
		}
	}
}

/*
 * Function:  netmask_to_prefix_len
 * --------------------------------
 * calculates the prefix length (32 or 128 bit Hamming weight) of a network mask
 *
 *  nm: network mask
 *
 *  returns:
 *      prefix length
 */
uint8_t netmask_to_prefix_len(char *nm)
{
        int ret;
        struct sockaddr_in sa;
        struct sockaddr_in6 sa6;

        // IPv6 if a ':' is found
        if (strchr(nm, ':')) {
                ret = inet_pton(AF_INET6, nm, &(sa6.sin6_addr));
                // invalid network address mask
                if (!ret) {
                        // TODO: error handling on refactor
                }

                // s6_addr is a uint8_t array of length 16, all the byte popcounts need to be summarized
                // avoid branching, use popcountll's 64 bits minimum
                uint64_t *s6_addr64 = (uint64_t *) sa6.sin6_addr.s6_addr;

                return __builtin_popcountll(s6_addr64[0]) + __builtin_popcountll(s6_addr64[1]);

        }

        // IPv4 otherwise
        ret = inet_pton(AF_INET, nm, &(sa.sin_addr));
        // invalid network address mask
        if (!ret) {
                // TODO: error handling on refactor
        }

        return __builtin_popcountl(sa.sin_addr.s_addr);
}

void ip_address_set_subnet(ip_address_t *addr, char *subnet, ip_subnet_type_t st)
{
	addr->subnet_type = st;
	if (st == ip_subnet_type_netmask) {
		addr->subnet = netmask_to_prefix_len(subnet);
	} else {
		addr->subnet = (uint8_t) atoi(subnet);
	}
}

void ip_address_free(ip_address_t *addr)
{
	if (addr->ip != NULL) {
		FREE_SAFE(addr->ip);
	}
	ip_address_init(addr);
}

void ip_address_list_init(ip_address_list_t *addr_ls)
{
	addr_ls->addr = NULL;
	addr_ls->count = 0;
}

void ip_address_list_add(ip_address_list_t *addr_ls, char *ip, char *subnet, ip_subnet_type_t st)
{
	ip_address_t *addr = NULL;

	if (addr_ls->count > 0) {
		for (uint32_t i = 0; i < addr_ls->count; i++) {
			// if the address is already in the list, don't add it
			if (strcmp(addr_ls->addr[i].ip, ip) == 0) {
				return;
			}
			// in case an address was deleted, we can reuse that portion of memory
			// find it and set the new address at that location
			if (addr_ls->addr[i].ip == NULL) {
				addr = &addr_ls->addr[i];
				break;
			}
		}
	}

	// in case the list doesn't contain empty space (none of the addresses were deleted)
	if (addr == NULL) {
		++addr_ls->count;
		addr_ls->addr = (ip_address_t *) xrealloc(addr_ls->addr, sizeof(ip_address_t) * addr_ls->count);
		addr = &addr_ls->addr[addr_ls->count - 1];
	}

	ip_address_init(addr);
	ip_address_set_ip(addr, ip);
	ip_address_set_subnet(addr, subnet, st);
}

void ip_address_list_free(ip_address_list_t *addr_ls)
{
	if (addr_ls->count > 0) {
		for (uint32_t i = 0; i < addr_ls->count; i++) {
			ip_address_free(&addr_ls->addr[i]);
		}
		FREE_SAFE(addr_ls->addr);
	}
	ip_address_list_init(addr_ls);
}

void ip_neighbor_init(ip_neighbor_t *n)
{
	n->ip = 0;
	n->phys_addr = 0;
	n->delete = false;
}

void ip_neighbor_set_ip(ip_neighbor_t *n, char *ip)
{
	n->ip = xstrdup(ip);
}

void ip_neighbor_set_delete(ip_neighbor_list_t *nbor_ls, char *ip)
{
	if (nbor_ls->count > 0) {
		for (uint32_t i = 0; i < nbor_ls->count; i++) {
			if (nbor_ls->nbor[i].ip != NULL) {
				if (strcmp(nbor_ls->nbor[i].ip, ip) == 0) {
					nbor_ls->nbor[i].delete = true;
				}
			}
		}
	}
}

void ip_neighbor_set_phys_addr(ip_neighbor_t *n, char *phys_addr)
{
	n->phys_addr = xstrdup(phys_addr);
}

void ip_neighbor_free(ip_neighbor_t *n)
{
	if (n->ip != NULL) {
		FREE_SAFE(n->ip);
	}
	if (n->phys_addr != NULL) {
		FREE_SAFE(n->phys_addr);
	}
	ip_neighbor_init(n);
}

void ip_neighbor_list_init(ip_neighbor_list_t *nbor_ls)
{
	nbor_ls->count = 0;
	nbor_ls->nbor = NULL;
}

void ip_neighbor_list_add(ip_neighbor_list_t *nbor_ls, char *ip, char *phys_addr)
{

	ip_neighbor_t *n = NULL;

	if (nbor_ls->count > 0) {
		for (uint32_t i = 0; i < nbor_ls->count; i++) {
			// in case a neighbor was deleted, we can reuse that portion of memory
			// find it and set the new neighbor at that location
			if (nbor_ls->nbor[i].ip == NULL) {
				n = &nbor_ls->nbor[i];
				break;
			}
		}
	}

	// in case the list doesn't contain empty space (none of the neighbors were deleted)
	if (n == NULL) {
		++nbor_ls->count;
		nbor_ls->nbor = (ip_neighbor_t *) xrealloc(nbor_ls->nbor, sizeof(ip_neighbor_t) * nbor_ls->count);
		n = &nbor_ls->nbor[nbor_ls->count - 1];
	}

	ip_neighbor_init(n);
	ip_neighbor_set_ip(n, ip);

	// don't set if phys_addr is "none"
	if (strcmp(phys_addr, "none") != 0) {
		ip_neighbor_set_phys_addr(n, phys_addr);
	}
}

void ip_neighbor_list_free(ip_neighbor_list_t *nbor_ls)
{
	if (nbor_ls->count > 0) {
		for (uint32_t i = 0; i < nbor_ls->count; i++) {
			ip_neighbor_free(&nbor_ls->nbor[i]);
		}
		FREE_SAFE(nbor_ls->nbor);
	}
	ip_neighbor_list_init(nbor_ls);
}
