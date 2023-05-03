#include "context.hpp"

namespace ietf::ifc {
/**
 * sysrepo-plugin-generator: Generated constructor for plugin context.
 *
 * @param sess Plugin session from the plugin init callback.
 *
 */
PluginContext::PluginContext(sysrepo::Session sess)
    : m_sess(sess)
{
    m_operCtx = std::make_shared<OperContext>();
    m_changeCtx = std::make_shared<ModuleChangeContext>();
    m_rpcCtx = std::make_shared<RpcContext>();
    m_notifCtx = std::make_shared<NotifContext>();
}

/**
 * sysrepo-plugin-generator: Generated getter for the plugin session.
 *
 * @return Plugin session from the init callback.
 *
 */
sysrepo::Session& PluginContext::getSession() { return m_sess; }

/**
 * sysrepo-plugin-generator: Generated getter for the subscription handle.
 *
 * @return Subscription handle.
 *
 */
std::optional<sysrepo::Subscription>& PluginContext::getSubscriptionHandle() { return m_subHandle; }

/**
 * sysrepo-plugin-generator: Generated getter for the operational data context.
 *
 * @return Operational data context.
 *
 */
std::shared_ptr<OperContext> PluginContext::getOperContext() const { return m_operCtx; }

/**
 * sysrepo-plugin-generator: Generated getter for the module change context.
 *
 * @return Module change context.
 *
 */
std::shared_ptr<ModuleChangeContext> PluginContext::getModuleChangeContext() const { return m_changeCtx; }

/**
 * sysrepo-plugin-generator: Generated getter for the RPC context.
 *
 * @return RPC context.
 *
 */
std::shared_ptr<RpcContext> PluginContext::getRpcContext() const { return m_rpcCtx; }

/**
 * sysrepo-plugin-generator: Generated getter for the notification context.
 *
 * @return Notification context.
 *
 */
std::shared_ptr<NotifContext> PluginContext::getNotifContext() const { return m_notifCtx; }

}