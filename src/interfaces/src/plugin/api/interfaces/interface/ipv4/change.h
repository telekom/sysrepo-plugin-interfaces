#ifndef INTERFACES_PLUGIN_API_INTERFACES_INTERFACE_IPV4_CHANGE_H
#define INTERFACES_PLUGIN_API_INTERFACES_INTERFACE_IPV4_CHANGE_H

#include <srpc.h>
#include <utarray.h>

int interfaces_interface_ipv4_change_neighbor_init(void* priv);
int interfaces_interface_ipv4_change_neighbor(void* priv, sr_session_ctx_t* session, const srpc_change_ctx_t* change_ctx);
void interfaces_interface_ipv4_change_neighbor_free(void* priv);
int interfaces_interface_ipv4_change_address_init(void* priv);
int interfaces_interface_ipv4_change_address(void* priv, sr_session_ctx_t* session, const srpc_change_ctx_t* change_ctx);
void interfaces_interface_ipv4_change_address_free(void* priv);
int interfaces_interface_ipv4_change_mtu_init(void* priv);
int interfaces_interface_ipv4_change_mtu(void* priv, sr_session_ctx_t* session, const srpc_change_ctx_t* change_ctx);
void interfaces_interface_ipv4_change_mtu_free(void* priv);
int interfaces_interface_ipv4_change_forwarding_init(void* priv);
int interfaces_interface_ipv4_change_forwarding(void* priv, sr_session_ctx_t* session, const srpc_change_ctx_t* change_ctx);
void interfaces_interface_ipv4_change_forwarding_free(void* priv);
int interfaces_interface_ipv4_change_enabled_init(void* priv);
int interfaces_interface_ipv4_change_enabled(void* priv, sr_session_ctx_t* session, const srpc_change_ctx_t* change_ctx);
void interfaces_interface_ipv4_change_enabled_free(void* priv);

#endif // INTERFACES_PLUGIN_API_INTERFACES_INTERFACE_IPV4_CHANGE_H