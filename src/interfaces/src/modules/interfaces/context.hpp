#pragma once

#include "api/nl.hpp"

#include <srpcpp.hpp>

/**
 * Operational context for the interfaces module.
 */
class InterfacesOperationalContext : public srpc::IModuleContext {
public:
    NlContext& getNetlinkContext() { return m_nlContext; }

private:
    NlContext m_nlContext;
};

/**
 * Module changes context for the interfaces module.
 */
class InterfacesModuleChangesContext : public srpc::IModuleContext {
public:
    NlContext& getNetlinkContext() { return m_nlContext; }

private:
    NlContext m_nlContext;
};

/**
 * RPC context for the interfaces module.
 */
class InterfacesRpcContext : public srpc::IModuleContext {
};
