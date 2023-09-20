#pragma once

#include <srpcpp.hpp>

/**
 * Operational context for the routing module.
 */
class RoutingOperationalContext : public srpc::IModuleContext {
};

/**
 * Module changes context for the routing module.
 */
class RoutingModuleChangesContext : public srpc::IModuleContext {
};

/**
 * RPC context for the routing module.
 */
class RoutingRpcContext : public srpc::IModuleContext { };
