#include "routing.hpp"

/**
 * Routing module constructor. Allocates each context.
 */
RoutingModule::RoutingModule(ietf::rt::PluginContext& plugin_ctx)
    : srpc::IModule<ietf::rt::PluginContext>(plugin_ctx)
{
}

/**
 * Return the operational context from the module.
 */
std::shared_ptr<srpc::IModuleContext> RoutingModule::getOperationalContext() { return m_operContext; }

/**
 * Return the module changes context from the module.
 */
std::shared_ptr<srpc::IModuleContext> RoutingModule::getModuleChangesContext() { return m_changeContext; }

/**
 * Return the RPC context from the module.
 */
std::shared_ptr<srpc::IModuleContext> RoutingModule::getRpcContext() { return m_rpcContext; }

/**
 * Get all operational callbacks which the module should use.
 */
std::list<srpc::OperationalCallback> RoutingModule::getOperationalCallbacks() { return {}; }

/**
 * Get all module change callbacks which the module should use.
 */
std::list<srpc::ModuleChangeCallback> RoutingModule::getModuleChangeCallbacks() { return {}; }

/**
 * Get all RPC callbacks which the module should use.
 */
std::list<srpc::RpcCallback> RoutingModule::getRpcCallbacks() { return {}; }

/**
 * Get module name.
 */
constexpr const char* RoutingModule::getName() { return "Routing"; }
