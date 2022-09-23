#ifndef INTERFACES_PLUGIN_TYPES_H
#define INTERFACES_PLUGIN_TYPES_H

#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <uthash.h>

#include <uthash.h>

// typedefs
typedef struct interfaces_interface_carrier_delay interfaces_interface_carrier_delay_t;
typedef struct interfaces_interface_dampening interfaces_interface_dampening_t;
typedef struct interfaces_interface_encapsulation_dot1q_vlan_outer_tag interfaces_interface_encapsulation_dot1q_vlan_outer_tag_t;
typedef struct interfaces_interface_encapsulation_dot1q_vlan_second_tag interfaces_interface_encapsulation_dot1q_vlan_second_tag_t;
typedef struct interfaces_interface_encapsulation_dot1q_vlan interfaces_interface_encapsulation_dot1q_vlan_t;
typedef struct interfaces_interface_encapsulation interfaces_interface_encapsulation_t;
typedef struct interfaces_interface_ipv4_address interfaces_interface_ipv4_address_t;
typedef struct interfaces_interface_ipv4_address_element interfaces_interface_ipv4_address_element_t;
typedef struct interfaces_interface_ipv4_neighbor interfaces_interface_ipv4_neighbor_t;
typedef struct interfaces_interface_ipv4_neighbor_element interfaces_interface_ipv4_neighbor_element_t;
typedef struct interfaces_interface_ipv4 interfaces_interface_ipv4_t;
typedef struct interfaces_interface_ipv6_address interfaces_interface_ipv6_address_t;
typedef struct interfaces_interface_ipv6_address_element interfaces_interface_ipv6_address_element_t;
typedef struct interfaces_interface_ipv6_neighbor interfaces_interface_ipv6_neighbor_t;
typedef struct interfaces_interface_ipv6_neighbor_element interfaces_interface_ipv6_neighbor_element_t;
typedef struct interfaces_interface_ipv6_autoconf interfaces_interface_ipv6_autoconf_t;
typedef struct interfaces_interface_ipv6 interfaces_interface_ipv6_t;
typedef struct interfaces_interface interfaces_interface_t;
typedef struct interfaces_interface_element interfaces_interface_element_t;
typedef struct interfaces interfaces_t;
typedef struct interface_ht_element interface_ht_element_t;
typedef struct interfaces_interface_state interfaces_interface_state_t;
typedef struct interfaces_interface_state_hash_element interfaces_interface_state_hash_element_t;
typedef struct interfaces_interface_hash_element interfaces_interface_hash_element_t;

enum interfaces_interface_link_up_down_trap_enable {
    interfaces_interface_link_up_down_trap_enable_disabled,
    interfaces_interface_link_up_down_trap_enable_enabled,
};

typedef enum interfaces_interface_link_up_down_trap_enable interfaces_interface_link_up_down_trap_enable_t;

struct interfaces_interface_carrier_delay {
    uint32_t down;
    uint32_t up;
};

struct interfaces_interface_dampening {
    uint32_t half_life;
    uint32_t reuse;
    uint32_t suppress;
    uint32_t max_suppress_time;
};

struct interfaces_interface_encapsulation_dot1q_vlan_outer_tag {
    char* tag_type;
    uint16_t vlan_id;
};

struct interfaces_interface_encapsulation_dot1q_vlan_second_tag {
    char* tag_type;
    uint16_t vlan_id;
};

struct interfaces_interface_encapsulation_dot1q_vlan {
    interfaces_interface_encapsulation_dot1q_vlan_outer_tag_t outer_tag;
    interfaces_interface_encapsulation_dot1q_vlan_second_tag_t second_tag;
};

struct interfaces_interface_encapsulation {
    interfaces_interface_encapsulation_dot1q_vlan_t dot1q_vlan;
};

struct interfaces_interface_ipv4_address {
    char* ip;
    union {
        uint8_t prefix_length;
        char* netmask;
    } subnet;
};

struct interfaces_interface_ipv4_address_element {
    interfaces_interface_ipv4_address_element_t* next;
    interfaces_interface_ipv4_address_t address;
};

struct interfaces_interface_ipv4_neighbor {
    char* ip;
    char* link_layer_address;
};

struct interfaces_interface_ipv4_neighbor_element {
    interfaces_interface_ipv4_neighbor_element_t* next;
    interfaces_interface_ipv4_neighbor_t neighbor;
};

struct interfaces_interface_ipv4 {
    uint8_t enabled;
    uint8_t forwarding;
    uint16_t mtu;
    interfaces_interface_ipv4_address_element_t* address;
    interfaces_interface_ipv4_neighbor_element_t* neighbor;
};

struct interfaces_interface_ipv6_address {
    char* ip;
    uint8_t prefix_length;
};

struct interfaces_interface_ipv6_address_element {
    interfaces_interface_ipv6_address_element_t* next;
    interfaces_interface_ipv6_address_t address;
};

struct interfaces_interface_ipv6_neighbor {
    char* ip;
    char* link_layer_address;
};

struct interfaces_interface_ipv6_neighbor_element {
    interfaces_interface_ipv6_neighbor_element_t* next;
    interfaces_interface_ipv6_neighbor_t neighbor;
};

struct interfaces_interface_ipv6_autoconf {
    uint8_t create_global_addresses;
    uint8_t create_temporary_addresses;
    uint32_t temporary_valid_lifetime;
    uint32_t temporary_preferred_lifetime;
};

struct interfaces_interface_ipv6 {
    uint8_t enabled;
    uint8_t forwarding;
    uint32_t mtu;
    interfaces_interface_ipv6_address_element_t* address;
    interfaces_interface_ipv6_neighbor_element_t* neighbor;
    uint32_t dup_addr_detect_transmits;
    interfaces_interface_ipv6_autoconf_t autoconf;
};

struct interfaces_interface {
    char* name;
    char* description;
    char* type;
    uint8_t enabled;
    interfaces_interface_link_up_down_trap_enable_t link_up_down_trap_enable;
    interfaces_interface_carrier_delay_t carrier_delay;
    interfaces_interface_dampening_t dampening;
    interfaces_interface_encapsulation_t encapsulation;
    char* loopback;
    uint32_t max_frame_size;
    char* parent_interface;
    interfaces_interface_ipv4_t ipv4;
    interfaces_interface_ipv6_t ipv6;
};

struct interfaces_interface_element {
    interfaces_interface_element_t* next;
    interfaces_interface_t interface;
};

/* 
 *  - interface hash table element
 *  - used due to interface name indexing 
 */
struct interface_ht_element {
    interfaces_interface_t interface;
    /* makes the structure hashable */
    UT_hash_handle hh;
};

struct interfaces {
    interfaces_interface_element_t* interface;
};

struct interfaces_interface_state {
    char* name; // key
    uint8_t state;
    time_t last_change;
};

struct interfaces_interface_state_hash_element {
    interfaces_interface_state_t state;
    UT_hash_handle hh;
};

struct interfaces_interface_hash_element {
    interfaces_interface_t interface;
    UT_hash_handle hh;
};

#endif // INTERFACES_PLUGIN_TYPES_H
