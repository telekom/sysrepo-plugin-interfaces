#include <linux/if_bridge.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <uthash/include/utlist.h>

#include "list.h"
#include "memory.h"

void bridge_vlan_list_init(bridge_vlan_list_element_t** head)
{
    *head = NULL;
}

bridge_vlan_list_element_t* bridge_vlan_list_find(bridge_vlan_list_element_t** head, struct bridge_vlan_info vlan_info)
{
    bridge_vlan_list_element_t* found = NULL;
    bridge_vlan_list_element_t* iter = NULL;

    LL_FOREACH(*head, iter)
    {
        if (iter->info.vid == vlan_info.vid && iter->info.flags == vlan_info.flags) {
            found = iter;
        }
    }
    return found;
}

void bridge_vlan_list_add(bridge_vlan_list_element_t** head, struct bridge_vlan_info vlan_info)
{
    bridge_vlan_list_element_t* new_vlan = NULL;
    bridge_vlan_list_element_t* found = NULL;

    found = bridge_vlan_list_find(head, vlan_info);
    if (!found) {
        new_vlan = xmalloc(sizeof(*new_vlan));
        new_vlan->next = NULL;
        new_vlan->info = vlan_info;
    }

    // use prepend - head is always the newest element
    LL_PREPEND(*head, new_vlan);
}

void bridge_vlan_list_free(bridge_vlan_list_element_t** head)
{
    bridge_vlan_list_element_t *iter = NULL, *tmp = NULL;

    LL_FOREACH_SAFE(*head, iter, tmp)
    {
        // bridge_vlan_free(&iter->vlan);
        FREE_SAFE(iter);
    }
}

/*
 * For a given comma separated list of vids and vid ranges, eg. `20,22,30-35,40`,
 * save the next vlan entry in vlan_info. The function returns the remaining part of
 * vids_str, until all entries are processed, then the function returns NULL.
 *
 * Assume vids_str format is correct (checked by libyang, type: dot1qtypes:vid-range-type).
 */
char* vids_str_next_vlan(char* vids_str, struct bridge_vlan_info* vlan_info)
{
    assert(vids_str != NULL);
    // clear old values
    vlan_info->vid = 0;
    vlan_info->flags = 0;

    if (*vids_str == '-') {
        // new vid is end of vlan range
        vlan_info->flags |= BRIDGE_VLAN_INFO_RANGE_END;
        vids_str++;
    }

    char* remaining = NULL;
    vlan_info->vid = (uint16_t)strtoul(vids_str, &remaining, 10);

    if (*remaining != '\0') {
        // remaining points to first invalid character
        if (*remaining == ',') {
            return remaining + 1;
        } else if (*remaining == '-') {
            // new vid is start of vlan range
            vlan_info->flags |= BRIDGE_VLAN_INFO_RANGE_BEGIN;
        }
        return remaining;
    }
    return NULL;
}

bridge_vlan_list_element_t* bridge_vlan_list_from_vids_list(char* vids_list, uint16_t port_flags)
{
    bridge_vlan_list_element_t* head = NULL;

    do {
        struct bridge_vlan_info vlan_info = { 0 };
        vids_list = vids_str_next_vlan(vids_list, &vlan_info);
        vlan_info.flags |= port_flags;
        bridge_vlan_list_add(&head, vlan_info);
    } while (vids_list);

    return head;
}
