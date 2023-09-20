#include "plugin.hpp"
#include "core/context.hpp"

#include <sysrepo-cpp/Session.hpp>
#include <sysrepo-cpp/utils/utils.hpp>

#include <srpcpp.hpp>

#include "modules/routing.hpp"

#include "sysrepo.h"

namespace sr = sysrepo;

/**
 * @brief Plugin init callback.
 *
 * @param session Plugin session.
 * @param priv Private data.
 *
 * @return Error code (SR_ERR_OK on success).
 */
int sr_plugin_init_cb(sr_session_ctx_t* session, void** priv)
{
    sr::ErrorCode error = sysrepo::ErrorCode::Ok;
    auto sess = sysrepo::wrapUnmanagedSession(session);
    auto& registry(srpc::ModuleRegistry<ietf::rt::PluginContext>::getInstance());
    auto ctx = new ietf::rt::PluginContext(sess);

    *priv = static_cast<void*>(ctx);

    // create session subscriptions
    SRPLG_LOG_INF(ctx->getPluginName(), "Creating plugin subscriptions");

    registry.registerModule<RoutingModule>(*ctx);

    auto& modules = registry.getRegisteredModules();

    // for all registered modules - apply startup datastore values
    // startup datastore values are coppied into the running datastore when the first connection with sysrepo is made
    for (auto& mod : modules) {
        SRPLG_LOG_INF(ctx->getPluginName(), "Applying startup values for module %s", mod->getName());
        for (auto& applier : mod->getValueAppliers()) {
            try {
                applier->applyDatastoreValues(sess);
            } catch (const std::runtime_error& err) {
                SRPLG_LOG_INF(ctx->getPluginName(), "Failed to apply datastore values for the following paths:");
                for (const auto& path : applier->getPaths()) {
                    SRPLG_LOG_INF(ctx->getPluginName(), "\t%s", path.c_str());
                }
            }
        }
    }

    // get registered modules and create subscriptions
    for (auto& mod : modules) {
        SRPLG_LOG_INF(ctx->getPluginName(), "Registering operational callbacks for module %s", mod->getName());
        srpc::registerOperationalSubscriptions(sess, *ctx, mod);
        SRPLG_LOG_INF(ctx->getPluginName(), "Registering module change callbacks for module %s", mod->getName());
        srpc::registerModuleChangeSubscriptions(sess, *ctx, mod);
        SRPLG_LOG_INF(ctx->getPluginName(), "Registering RPC callbacks for module %s", mod->getName());
        srpc::registerRpcSubscriptions(sess, *ctx, mod);
        SRPLG_LOG_INF(ctx->getPluginName(), "Registered module %s", mod->getName());
    }

    SRPLG_LOG_INF("ietf-system-plugin", "Created plugin subscriptions");

    return static_cast<int>(error);
}

/**
 * @brief Plugin cleanup callback.
 *
 * @param session Plugin session.
 * @param priv Private data.
 *
 */
void sr_plugin_cleanup_cb(sr_session_ctx_t* session, void* priv)
{
    auto& registry(srpc::ModuleRegistry<ietf::rt::PluginContext>::getInstance());
    auto ctx = static_cast<ietf::rt::PluginContext*>(priv);
    const auto plugin_name = ctx->getPluginName();

    SRPLG_LOG_INF(plugin_name, "Plugin cleanup called");

    auto& modules = registry.getRegisteredModules();
    for (auto& mod : modules) {
        SRPLG_LOG_INF(ctx->getPluginName(), "Cleaning up module: %s", mod->getName());
    }

    // cleanup context manually
    delete ctx;

    SRPLG_LOG_INF(plugin_name, "Plugin cleanup finished");
}
