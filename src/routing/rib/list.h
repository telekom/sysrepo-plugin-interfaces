#ifndef ROUTING_RIB_LIST_H
#define ROUTING_RIB_LIST_H

#include "rib.h"

struct rib_list_element {
	struct rib rib;
	struct rib_list_element *next;
};

void rib_list_init(struct rib_list_element **head);
void rib_list_add(struct rib_list_element **head, char *name, int af);
int rib_list_set_description(struct rib_list_element **head, char *name, int af, const char *desc);
int rib_list_set_default(struct rib_list_element **head, char *name, int af, int def);
struct rib *rib_list_get(struct rib_list_element **head, char *name, int af);
void rib_list_free(struct rib_list_element **head);

#endif // ROUTING_RIB_LIST_H
