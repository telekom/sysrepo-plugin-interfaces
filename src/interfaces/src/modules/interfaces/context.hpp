#pragma once

#include <srpcpp.hpp>

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
