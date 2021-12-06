#ifndef ROUTING_RIB_DESCRIPTION_PAIR_HPP
#define ROUTING_RIB_DESCRIPTION_PAIR_HPP

#define ROUTING_RIB_DESCRIPTION_SIZE 256

struct rib_description_pair {
	// name = 32, ipv4/ipv6 + '-' = 5
	char name[32 + 5];

	// 256 chosen for now, can be changed later
	char description[ROUTING_RIB_DESCRIPTION_SIZE];
};

#endif // ROUTING_RIB_DESCRIPTION_PAIR_HPP
