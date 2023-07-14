#pragma once

#include <srpcpp.hpp>

#include "core/context.hpp"

/**
 * Operational context for the interfaces module.
 */
class InterfacesOperationalContext : public srpc::IModuleContext { };

/**
 * Module changes context for the interfaces module.
 */
class InterfacesModuleChangesContext : public srpc::IModuleContext { };

/**
 * RPC context for the interfaces module.
 */
class InterfacesRpcContext : public srpc::IModuleContext { };

/**
 * @brief Interfaces module.
 */
class InterfacesModule : public srpc::IModule<ietf::ifc::PluginContext> {
public:
    /**
     * Interfaces module constructor. Allocates each context.
     */
    InterfacesModule(ietf::ifc::PluginContext& plugin_ctx);

    /**
     * Return the operational context from the module.
     */
    virtual std::shared_ptr<srpc::IModuleContext> getOperationalContext() override;

    /**
     * Return the module changes context from the module.
     */
    virtual std::shared_ptr<srpc::IModuleContext> getModuleChangesContext() override;

    /**
     * Return the RPC context from the module.
     */
    virtual std::shared_ptr<srpc::IModuleContext> getRpcContext() override;

    /**
     * Get all operational callbacks which the module should use.
     */
    virtual std::list<srpc::OperationalCallback> getOperationalCallbacks() override;

    /**
     * Get all module change callbacks which the module should use.
     */
    virtual std::list<srpc::ModuleChangeCallback> getModuleChangeCallbacks() override;

    /**
     * Get all RPC callbacks which the module should use.
     */
    virtual std::list<srpc::RpcCallback> getRpcCallbacks() override;

    /**
     * Get module name.
     */
    virtual constexpr const char* getName() override;

    /**
     * System module destructor.
     */
    ~InterfacesModule() { }

private:
    std::shared_ptr<InterfacesOperationalContext> m_operContext;
    std::shared_ptr<InterfacesModuleChangesContext> m_changeContext;
    std::shared_ptr<InterfacesRpcContext> m_rpcContext;
};
