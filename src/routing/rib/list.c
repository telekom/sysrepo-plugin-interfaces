#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <net/if.h>

#include <sysrepo.h>

#include "utils/memory.h"
#include "rib.h"
#include "rib/list.h"

void rib_list_init(struct rib_list *ls)
{
	ls->list = NULL;
	ls->size = 0;
}

void rib_list_add(struct rib_list *ls, char *name, int af)
{
	struct rib *exists = rib_list_get(ls, name, af);
	if (exists == NULL) {
		struct rib *ptr = NULL;
		ls->list = realloc(ls->list, sizeof(struct rib) * (unsigned) (ls->size + 1));
		ptr = &ls->list[ls->size];

		rib_init(ptr);
		rib_set_name(ptr, name);
		rib_set_address_family(ptr, af);

		ls->size += 1;
	}
}

int rib_list_set_description(struct rib_list *ls, char *name, int af, const char *desc)
{
	struct rib *rib = rib_list_get(ls, name, af);
	if (rib == NULL) {
		return -1;
	}

	rib_set_description(rib, desc);
	return 0;
}

int rib_list_set_default(struct rib_list *ls, char *name, int af, int def)
{
	struct rib *rib = rib_list_get(ls, name, af);
	if (rib == NULL) {
		return -1;
	}

	rib_set_default(rib, def);
	return 0;
}

struct rib *rib_list_get(struct rib_list *ls, char *name, int af)
{
	for (int i = 0; i < ls->size; i++) {
		struct rib *ptr = &ls->list[i];
		if (strcmp(ptr->name, name) == 0 && ptr->address_family == af) {
			return &ls->list[i];
		}
	}
	return NULL;
}

void rib_list_free(struct rib_list *ls)
{
	if (ls->list) {
		for (int i = 0; i < ls->size; i++) {
			rib_free(&ls->list[i]);
		}
		FREE_SAFE(ls->list);
	}
}
