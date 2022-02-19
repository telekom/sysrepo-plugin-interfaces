#ifndef ROUTING_RIB_H
#define ROUTING_RIB_H

#include "route/list_hash.h"
#include "rib/description_pair.h"

struct rib {
	char name[32];
	char description[ROUTING_RIB_DESCRIPTION_SIZE];
	int address_family;
	int default_rib;
	struct route_list_hash_element *routes_head;
};

void rib_init(struct rib *rib);
void rib_set_address_family(struct rib *rib, int af);
void rib_set_description(struct rib *rib, const char *desc);
void rib_set_default(struct rib *rib, int def);
void rib_set_name(struct rib *rib, char *buff);
void rib_free(struct rib *rib);

#endif // ROUTING_RIB_H
