#include "plugin.hpp"
#include "core/context.hpp"

// subscription API
#include "core/sub/change.hpp"
#include "core/sub/oper.hpp"
#include "core/sub/rpc.hpp"
#include "sysrepo-cpp/Enum.hpp"
#include "sysrepo-cpp/Subscription.hpp"
#include "sysrepo.h"

#include <sysrepo-cpp/Session.hpp>
#include <sysrepo-cpp/utils/utils.hpp>

namespace sr = sysrepo;

/**
 * @brief Operational callback struct.
 */
struct OperationalCallback {
    std::string xpath; ///< XPath of the data.
    sysrepo::OperGetCb callback; ///< Callback function.
};

/**
 * @brief Module change callback struct.
 */
struct ModuleChangeCallback {
    std::string xpath; ///< XPath of the data.
    sysrepo::ModuleChangeCb callback; ///< Callback function.
};

/**
 * @brief RPC callback struct.
 */
struct RpcCallback {
    std::string xpath; ///< XPath of the data.
    sysrepo::RpcActionCb callback; ///< Callback function.
};

/**
 * Register all operational plugin subscriptions.
 *
 * @param sess Session to use for creating subscriptions.
 * @param ctx Plugin context.
 *
 */
//void registerOperationalSubscriptions(sr::Session& sess, ietf::ifc::PluginContext& ctx);

/**
 * Register all module change plugin subscriptions.
 *
 * @param sess Session to use for creating subscriptions.
 * @param ctx Plugin context.
 *
 */
void registerModuleChangeSubscriptions(sr::Session& sess, ietf::ifc::PluginContext& ctx);

/**
 * Register all RPC plugin subscriptions.
 *
 * @param sess Session to use for creating subscriptions.
 * @param ctx Plugin context.
 *
 */
//void registerRpcSubscriptions(sr::Session& sess, ietf::ifc::PluginContext& ctx);

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
    auto plugin_ctx = new ietf::ifc::PluginContext(sess);

    *priv = static_cast<void*>(plugin_ctx);

    // create session subscriptions
    SRPLG_LOG_INF("ietf-interfaces-plugin", "Creating plugin subscriptions");

    //registerOperationalSubscriptions(sess, *plugin_ctx);
    registerModuleChangeSubscriptions(sess, *plugin_ctx);
    //registerRpcSubscriptions(sess, *plugin_ctx);

    SRPLG_LOG_INF("ietf-interfaces-plugin", "Created plugin subscriptions");

    return static_cast<int>(error);
}

void sr_plugin_cleanup_cb(sr_session_ctx_t* session, void* priv)
{

    SRPLG_LOG_INF("ietf-interfaces-plugin", "Plugin cleanup called");
    auto plugin_ctx = static_cast<ietf::ifc::PluginContext*>(priv);
    delete plugin_ctx;
    SRPLG_LOG_INF("ietf-interfaces-plugin", "Plugin cleanup finished");
}

void registerModuleChangeSubscriptions(sr::Session& sess, ietf::ifc::PluginContext& ctx)
{
    const auto change_callbacks = {
        ModuleChangeCallback { "/ietf-interfaces:interfaces/interface/enabled", ietf::ifc::sub::change::InterfaceModuleEnabledChangeCb(ctx.getModuleChangeContext()) },
        ModuleChangeCallback { "/ietf-interfaces:interfaces/interface/type", ietf::ifc::sub::change::InterfaceModuleTypeChangeCb(ctx.getModuleChangeContext()) },
        ModuleChangeCallback { "/ietf-interfaces:interfaces/interface/ipv4/enabled", ietf::ifc::sub::change::InterfaceModuleIPV4EnableChangeCb(ctx.getModuleChangeContext()) },
        ModuleChangeCallback { "/ietf-interfaces:interfaces/interface/ipv4/mtu", ietf::ifc::sub::change::InterfaceModuleIPV4MtuChangeCb(ctx.getModuleChangeContext()) }
    };

    auto& sub_handle = ctx.getSubscriptionHandle();

    for (auto& cb : change_callbacks) {
        if (sub_handle.has_value()) {
            sub_handle->onModuleChange("ietf-interfaces", cb.callback, cb.xpath);
        } else {
            sub_handle = sess.onModuleChange("ietf-interfaces", cb.callback, cb.xpath);
        }
    }
}