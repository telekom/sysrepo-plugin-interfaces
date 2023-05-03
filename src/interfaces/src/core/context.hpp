#pragma once

#include <sysrepo-cpp/Session.hpp>

namespace ietf::ifc {
/**
 * @brief Operational data context.
 */
class OperContext {
public:
    /**
     * sysrepo-plugin-generator: Generated default constructor for operational context.
     */
    OperContext() = default;
};

/**
 * @brief Module changes data context.
 */
class ModuleChangeContext {
public:
    /**
     * sysrepo-plugin-generator: Generated default constructor for module change context.
     */
    ModuleChangeContext() = default;
};

/**
 * @brief RPC data context.
 */
class RpcContext {
public:
    /**
     * sysrepo-plugin-generator: Generated default constructor for RPC context.
     */
    RpcContext() = default;
};

/**
 * @brief Notification data context.
 */
class NotifContext {
public:
    /**
     * sysrepo-plugin-generator: Generated default constructor for notification context.
     */
    NotifContext() = default;
};

/**
 * @brief Plugin context.
 */
class PluginContext {
public:
    /**
     * sysrepo-plugin-generator: Generated constructor for plugin context.
     *
     * @param sess Plugin session from the plugin init callback.
     *
     */
    PluginContext(sysrepo::Session sess);

    /**
     * sysrepo-plugin-generator: Generated getter for the plugin session.
     *
     * @return Plugin session from the init callback.
     *
     */
    sysrepo::Session& getSession();

    /**
     * sysrepo-plugin-generator: Generated getter for the subscription handle.
     *
     * @return Subscription handle.
     *
     */
    std::optional<sysrepo::Subscription>& getSubscriptionHandle();

    /**
     * sysrepo-plugin-generator: Generated getter for the operational data context.
     *
     * @return Operational data context.
     *
     */
    std::shared_ptr<OperContext> getOperContext() const;

    /**
     * sysrepo-plugin-generator: Generated getter for the module change context.
     *
     * @return Module change context.
     *
     */
    std::shared_ptr<ModuleChangeContext> getModuleChangeContext() const;

    /**
     * sysrepo-plugin-generator: Generated getter for the RPC context.
     *
     * @return RPC context.
     *
     */
    std::shared_ptr<RpcContext> getRpcContext() const;

    /**
     * sysrepo-plugin-generator: Generated getter for the notification context.
     *
     * @return Notification context.
     *
     */
    std::shared_ptr<NotifContext> getNotifContext() const;

    /**
     * sysrepo-plugin-generator: Generated default destructor for plugin context.
     */
    ~PluginContext() = default;

private:
    sysrepo::Session m_sess; ///< Plugin session from the plugin init callback.

    // Contexts
    std::shared_ptr<OperContext> m_operCtx; ///< Operational data context.
    std::shared_ptr<ModuleChangeContext> m_changeCtx; ///< Module change context.
    std::shared_ptr<RpcContext> m_rpcCtx; ///< RPC context.
    std::shared_ptr<NotifContext> m_notifCtx; ///< Notification context.

    // Subcription handle
    std::optional<sysrepo::Subscription> m_subHandle;
};
}