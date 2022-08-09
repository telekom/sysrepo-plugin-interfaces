#ifndef INTERFACES_PLUGIN_API_INTERFACES_CHANGE_H
#define INTERFACES_PLUGIN_API_INTERFACES_CHANGE_H

#include <srpc.h>
#include <utarray.h>

int interfaces_change_interface(void* priv, sr_session_ctx_t* session, const srpc_change_ctx_t* change_ctx);

#endif // INTERFACES_PLUGIN_API_INTERFACES_CHANGE_H