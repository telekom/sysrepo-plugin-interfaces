#ifndef LINK_DATA_H_ONCE
#define LINK_DATA_H_ONCE

#include <stdint.h>
#include <stdbool.h>
#include "utils/ip_data.h"
#include "utils/ipv4_data.h"
#include "utils/ipv6_data.h"

#define LD_MAX_LINKS 100 // TODO: check this

typedef struct link_data_s link_data_t;
typedef struct link_data_list_s link_data_list_t;

struct link_data_s {
	char *name;
	char *description;
	char *type;
	char *enabled;
	bool delete;
	ipv4_data_t ipv4;
	ipv6_data_t ipv6;
	struct {
		struct {
			struct {
				char *outer_tag_type;
				uint16_t outer_vlan_id;
				char *second_tag_type;
				uint16_t second_vlan_id;
			} dot1q_vlan;
			// TODO: add more interface types that can have a configurable L2 encapsulation
		} encapsulation;

		char *parent_interface;
	// TOOD: cover more yang nodes from extensions
	} extensions;
};

struct link_data_list_s {
	link_data_t links[LD_MAX_LINKS];
	uint8_t count;
};

// link_data struct functions
void link_data_init(link_data_t *l);
void link_data_set_name(link_data_t *l, char *name);
void link_data_free(link_data_t *l);

// link_data_list functions - init
int link_data_list_init(link_data_list_t *ld);
int link_data_list_add(link_data_list_t *ld, char *name);

// basic options
int link_data_list_set_description(link_data_list_t *ld, char *name, char *description);
int link_data_list_set_type(link_data_list_t *ld, char *name, char *type);
int link_data_list_set_enabled(link_data_list_t *ld, char *name, char *enabled);
int link_data_list_set_delete(link_data_list_t *ld, char *name, bool delete);

// ipv4 options
int link_data_list_set_ipv4_forwarding(link_data_list_t *ld, char *name, char *forwarding);
int link_data_list_set_ipv4_enabled(link_data_list_t *ld, char *name, char *enabled);
int link_data_list_set_ipv4_mtu(link_data_list_t *ld, char *name, char *mtu);
int link_data_list_add_ipv4_address(link_data_list_t *ld, char *name, char *ip, char *subnet, ip_subnet_type_t st);
int link_data_list_set_delete_ipv4_address(link_data_list_t *ld, char *name, char *ip);
int link_data_list_add_ipv4_neighbor(link_data_list_t *ld, char *name, char *ip, char *phys_addr);
int link_data_list_set_delete_ipv4_neighbor(link_data_list_t *ld, char *name, char *ip);

// ipv6 options
int link_data_list_set_ipv6_forwarding(link_data_list_t *ld, char *name, char *forwarding);
int link_data_list_set_ipv6_enabled(link_data_list_t *ld, char *name, char *enabled);
int link_data_list_set_ipv6_mtu(link_data_list_t *ld, char *name, char *mtu);
int link_data_list_add_ipv6_address(link_data_list_t *ld, char *name, char *ip, char *subnet);
int link_data_list_set_delete_ipv6_address(link_data_list_t *ld, char *name, char *ip);
int link_data_list_add_ipv6_neighbor(link_data_list_t *ld, char *name, char *ip, char *phys_addr);
int link_data_list_set_delete_ipv6_neighbor(link_data_list_t *ld, char *name, char *ip);
int link_data_list_set_ipv6_cga(link_data_list_t *ld, char *name, char *cga);
int link_data_list_set_ipv6_cta(link_data_list_t *ld, char *name, char *cta);
int link_data_list_set_ipv6_tvl(link_data_list_t *ld, char *name, char *tvl);
int link_data_list_set_ipv6_tpl(link_data_list_t *ld, char *name, char *tpl);

// if-extensions options
int link_data_list_set_parent(link_data_list_t *ld, char *name, char *parent);

// vlan-encapsulation options
int link_data_list_set_outer_tag_type(link_data_list_t *ld, char *name, char *outer_tag_type);
int link_data_list_set_outer_vlan_id(link_data_list_t *ld, char *name, uint16_t outer_vlan_id);
int link_data_list_set_second_tag_type(link_data_list_t *ld, char *name, char *second_tag_type);
int link_data_list_set_second_vlan_id(link_data_list_t *ld, char *name, uint16_t second_vlan_id);

void link_data_list_free(link_data_list_t *ld);

#endif /* IF_STATE_H_ONCE */
