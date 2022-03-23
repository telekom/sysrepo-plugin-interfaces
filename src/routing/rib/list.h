#ifndef ROUTING_RIB_LIST_H
#define ROUTING_RIB_LIST_H

#include "rib.h"

struct rib_list {
	struct rib *list;
	size_t size;
};

void rib_list_init(struct rib_list *ls);
void rib_list_add(struct rib_list *ls, char *name, int af);
int rib_list_set_description(struct rib_list *ls, char *name, int af, const char *desc);
int rib_list_set_default(struct rib_list *ls, char *name, int af, int def);
struct rib *rib_list_get(struct rib_list *ls, char *name, int af);
void rib_list_free(struct rib_list *ls);

#endif // ROUTING_RIB_LIST_H
