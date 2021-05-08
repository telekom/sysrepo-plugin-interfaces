#include <linux/if.h>
#include "utils/ip_data.h"
#include "utils/ipv4_data.h"
#include "utils/ipv6_data.h"
#include "utils/link_data.h"
#include "utils/memory.h"
#include <string.h>
#include <errno.h>

static link_data_t *data_list_get_by_name(link_data_list_t *ld, char *name);
extern int add_existing_links(link_data_list_t *ld);

void link_data_init(link_data_t *l)
{
	l->name = NULL;
	l->description = NULL;
	l->type = NULL;
	l->enabled = NULL;
	l->delete = false;
	ip_data_init(&l->ipv4);
	ipv6_data_init(&l->ipv6);
}

int link_data_list_init(link_data_list_t *ld)
{
	int error = 0;

	for (int i = 0; i < LD_MAX_LINKS; i++) {
		link_data_init(&ld->links[i]);
	}
	ld->count = 0;

	error = add_existing_links(ld);
	if (error != 0) {
		return -1;
	}

	return 0;
}

void link_data_set_name(link_data_t *l, char *name)
{
	l->name = xstrdup(name);
}

int link_data_list_add(link_data_list_t *ld, char *name)
{
	bool name_found = false;

	if (ld->count >= LD_MAX_LINKS) {
		return EINVAL;
	}

	for (int i = 0; i < ld->count; i++) {
		if (ld->links[i].name != NULL) { // in case we deleted a link it will be NULL
			if (strcmp(ld->links[i].name, name) == 0) {
				name_found = true;
				break;
			}
		}
	}

	if (!name_found) {
		// set the new link to the first free one in the list
		// the one with name == 0
		int pos = ld->count;
		for (int i = 0; i < ld->count; i++) {
			if (ld->links[i].name == NULL) {
				pos = i;
				break;
			}
		}
		link_data_set_name(&ld->links[pos], name);
		if (pos == ld->count) {
			++ld->count;
		}
	}

	return 0;
}

int link_data_list_set_ipv4_forwarding(link_data_list_t *ld, char *name, char *forwarding)
{
	int error = 0;
	link_data_t *l = NULL;

	l = data_list_get_by_name(ld, name);

	if (l != NULL) {
		ip_data_set_forwarding(&l->ipv4, forwarding);
	} else {
		error = EINVAL;
	}

	return error;
}

int link_data_list_set_ipv4_enabled(link_data_list_t *ld, char *name, char *enabled)
{
	int error = 0;
	link_data_t *l = NULL;

	l = data_list_get_by_name(ld, name);

	if (l != NULL) {
		ip_data_set_enabled(&l->ipv4, enabled);
	} else {
		error = EINVAL;
	}

	return error;
}

int link_data_list_set_ipv4_mtu(link_data_list_t *ld, char *name, char *mtu)
{
	int error = 0;
	link_data_t *l = NULL;

	l = data_list_get_by_name(ld, name);

	if (l != NULL) {
		ip_data_set_mtu(&l->ipv4, mtu);
	} else {
		error = EINVAL;
	}

	return error;
}

int link_data_list_add_ipv4_address(link_data_list_t *ld, char *name, char *ip, char *subnet, ip_subnet_type_t st)
{
	int error = 0;
	link_data_t *l = NULL;

	l = data_list_get_by_name(ld, name);

	if (l != NULL) {
		ip_data_add_address(&l->ipv4, ip, subnet, st);
	} else {
		error = EINVAL;
	}
	return error;
}

int link_data_list_set_delete_ipv4_address(link_data_list_t *ld, char *name, char *ip)
{
	int error = 0;
	link_data_t *l = NULL;

	l = data_list_get_by_name(ld, name);

	if (l != NULL) {
		ip_address_set_delete(&l->ipv4.addr_list, ip);
	} else {
		error = EINVAL;
	}
	return error;
}

int link_data_list_add_ipv4_neighbor(link_data_list_t *ld, char *name, char *ip, char *phys_addr)
{
	int error = 0;
	link_data_t *l = NULL;

	l = data_list_get_by_name(ld, name);

	if (l != NULL) {
		ip_data_add_neighbor(&l->ipv4, ip, phys_addr);
	} else {
		error = EINVAL;
	}
	return error;
}

int link_data_list_set_delete_ipv4_neighbor(link_data_list_t *ld, char *name, char *ip)
{
	int error = 0;
	link_data_t *l = NULL;

	l = data_list_get_by_name(ld, name);

	if (l != NULL) {
		ip_neighbor_set_delete(&l->ipv4.nbor_list, ip);
	} else {
		error = EINVAL;
	}
	return error;
}

int link_data_list_set_ipv6_forwarding(link_data_list_t *ld, char *name, char *forwarding)
{
	int error = 0;
	link_data_t *l = NULL;

	l = data_list_get_by_name(ld, name);

	if (l != NULL) {
		ip_data_set_forwarding(&l->ipv6.ip_data, forwarding);
	} else {
		error = EINVAL;
	}

	return error;
}

int link_data_list_set_ipv6_enabled(link_data_list_t *ld, char *name, char *enabled)
{
	int error = 0;
	link_data_t *l = NULL;

	l = data_list_get_by_name(ld, name);

	if (l != NULL) {
		ip_data_set_enabled(&l->ipv6.ip_data, enabled);
	} else {
		error = EINVAL;
	}

	return error;
}

int link_data_list_set_ipv6_mtu(link_data_list_t *ld, char *name, char *mtu)
{
	int error = 0;
	link_data_t *l = NULL;

	l = data_list_get_by_name(ld, name);

	if (l != NULL) {
		ip_data_set_mtu(&l->ipv6.ip_data, mtu);
	} else {
		error = EINVAL;
	}

	return error;
}

int link_data_list_add_ipv6_address(link_data_list_t *ld, char *name, char *ip, char *subnet)
{
	int error = 0;
	link_data_t *l = NULL;

	l = data_list_get_by_name(ld, name);

	if (l != NULL) {
		ip_data_add_address(&l->ipv6.ip_data, ip, subnet, ip_subnet_type_prefix_length);
	} else {
		error = EINVAL;
	}
	return error;
}

int link_data_list_set_delete_ipv6_address(link_data_list_t *ld, char *name, char *ip)
{
	int error = 0;
	link_data_t *l = NULL;

	l = data_list_get_by_name(ld, name);

	if (l != NULL) {
		ip_address_set_delete(&l->ipv6.ip_data.addr_list, ip);
	} else {
		error = EINVAL;
	}
	return error;
}

int link_data_list_add_ipv6_neighbor(link_data_list_t *ld, char *name, char *ip, char *phys_addr)
{
	int error = 0;
	link_data_t *l = NULL;

	l = data_list_get_by_name(ld, name);

	if (l != NULL) {
		ip_data_add_neighbor(&l->ipv6.ip_data, ip, phys_addr);
	} else {
		error = EINVAL;
	}
	return error;
}

int link_data_list_set_delete_ipv6_neighbor(link_data_list_t *ld, char *name, char *ip)
{
	int error = 0;
	link_data_t *l = NULL;

	l = data_list_get_by_name(ld, name);

	if (l != NULL) {
		ip_neighbor_set_delete(&l->ipv6.ip_data.nbor_list, ip);
	} else {
		error = EINVAL;
	}
	return error;
}

int link_data_list_set_ipv6_cga(link_data_list_t *ld, char *name, char *cga)
{
	int error = 0;
	link_data_t *l = NULL;

	l = data_list_get_by_name(ld, name);

	if (l != NULL) {
		ipv6_data_set_cga(&l->ipv6, cga);
	} else {
		error = EINVAL;
	}
	return error;
}

int link_data_list_set_ipv6_cta(link_data_list_t *ld, char *name, char *cta)
{
	int error = 0;
	link_data_t *l = NULL;

	l = data_list_get_by_name(ld, name);

	if (l != NULL) {
		ipv6_data_set_cta(&l->ipv6, cta);
	} else {
		error = EINVAL;
	}
	return error;
}

int link_data_list_set_ipv6_tvl(link_data_list_t *ld, char *name, char *tvl)
{
	int error = 0;
	link_data_t *l = NULL;

	l = data_list_get_by_name(ld, name);

	if (l != NULL) {
		ipv6_data_set_tvl(&l->ipv6, tvl);
	} else {
		error = EINVAL;
	}
	return error;
}

int link_data_list_set_ipv6_tpl(link_data_list_t *ld, char *name, char *tpl)
{
	int error = 0;
	link_data_t *l = NULL;

	l = data_list_get_by_name(ld, name);

	if (l != NULL) {
		ipv6_data_set_tpl(&l->ipv6, tpl);
	} else {
		error = EINVAL;
	}
	return error;
}

int link_data_list_set_description(link_data_list_t *ld, char *name, char *description)
{
	int error = 0;
	link_data_t *l = NULL;

	l = data_list_get_by_name(ld, name);

	if (l != NULL) {
		if (l->description) {
			FREE_SAFE(l->description);
		}
		l->description = xstrdup(description);
		if (l->description == NULL) {
			error = EINVAL;
		}
	} else {
		error = EINVAL;
	}

	return error;
}

int link_data_list_set_type(link_data_list_t *ld, char *name, char *type)
{
	int error = 0;
	link_data_t *l = NULL;

	l = data_list_get_by_name(ld, name);

	if (l != NULL) {
		// if previously set -> free allocated mem
		if (l->type) {
			FREE_SAFE(l->type);
		}
		l->type = xstrdup(type);
		if (l->type == NULL) {
			error = EINVAL;
		}
	} else {
		error = EINVAL;
	}

	return error;
}

int link_data_list_set_enabled(link_data_list_t *ld, char *name, char *enabled)
{
	int error = 0;
	link_data_t *l = NULL;

	l = data_list_get_by_name(ld, name);

	if (l != NULL) {
		if (l->enabled) {
			FREE_SAFE(l->enabled);
		}
		l->enabled = xstrdup(enabled);
		if (l->enabled == NULL) {
			error = EINVAL;
		}
	} else {
		error = EINVAL;
	}

	return error;
}

int link_data_list_set_delete(link_data_list_t *ld, char *name, bool delete)
{
	int error = 0;
	link_data_t *l = NULL;

	l = data_list_get_by_name(ld, name);

	if (l != NULL) {
		l->delete = delete;
	} else {
		error = EINVAL;
	}

	return error;
}

static link_data_t *data_list_get_by_name(link_data_list_t *ld, char *name)
{
	link_data_t *l = NULL;
	for (int i = 0; i < ld->count; i++) {
		if (ld->links[i].name != NULL) {
			if (strcmp(ld->links[i].name, name) == 0) {
				l = &ld->links[i];
				break;
			}
		}
	}
	return l;
}

void link_data_free(link_data_t *l)
{
	if (l->name) {
		FREE_SAFE(l->name);
	}

	if (l->description) {
		FREE_SAFE(l->description);
	}

	if (l->type) {
		FREE_SAFE(l->type);
	}

	if (l->enabled) {
		FREE_SAFE(l->enabled);
	}

	ip_data_free(&l->ipv4);
	ipv6_data_free(&l->ipv6);
}

void link_data_list_free(link_data_list_t *ld)
{
	for (int i = 0; i < ld->count; i++) {
		link_data_free(&ld->links[i]);
	}
}
