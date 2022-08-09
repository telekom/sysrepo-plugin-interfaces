#ifndef INTERFACES_PLUGIN_TYPES_H
#define INTERFACES_PLUGIN_TYPES_H

#include <stdbool.h>
#include <stdint.h>

// typedefs
typedef enum interfaces_interfaces_interface_link_up_down_trap_enable interfaces_interfaces_interface_link_up_down_trap_enable_t;
typedef struct interfaces_interfaces_interface interfaces_interfaces_interface_t;
typedef struct interfaces_interfaces_interface_element interfaces_interfaces_interface_element_t;
typedef struct interfaces_interfaces interfaces_interfaces_t;

enum interfaces_interfaces_interface_link_up_down_trap_enable {
    interfaces_interfaces_interface_link_up_down_trap_enable_disabled,
    interfaces_interfaces_interface_link_up_down_trap_enable_enabled,
};

struct interfaces_interfaces_interface {
    char* name;
    char* description;
    char* type;
    uint8_t enabled;
    interfaces_interfaces_interface_link_up_down_trap_enable_t link_up_down_trap_enable;
};

struct interfaces_interfaces_interface_element {
    interfaces_interfaces_interface_element_t* next;
    interfaces_interfaces_interface_t interface;
};

struct interfaces_interfaces {
    interfaces_interfaces_interface_element_t* interface;
};

#endif // INTERFACES_PLUGIN_TYPES_H