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

#ifndef IF_NIC_STATS_H_ONCE
#define IF_NIC_STATS_H_ONCE

#include <stdint.h>

typedef struct nic_stats_s nic_stats_t;

struct nic_stats_s {
	uint64_t rx_packets;
	uint64_t rx_broadcast;
	uint64_t tx_packets;
	uint64_t tx_broadcast;
	uint64_t tx_multicast;
};

int get_nic_stats(char *if_name, nic_stats_t *nic_stats);

#endif /* IF_NIC_STATS_H_ONCE */
