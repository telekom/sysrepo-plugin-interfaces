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
        srpc::OperationalCallback {
            "ietf-interfaces",
            "/ietf-interfaces:interfaces/interface",
            InterfaceOperGetCb(m_operContext),
        },
        srpc::OperationalCallback {
            "ietf-interfaces",
            "/ietf-interfaces:interfaces/interface/admin-status",
            InterfaceAdminStatusOperGetCb(m_operContext),
        },
        srpc::OperationalCallback {
            "ietf-interfaces",
            "/ietf-interfaces:interfaces/interface/oper-status",
            InterfaceOperStatusOperGetCb(m_operContext),
        },
        srpc::OperationalCallback {
            "ietf-interfaces",
            "/ietf-interfaces:interfaces/interface/if-index",
            InterfaceIfIndexOperGetCb(m_operContext),
        },
        srpc::OperationalCallback {
            "ietf-interfaces",
            "/ietf-interfaces:interfaces/interface/phys-address",
            InterfacePhysAddressOperGetCb(m_operContext),
        },
        srpc::OperationalCallback {
            "ietf-interfaces",
            "/ietf-interfaces:interfaces/interface/higher-layer-if",
            InterfaceHigherLayerIfOperGetCb(m_operContext),
        },
        srpc::OperationalCallback {
            "ietf-interfaces",
            "/ietf-interfaces:interfaces/interface/lower-layer-if",
            InterfaceLowerLayerIfOperGetCb(m_operContext),
        },
        srpc::OperationalCallback {
            "ietf-interfaces",
            "/ietf-interfaces:interfaces/interface/speed",
            InterfaceSpeedOperGetCb(m_operContext),
        },
        srpc::OperationalCallback {
            "ietf-interfaces",
            "/ietf-interfaces:interfaces/interface/statistics",
            InterfaceStatsOperGetCb(m_operContext),
        },
        srpc::OperationalCallback {
            "ietf-interfaces",
            "/ietf-interfaces:interfaces/interface/ipv4/address",
            Ipv4AddrOperGetCb(m_operContext),
        },
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
