#ifndef INTERFACES_PLUGIN_API_INTERFACES_INTERFACE_CHANGE_H
#define INTERFACES_PLUGIN_API_INTERFACES_INTERFACE_CHANGE_H

#include <srpc.h>
#include <utarray.h>

int interfaces_interface_change_link_up_down_trap_enable(void* priv, sr_session_ctx_t* session, const srpc_change_ctx_t* change_ctx);
int interfaces_interface_change_enabled(void* priv, sr_session_ctx_t* session, const srpc_change_ctx_t* change_ctx);
int interfaces_interface_change_type(void* priv, sr_session_ctx_t* session, const srpc_change_ctx_t* change_ctx);
int interfaces_interface_change_description(void* priv, sr_session_ctx_t* session, const srpc_change_ctx_t* change_ctx);
int interfaces_interface_change_name(void* priv, sr_session_ctx_t* session, const srpc_change_ctx_t* change_ctx);

#endif // INTERFACES_PLUGIN_API_INTERFACES_INTERFACE_CHANGE_H