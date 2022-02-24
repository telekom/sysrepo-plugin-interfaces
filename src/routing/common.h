#ifndef ROUTING_PLUGIN_COMMON_H
#define ROUTING_PLUGIN_COMMON_H

#include "route/list_hash.h"
#define PLUGIN_NAME "routing plugin"

#define ROUTING_RIBS_COUNT 256
#define ROUTING_PROTOS_COUNT 256

#define BASE_YANG_MODEL "ietf-routing"

// base of all - routing container
#define ROUTING_CONTAINER_YANG_PATH "/" BASE_YANG_MODEL ":routing"

// router ID
#define ROUTING_ROUTER_ID_YANG_PATH ROUTING_CONTAINER_YANG_PATH "/router-id"

// interfaces container
#define ROUTING_INTERFACES_CONTAINER_YANG_PATH ROUTING_CONTAINER_YANG_PATH "/interfaces"
#define ROUTING_INTERFACE_LEAF_LIST_YANG_PATH ROUTING_INTERFACES_CONTAINER_YANG_PATH "/interface"

// control plane protocols container
#define ROUTING_CONTROL_PLANE_PROTOCOLS_CONTAINER_YANG_PATH ROUTING_CONTAINER_YANG_PATH "/control-plane-protocols"
#define ROUTING_CONTROL_PLANE_PROTOCOL_LIST_YANG_PATH ROUTING_CONTROL_PLANE_PROTOCOLS_CONTAINER_YANG_PATH "/control-plane-protocol"

// static routes
#define ROUTING_STATIC_ROUTES_CONTAINER_YANG_PATH ROUTING_CONTROL_PLANE_PROTOCOL_LIST_YANG_PATH "[type=\"ietf-routing:static\"][name=\"static\"]/static-routes"

// ribs
#define ROUTING_RIBS_CONTAINER_YANG_PATH ROUTING_CONTAINER_YANG_PATH "/ribs"
#define ROUTING_RIB_LIST_YANG_PATH ROUTING_RIBS_CONTAINER_YANG_PATH "/rib"
#define ROUTING_RIB_LIST_ACTIVE_ROUTE_RPC_PATH ROUTING_RIB_LIST_YANG_PATH "/active-route"

#include <netlink/route/nexthop.h>
#include <routing/rib/list.h>

void foreach_nexthop(struct rtnl_nexthop *nh, void *arg);

int routing_collect_ribs(struct nl_cache *routes_cache, struct rib_list_element **ribs_head);
int routing_collect_routes(struct nl_cache *routes_cache, struct nl_cache *link_cache, struct rib_list_element **ribs_head);
int routing_build_rib_descriptions(struct rib_list_element **ribs_head);
int routing_is_rib_known(int table);

int routing_apply_new_routes(struct nl_sock *socket, struct route_list_hash_element *routes_hash);
int routing_apply_modify_routes(struct nl_sock *socket, struct route_list_hash_element *routes_hash);
int routing_apply_delete_routes(struct nl_sock *socket, struct route_list_hash_element *routes_hash);

#endif // ROUTING_PLUGIN_COMMON_H