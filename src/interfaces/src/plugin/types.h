#ifndef INTERFACES_PLUGIN_TYPES_H
#define INTERFACES_PLUGIN_TYPES_H

#include <stdbool.h>
#include <stdint.h>

// typedefs
typedef struct interfaces_interfaces_interface_carrier_delay interfaces_interfaces_interface_carrier_delay_t;
typedef struct interfaces_interfaces_interface_dampening interfaces_interfaces_interface_dampening_t;
typedef struct interfaces_interfaces_interface_encapsulation_dot1q_vlan_outer_tag interfaces_interfaces_interface_encapsulation_dot1q_vlan_outer_tag_t;
typedef struct interfaces_interfaces_interface_encapsulation_dot1q_vlan_second_tag interfaces_interfaces_interface_encapsulation_dot1q_vlan_second_tag_t;
typedef struct interfaces_interfaces_interface_encapsulation_dot1q_vlan interfaces_interfaces_interface_encapsulation_dot1q_vlan_t;
typedef struct interfaces_interfaces_interface_encapsulation interfaces_interfaces_interface_encapsulation_t;
typedef struct interfaces_interfaces_interface interfaces_interfaces_interface_t;
typedef struct interfaces_interfaces_interface_element interfaces_interfaces_interface_element_t;
typedef struct interfaces_interfaces interfaces_interfaces_t;

enum interfaces_interfaces_interface_link_up_down_trap_enable {
    interfaces_interfaces_interface_link_up_down_trap_enable_disabled,
    interfaces_interfaces_interface_link_up_down_trap_enable_enabled,
};

typedef enum interfaces_interfaces_interface_link_up_down_trap_enable interfaces_interfaces_interface_link_up_down_trap_enable_t;

struct interfaces_interfaces_interface_carrier_delay {
    uint32_t down;
    uint32_t up;
};

struct interfaces_interfaces_interface_dampening {
    uint32_t half_life;
    uint32_t reuse;
    uint32_t suppress;
    uint32_t max_suppress_time;
};

struct interfaces_interfaces_interface_encapsulation_dot1q_vlan_outer_tag {
    char* tag_type;
    uint16_t vlan_id;
};

struct interfaces_interfaces_interface_encapsulation_dot1q_vlan_second_tag {
    char* tag_type;
    uint16_t vlan_id;
};

struct interfaces_interfaces_interface_encapsulation_dot1q_vlan {
    interfaces_interfaces_interface_encapsulation_dot1q_vlan_outer_tag_t outer_tag;
    interfaces_interfaces_interface_encapsulation_dot1q_vlan_second_tag_t second_tag;
};

struct interfaces_interfaces_interface_encapsulation {
    interfaces_interfaces_interface_encapsulation_dot1q_vlan_t dot1q_vlan;
};

struct interfaces_interfaces_interface {
    char* name;
    char* description;
    char* type;
    uint8_t enabled;
    interfaces_interfaces_interface_link_up_down_trap_enable_t link_up_down_trap_enable;
    interfaces_interfaces_interface_carrier_delay_t carrier_delay;
    interfaces_interfaces_interface_dampening_t dampening;
    interfaces_interfaces_interface_encapsulation_t encapsulation;
    char* loopback;
    uint32_t max_frame_size;
    char* parent_interface;
};

struct interfaces_interfaces_interface_element {
    interfaces_interfaces_interface_element_t* next;
    interfaces_interfaces_interface_t interface;
};

struct interfaces_interfaces {
    interfaces_interfaces_interface_element_t* interface;
};

#endif // INTERFACES_PLUGIN_TYPES_H