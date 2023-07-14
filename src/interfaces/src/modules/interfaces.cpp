#include "interfaces.hpp"
#include "modules/interfaces/oper.hpp"

#include <memory>

/**
 * Interfaces module constructor. Allocates each context.
 */
InterfacesModule::InterfacesModule(ietf::ifc::PluginContext& plugin_ctx)
    : srpc::IModule<ietf::ifc::PluginContext>(plugin_ctx)
{
    m_operContext = std::make_shared<InterfacesOperationalContext>();
    m_changeContext = std::make_shared<InterfacesModuleChangesContext>();
    m_rpcContext = std::make_shared<InterfacesRpcContext>();
}

/**
 * Return the operational context from the module.
 */
std::shared_ptr<srpc::IModuleContext> InterfacesModule::getOperationalContext() { return m_operContext; }

/**
 * Return the module changes context from the module.
 */
std::shared_ptr<srpc::IModuleContext> InterfacesModule::getModuleChangesContext() { return m_changeContext; }

/**
 * Return the RPC context from the module.
 */
std::shared_ptr<srpc::IModuleContext> InterfacesModule::getRpcContext() { return m_rpcContext; }

/**
 * Get all operational callbacks which the module should use.
 */
std::list<srpc::OperationalCallback> InterfacesModule::getOperationalCallbacks()
{
    return {
        srpc::OperationalCallback { "/ietf-interfaces:interfaces/interface", InterfaceOperGetCb(m_operContext) },
    };
}

/**
 * Get all module change callbacks which the module should use.
 */
std::list<srpc::ModuleChangeCallback> InterfacesModule::getModuleChangeCallbacks() { return {}; }

/**
 * Get all RPC callbacks which the module should use.
 */
std::list<srpc::RpcCallback> InterfacesModule::getRpcCallbacks() { return {}; }

/**
 * Get module name.
 */
constexpr const char* InterfacesModule::getName() { return "Interfaces"; }
