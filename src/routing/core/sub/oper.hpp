#pragma once

#include "core/context.hpp"

#include <iostream>
#include <optional>
#include <string_view>

#include <sysrepo-cpp/Session.hpp>
#include <libyang-cpp/Context.hpp>

namespace sr = sysrepo;
namespace ly = libyang;

namespace ietf::rt {
namespace sub::oper {
/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/router-id.
 */
class RouterIdOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        RouterIdOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/router-id.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/interfaces/interface.
 */
class InterfaceOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        InterfaceOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/interfaces/interface.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/interfaces.
 */
class InterfacesOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        InterfacesOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/interfaces.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/type.
 */
class TypeOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        TypeOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/type.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/name.
 */
class NameOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        NameOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/name.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/description.
 */
class DescriptionOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        DescriptionOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/description.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/destination-prefix.
 */
class V4RouteDestinationPrefixOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        V4RouteDestinationPrefixOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/destination-prefix.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/description.
 */
class V4RouteDescriptionOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        V4RouteDescriptionOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/description.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop/outgoing-interface.
 */
class V4RouteNextHopOutgoingInterfaceOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        V4RouteNextHopOutgoingInterfaceOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop/outgoing-interface.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop/next-hop-address.
 */
class V4RouteNextHopNextHopAddressOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        V4RouteNextHopNextHopAddressOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop/next-hop-address.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop/special-next-hop.
 */
class V4RouteNextHopSpecialNextHopOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        V4RouteNextHopSpecialNextHopOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop/special-next-hop.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop/next-hop-list/next-hop[index='%s']/index.
 */
class V4RouteNextHopNextHopListNextHopIndexOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        V4RouteNextHopNextHopListNextHopIndexOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop/next-hop-list/next-hop[index='%s']/index.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop/next-hop-list/next-hop[index='%s']/outgoing-interface.
 */
class V4RouteNextHopNextHopListNextHopOutgoingInterfaceOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        V4RouteNextHopNextHopListNextHopOutgoingInterfaceOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop/next-hop-list/next-hop[index='%s']/outgoing-interface.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop/next-hop-list/next-hop[index='%s']/next-hop-address.
 */
class V4RouteNextHopNextHopListNextHopNextHopAddressOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        V4RouteNextHopNextHopListNextHopNextHopAddressOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop/next-hop-list/next-hop[index='%s']/next-hop-address.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop/next-hop-list/next-hop[index='%s'].
 */
class V4RouteNextHopNextHopListNextHopOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        V4RouteNextHopNextHopListNextHopOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop/next-hop-list/next-hop[index='%s'].
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop/next-hop-list.
 */
class V4RouteNextHopNextHopListOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        V4RouteNextHopNextHopListOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop/next-hop-list.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop.
 */
class V4RouteNextHopOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        V4RouteNextHopOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s'].
 */
class V4RouteOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        V4RouteOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s'].
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4.
 */
class V4OperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        V4OperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/destination-prefix.
 */
class V6RouteDestinationPrefixOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        V6RouteDestinationPrefixOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/destination-prefix.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/description.
 */
class V6RouteDescriptionOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        V6RouteDescriptionOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/description.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop/outgoing-interface.
 */
class V6RouteNextHopOutgoingInterfaceOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        V6RouteNextHopOutgoingInterfaceOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop/outgoing-interface.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop/next-hop-address.
 */
class V6RouteNextHopNextHopAddressOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        V6RouteNextHopNextHopAddressOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop/next-hop-address.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop/special-next-hop.
 */
class V6RouteNextHopSpecialNextHopOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        V6RouteNextHopSpecialNextHopOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop/special-next-hop.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop/next-hop-list/next-hop[index='%s']/index.
 */
class V6RouteNextHopNextHopListNextHopIndexOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        V6RouteNextHopNextHopListNextHopIndexOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop/next-hop-list/next-hop[index='%s']/index.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop/next-hop-list/next-hop[index='%s']/outgoing-interface.
 */
class V6RouteNextHopNextHopListNextHopOutgoingInterfaceOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        V6RouteNextHopNextHopListNextHopOutgoingInterfaceOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop/next-hop-list/next-hop[index='%s']/outgoing-interface.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop/next-hop-list/next-hop[index='%s']/next-hop-address.
 */
class V6RouteNextHopNextHopListNextHopNextHopAddressOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        V6RouteNextHopNextHopListNextHopNextHopAddressOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop/next-hop-list/next-hop[index='%s']/next-hop-address.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop/next-hop-list/next-hop[index='%s'].
 */
class V6RouteNextHopNextHopListNextHopOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        V6RouteNextHopNextHopListNextHopOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop/next-hop-list/next-hop[index='%s'].
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop/next-hop-list.
 */
class V6RouteNextHopNextHopListOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        V6RouteNextHopNextHopListOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop/next-hop-list.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop.
 */
class V6RouteNextHopOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        V6RouteNextHopOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s'].
 */
class V6RouteOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        V6RouteOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s'].
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6.
 */
class V6OperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        V6OperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes.
 */
class StaticRoutesOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        StaticRoutesOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s'].
 */
class ControlPlaneProtocolOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        ControlPlaneProtocolOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s'].
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/control-plane-protocols.
 */
class ControlPlaneProtocolsOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        ControlPlaneProtocolsOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/control-plane-protocols.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/ribs/rib[name='%s']/name.
 */
class NameOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        NameOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/ribs/rib[name='%s']/name.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/ribs/rib[name='%s']/address-family.
 */
class AddressFamilyOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        AddressFamilyOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/ribs/rib[name='%s']/address-family.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/ribs/rib[name='%s']/default-rib.
 */
class DefaultRibOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        DefaultRibOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/ribs/rib[name='%s']/default-rib.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/ribs/rib[name='%s']/routes/route/route-preference.
 */
class RoutePreferenceOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        RoutePreferenceOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/ribs/rib[name='%s']/routes/route/route-preference.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/ribs/rib[name='%s']/routes/route/next-hop/outgoing-interface.
 */
class OutgoingInterfaceOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        OutgoingInterfaceOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/ribs/rib[name='%s']/routes/route/next-hop/outgoing-interface.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/ribs/rib[name='%s']/routes/route/next-hop/ietf-ipv4-unicast-routing:next-hop-address.
 */
class NextHopAddressOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        NextHopAddressOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/ribs/rib[name='%s']/routes/route/next-hop/ietf-ipv4-unicast-routing:next-hop-address.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/ribs/rib[name='%s']/routes/route/next-hop/ietf-ipv6-unicast-routing:next-hop-address.
 */
class NextHopAddressOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        NextHopAddressOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/ribs/rib[name='%s']/routes/route/next-hop/ietf-ipv6-unicast-routing:next-hop-address.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/ribs/rib[name='%s']/routes/route/next-hop/special-next-hop.
 */
class SpecialNextHopOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        SpecialNextHopOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/ribs/rib[name='%s']/routes/route/next-hop/special-next-hop.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/ribs/rib[name='%s']/routes/route/next-hop/next-hop-list/next-hop/outgoing-interface.
 */
class OutgoingInterfaceOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        OutgoingInterfaceOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/ribs/rib[name='%s']/routes/route/next-hop/next-hop-list/next-hop/outgoing-interface.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/ribs/rib[name='%s']/routes/route/next-hop/next-hop-list/next-hop/ietf-ipv4-unicast-routing:address.
 */
class AddressOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        AddressOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/ribs/rib[name='%s']/routes/route/next-hop/next-hop-list/next-hop/ietf-ipv4-unicast-routing:address.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/ribs/rib[name='%s']/routes/route/next-hop/next-hop-list/next-hop/ietf-ipv6-unicast-routing:address.
 */
class AddressOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        AddressOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/ribs/rib[name='%s']/routes/route/next-hop/next-hop-list/next-hop/ietf-ipv6-unicast-routing:address.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/ribs/rib[name='%s']/routes/route/next-hop/next-hop-list/next-hop.
 */
class NextHopOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        NextHopOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/ribs/rib[name='%s']/routes/route/next-hop/next-hop-list/next-hop.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/ribs/rib[name='%s']/routes/route/next-hop/next-hop-list.
 */
class NextHopListOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        NextHopListOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/ribs/rib[name='%s']/routes/route/next-hop/next-hop-list.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/ribs/rib[name='%s']/routes/route/next-hop.
 */
class NextHopOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        NextHopOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/ribs/rib[name='%s']/routes/route/next-hop.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/ribs/rib[name='%s']/routes/route/source-protocol.
 */
class SourceProtocolOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        SourceProtocolOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/ribs/rib[name='%s']/routes/route/source-protocol.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/ribs/rib[name='%s']/routes/route/active.
 */
class ActiveOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        ActiveOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/ribs/rib[name='%s']/routes/route/active.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/ribs/rib[name='%s']/routes/route/last-updated.
 */
class LastUpdatedOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        LastUpdatedOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/ribs/rib[name='%s']/routes/route/last-updated.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/ribs/rib[name='%s']/routes/route/ietf-ipv4-unicast-routing:destination-prefix.
 */
class DestinationPrefixOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        DestinationPrefixOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/ribs/rib[name='%s']/routes/route/ietf-ipv4-unicast-routing:destination-prefix.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/ribs/rib[name='%s']/routes/route/ietf-ipv6-unicast-routing:destination-prefix.
 */
class DestinationPrefixOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        DestinationPrefixOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/ribs/rib[name='%s']/routes/route/ietf-ipv6-unicast-routing:destination-prefix.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/ribs/rib[name='%s']/routes/route.
 */
class RouteOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        RouteOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/ribs/rib[name='%s']/routes/route.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/ribs/rib[name='%s']/routes.
 */
class RoutesOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        RoutesOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/ribs/rib[name='%s']/routes.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/ribs/rib[name='%s']/description.
 */
class DescriptionOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        DescriptionOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/ribs/rib[name='%s']/description.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/ribs/rib[name='%s'].
 */
class RibOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        RibOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/ribs/rib[name='%s'].
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing/ribs.
 */
class RibsOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        RibsOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing/ribs.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-routing:routing.
 */
class RoutingOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        RoutingOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-routing:routing.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::rt::OperContext> m_ctx;
};

}
}