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

#include <ifaddrs.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <linux/ethtool.h>
#include <linux/sockios.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include "if_nic_stats.h"
#include "utils/memory.h"

int get_nic_stats(char *if_name, nic_stats_t *nic_stats)
{
	int skfd = -1;
	struct ethtool_drvinfo drvinfo = {0};
	struct ethtool_gstrings *gstrings = NULL;
	struct ethtool_stats    *stats   = NULL;
	struct ifreq ifr = {0};
	unsigned int n_stats = 0;
	unsigned int sz_str = 0;
	unsigned int sz_stats = 0;

	skfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (skfd < 0) {
		goto error_out;
	}

	// set interface name
	strncpy(&ifr.ifr_name[0], if_name, IFNAMSIZ);
	ifr.ifr_name[IFNAMSIZ - 1] = 0;

	// how many stats are available
	drvinfo.cmd = ETHTOOL_GDRVINFO;
	ifr.ifr_data = (caddr_t) &drvinfo;

	if (ioctl(skfd, SIOCETHTOOL, &ifr) != 0) {
		goto error_out;
	}

	n_stats = drvinfo.n_stats;
	if (n_stats < 1) {
		goto error_out;
	}

	// allocate memory for stat names and values
	sz_str = n_stats * ETH_GSTRING_LEN;
	sz_stats = n_stats * sizeof(uint64_t);

	gstrings = xcalloc(1, sz_str + sizeof(struct ethtool_gstrings));

	stats = xcalloc(1, sz_stats + sizeof(struct ethtool_stats));

	// get stat names
	gstrings->cmd = ETHTOOL_GSTRINGS;
	gstrings->string_set = ETH_SS_STATS;
	gstrings->len = n_stats;
	ifr.ifr_data = (caddr_t) gstrings;

	if (ioctl(skfd, SIOCETHTOOL, &ifr) != 0) {
		goto error_out;
	}

	// get stat values
	stats->cmd = ETHTOOL_GSTATS;
	stats->n_stats = n_stats;
	ifr.ifr_data = (caddr_t) stats;

	if (ioctl(skfd, SIOCETHTOOL, &ifr) != 0) {
		goto error_out;
	}

	for (unsigned int i = 0; i < n_stats; i++) {
		char *stat_name = (char *)&gstrings->data[i * ETH_GSTRING_LEN];
		uint64_t stat_val = stats->data[i];

		if (strncmp(stat_name, "rx_packets", ETH_GSTRING_LEN) == 0) {
			nic_stats->rx_packets = stat_val;
		} else if (strncmp(stat_name, "rx_broadcast", ETH_GSTRING_LEN) == 0) {
			nic_stats->rx_broadcast = stat_val;
		} else if (strncmp(stat_name, "tx_packets", ETH_GSTRING_LEN) == 0) {
			nic_stats->tx_packets = stat_val;
		} else if (strncmp(stat_name, "tx_broadcast", ETH_GSTRING_LEN) == 0) {
			nic_stats->tx_broadcast = stat_val;
		} else if (strncmp(stat_name, "tx_multicast", ETH_GSTRING_LEN) == 0) {
			nic_stats->tx_multicast = stat_val;
		}
	}

	FREE_SAFE(gstrings);
	FREE_SAFE(stats);
	close(skfd);

	return 0;

error_out:
	if (gstrings != NULL) {
		FREE_SAFE(gstrings);
	}

	if (stats != NULL) {
		FREE_SAFE(stats);
	}

	if (skfd >= 0) {
		close(skfd);
	}

	return -1;
}
