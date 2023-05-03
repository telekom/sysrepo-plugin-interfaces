#include "plugin.hpp"
#include "core/context.hpp"

#include <sysrepo-cpp/Session.hpp>
#include <sysrepo-cpp/utils/utils.hpp>

namespace sr = sysrepo;

int sr_plugin_init_cb(sr_session_ctx_t* session, void** priv)
{
    sr::ErrorCode error = sysrepo::ErrorCode::Ok;
    auto sess = sysrepo::wrapUnmanagedSession(session);
    auto plugin_ctx = new ietf::ifc::PluginContext(sess);

    *priv = static_cast<void*>(plugin_ctx);

    return static_cast<int>(error);
}

void sr_plugin_cleanup_cb(sr_session_ctx_t* session, void* priv)
{
    auto plugin_ctx = static_cast<ietf::ifc::PluginContext*>(priv);
    delete plugin_ctx;
}