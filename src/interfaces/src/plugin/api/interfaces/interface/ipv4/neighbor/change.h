#ifndef INTERFACES_PLUGIN_API_INTERFACES_INTERFACE_IPV4_NEIGHBOR_CHANGE_H
#define INTERFACES_PLUGIN_API_INTERFACES_INTERFACE_IPV4_NEIGHBOR_CHANGE_H

#include <srpc.h>
#include <utarray.h>

int interfaces_interface_ipv4_neighbor_change_link_layer_address_init(void* priv);
int interfaces_interface_ipv4_neighbor_change_link_layer_address(void* priv, sr_session_ctx_t* session, const srpc_change_ctx_t* change_ctx);
void interfaces_interface_ipv4_neighbor_change_link_layer_address_free(void* priv);

int interfaces_interface_ipv4_neighbor_change_ip_init(void* priv);
int interfaces_interface_ipv4_neighbor_change_ip(void* priv, sr_session_ctx_t* session, const srpc_change_ctx_t* change_ctx);
void interfaces_interface_ipv4_neighbor_change_ip_free(void* priv);

#endif // INTERFACES_PLUGIN_API_INTERFACES_INTERFACE_IPV4_NEIGHBOR_CHANGE_H