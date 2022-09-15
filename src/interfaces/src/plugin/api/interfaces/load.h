#ifndef INTERFACES_PLUGIN_API_INTERFACES_LOAD_H
#define INTERFACES_PLUGIN_API_INTERFACES_LOAD_H

#include "plugin/data/interfaces/interface/hash.h"
#include "plugin/context.h"
#include <utarray.h>

enum interfaces_load_exit_status {
    interfaces_load_failure = -1,
    interfaces_load_success =  0,
    interfaces_load_continue = 1,
};

static char *interfaces_get_interface_name(struct rtnl_link *link);

static char *interfaces_get_interface_description(interfaces_ctx_t *ctx, char *name);

static int read_from_sys_file(const char *dir_path, char *interface, int *val);

static char *interfaces_get_interface_type(struct rtnl_link *link, char *name);

static uint8_t interfaces_get_interface_enabled(struct rtnl_link *link);

static int interfaces_parse_link(interfaces_ctx_t *ctx, struct nl_sock *socket, struct nl_cache *cache, struct rtnl_link *link, interfaces_interfaces_interface_t *interface);

static char *interfaces_get_interface_parent_interface(struct nl_cache *cache, struct rtnl_link *link);

static int interfaces_add_link(interfaces_interface_hash_element_t **if_hash, interfaces_interfaces_interface_t *interface);

static struct rtnl_link *interfaces_get_next_link(struct rtnl_link *link);

static int interfaces_interfaces_worker(interfaces_ctx_t *ctx, struct nl_sock *socket, struct nl_cache *cache, interfaces_interface_hash_element_t **if_hash);

int interfaces_load_interface(interfaces_ctx_t* ctx, interfaces_interface_hash_element_t **if_hash;

#endif // INTERFACES_PLUGIN_API_INTERFACES_LOAD_H
