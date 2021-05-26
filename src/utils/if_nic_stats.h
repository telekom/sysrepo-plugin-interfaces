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