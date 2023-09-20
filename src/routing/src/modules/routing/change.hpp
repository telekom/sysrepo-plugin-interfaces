#pragma once

#include "context.hpp"

#include <iostream>
#include <optional>
#include <string_view>

#include <sysrepo-cpp/Session.hpp>

namespace sr = sysrepo;

namespace ietf::rt {
namespace sub::change {
    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path /ietf-routing:routing/router-id.
     */
    class RouterIdModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        RouterIdModuleChangeCb(std::shared_ptr<RoutingModuleChangesContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path /ietf-routing:routing/router-id.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param event Type of the event that has occured.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * SR_EV_DONE, for example) have the same request ID.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<RoutingModuleChangesContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path
     * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/type.
     */
    class TypeModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        TypeModuleChangeCb(std::shared_ptr<RoutingModuleChangesContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path
         * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/type.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param event Type of the event that has occured.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * SR_EV_DONE, for example) have the same request ID.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<RoutingModuleChangesContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path
     * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/name.
     */
    class ControlPlaneProtocolNameModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        ControlPlaneProtocolNameModuleChangeCb(std::shared_ptr<RoutingModuleChangesContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path
         * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/name.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param event Type of the event that has occured.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * SR_EV_DONE, for example) have the same request ID.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<RoutingModuleChangesContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path
     * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/description.
     */
    class ControlPlaneProtocolDescriptionModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        ControlPlaneProtocolDescriptionModuleChangeCb(std::shared_ptr<RoutingModuleChangesContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path
         * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/description.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param event Type of the event that has occured.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * SR_EV_DONE, for example) have the same request ID.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<RoutingModuleChangesContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path
     * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/destination-prefix.
     */
    class V4RouteDestinationPrefixModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        V4RouteDestinationPrefixModuleChangeCb(std::shared_ptr<RoutingModuleChangesContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path
         * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/destination-prefix.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param event Type of the event that has occured.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * SR_EV_DONE, for example) have the same request ID.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<RoutingModuleChangesContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path
     * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/description.
     */
    class V4RouteDescriptionModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        V4RouteDescriptionModuleChangeCb(std::shared_ptr<RoutingModuleChangesContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path
         * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/description.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param event Type of the event that has occured.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * SR_EV_DONE, for example) have the same request ID.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<RoutingModuleChangesContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path
     * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop/outgoing-interface.
     */
    class V4RouteNextHopOutgoingInterfaceModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        V4RouteNextHopOutgoingInterfaceModuleChangeCb(std::shared_ptr<RoutingModuleChangesContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path
         * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop/outgoing-interface.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param event Type of the event that has occured.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * SR_EV_DONE, for example) have the same request ID.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<RoutingModuleChangesContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path
     * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop/next-hop-address.
     */
    class V4RouteNextHopNextHopAddressModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        V4RouteNextHopNextHopAddressModuleChangeCb(std::shared_ptr<RoutingModuleChangesContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path
         * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop/next-hop-address.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param event Type of the event that has occured.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * SR_EV_DONE, for example) have the same request ID.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<RoutingModuleChangesContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path
     * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop/special-next-hop.
     */
    class V4RouteNextHopSpecialNextHopModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        V4RouteNextHopSpecialNextHopModuleChangeCb(std::shared_ptr<RoutingModuleChangesContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path
         * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop/special-next-hop.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param event Type of the event that has occured.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * SR_EV_DONE, for example) have the same request ID.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<RoutingModuleChangesContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path
     * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop/next-hop-list/next-hop[index='%s']/index.
     */
    class V4RouteNextHopNextHopListNextHopIndexModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        V4RouteNextHopNextHopListNextHopIndexModuleChangeCb(std::shared_ptr<RoutingModuleChangesContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path
         * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop/next-hop-list/next-hop[index='%s']/index.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param event Type of the event that has occured.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * SR_EV_DONE, for example) have the same request ID.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<RoutingModuleChangesContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path
     * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop/next-hop-list/next-hop[index='%s']/outgoing-interface.
     */
    class V4RouteNextHopNextHopListNextHopOutgoingInterfaceModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        V4RouteNextHopNextHopListNextHopOutgoingInterfaceModuleChangeCb(std::shared_ptr<RoutingModuleChangesContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path
         * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop/next-hop-list/next-hop[index='%s']/outgoing-interface.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param event Type of the event that has occured.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * SR_EV_DONE, for example) have the same request ID.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<RoutingModuleChangesContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path
     * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop/next-hop-list/next-hop[index='%s']/next-hop-address.
     */
    class V4RouteNextHopNextHopListNextHopNextHopAddressModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        V4RouteNextHopNextHopListNextHopNextHopAddressModuleChangeCb(std::shared_ptr<RoutingModuleChangesContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path
         * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop/next-hop-list/next-hop[index='%s']/next-hop-address.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param event Type of the event that has occured.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * SR_EV_DONE, for example) have the same request ID.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<RoutingModuleChangesContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path
     * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop/next-hop-list/next-hop[index='%s'].
     */
    class V4RouteNextHopNextHopListNextHopModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        V4RouteNextHopNextHopListNextHopModuleChangeCb(std::shared_ptr<RoutingModuleChangesContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path
         * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop/next-hop-list/next-hop[index='%s'].
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param event Type of the event that has occured.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * SR_EV_DONE, for example) have the same request ID.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<RoutingModuleChangesContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path
     * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop/next-hop-list.
     */
    class V4RouteNextHopNextHopListModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        V4RouteNextHopNextHopListModuleChangeCb(std::shared_ptr<RoutingModuleChangesContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path
         * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop/next-hop-list.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param event Type of the event that has occured.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * SR_EV_DONE, for example) have the same request ID.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<RoutingModuleChangesContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path
     * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop.
     */
    class V4RouteNextHopModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        V4RouteNextHopModuleChangeCb(std::shared_ptr<RoutingModuleChangesContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path
         * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param event Type of the event that has occured.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * SR_EV_DONE, for example) have the same request ID.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<RoutingModuleChangesContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path
     * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s'].
     */
    class V4RouteModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        V4RouteModuleChangeCb(std::shared_ptr<RoutingModuleChangesContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path
         * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s'].
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param event Type of the event that has occured.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * SR_EV_DONE, for example) have the same request ID.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<RoutingModuleChangesContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path
     * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4.
     */
    class V4ModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        V4ModuleChangeCb(std::shared_ptr<RoutingModuleChangesContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path
         * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param event Type of the event that has occured.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * SR_EV_DONE, for example) have the same request ID.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<RoutingModuleChangesContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path
     * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/destination-prefix.
     */
    class V6RouteDestinationPrefixModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        V6RouteDestinationPrefixModuleChangeCb(std::shared_ptr<RoutingModuleChangesContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path
         * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/destination-prefix.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param event Type of the event that has occured.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * SR_EV_DONE, for example) have the same request ID.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<RoutingModuleChangesContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path
     * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/description.
     */
    class V6RouteDescriptionModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        V6RouteDescriptionModuleChangeCb(std::shared_ptr<RoutingModuleChangesContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path
         * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/description.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param event Type of the event that has occured.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * SR_EV_DONE, for example) have the same request ID.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<RoutingModuleChangesContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path
     * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop/outgoing-interface.
     */
    class V6RouteNextHopOutgoingInterfaceModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        V6RouteNextHopOutgoingInterfaceModuleChangeCb(std::shared_ptr<RoutingModuleChangesContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path
         * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop/outgoing-interface.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param event Type of the event that has occured.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * SR_EV_DONE, for example) have the same request ID.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<RoutingModuleChangesContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path
     * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop/next-hop-address.
     */
    class V6RouteNextHopNextHopAddressModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        V6RouteNextHopNextHopAddressModuleChangeCb(std::shared_ptr<RoutingModuleChangesContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path
         * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop/next-hop-address.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param event Type of the event that has occured.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * SR_EV_DONE, for example) have the same request ID.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<RoutingModuleChangesContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path
     * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop/special-next-hop.
     */
    class V6RouteNextHopSpecialNextHopModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        V6RouteNextHopSpecialNextHopModuleChangeCb(std::shared_ptr<RoutingModuleChangesContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path
         * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop/special-next-hop.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param event Type of the event that has occured.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * SR_EV_DONE, for example) have the same request ID.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<RoutingModuleChangesContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path
     * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop/next-hop-list/next-hop[index='%s']/index.
     */
    class V6RouteNextHopNextHopListNextHopIndexModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        V6RouteNextHopNextHopListNextHopIndexModuleChangeCb(std::shared_ptr<RoutingModuleChangesContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path
         * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop/next-hop-list/next-hop[index='%s']/index.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param event Type of the event that has occured.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * SR_EV_DONE, for example) have the same request ID.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<RoutingModuleChangesContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path
     * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop/next-hop-list/next-hop[index='%s']/outgoing-interface.
     */
    class V6RouteNextHopNextHopListNextHopOutgoingInterfaceModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        V6RouteNextHopNextHopListNextHopOutgoingInterfaceModuleChangeCb(std::shared_ptr<RoutingModuleChangesContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path
         * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop/next-hop-list/next-hop[index='%s']/outgoing-interface.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param event Type of the event that has occured.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * SR_EV_DONE, for example) have the same request ID.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<RoutingModuleChangesContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path
     * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop/next-hop-list/next-hop[index='%s']/next-hop-address.
     */
    class V6RouteNextHopNextHopListNextHopNextHopAddressModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        V6RouteNextHopNextHopListNextHopNextHopAddressModuleChangeCb(std::shared_ptr<RoutingModuleChangesContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path
         * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop/next-hop-list/next-hop[index='%s']/next-hop-address.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param event Type of the event that has occured.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * SR_EV_DONE, for example) have the same request ID.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<RoutingModuleChangesContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path
     * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop/next-hop-list/next-hop[index='%s'].
     */
    class V6RouteNextHopNextHopListNextHopModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        V6RouteNextHopNextHopListNextHopModuleChangeCb(std::shared_ptr<RoutingModuleChangesContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path
         * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop/next-hop-list/next-hop[index='%s'].
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param event Type of the event that has occured.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * SR_EV_DONE, for example) have the same request ID.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<RoutingModuleChangesContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path
     * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop/next-hop-list.
     */
    class V6RouteNextHopNextHopListModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        V6RouteNextHopNextHopListModuleChangeCb(std::shared_ptr<RoutingModuleChangesContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path
         * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop/next-hop-list.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param event Type of the event that has occured.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * SR_EV_DONE, for example) have the same request ID.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<RoutingModuleChangesContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path
     * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop.
     */
    class V6RouteNextHopModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        V6RouteNextHopModuleChangeCb(std::shared_ptr<RoutingModuleChangesContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path
         * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param event Type of the event that has occured.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * SR_EV_DONE, for example) have the same request ID.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<RoutingModuleChangesContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path
     * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s'].
     */
    class V6RouteModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        V6RouteModuleChangeCb(std::shared_ptr<RoutingModuleChangesContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path
         * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s'].
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param event Type of the event that has occured.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * SR_EV_DONE, for example) have the same request ID.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<RoutingModuleChangesContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path
     * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6.
     */
    class V6ModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        V6ModuleChangeCb(std::shared_ptr<RoutingModuleChangesContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path
         * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param event Type of the event that has occured.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * SR_EV_DONE, for example) have the same request ID.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<RoutingModuleChangesContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path
     * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes.
     */
    class StaticRoutesModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        StaticRoutesModuleChangeCb(std::shared_ptr<RoutingModuleChangesContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path
         * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param event Type of the event that has occured.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * SR_EV_DONE, for example) have the same request ID.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<RoutingModuleChangesContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path
     * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s'].
     */
    class ControlPlaneProtocolModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        ControlPlaneProtocolModuleChangeCb(std::shared_ptr<RoutingModuleChangesContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path
         * /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s'].
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param event Type of the event that has occured.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * SR_EV_DONE, for example) have the same request ID.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<RoutingModuleChangesContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path /ietf-routing:routing/control-plane-protocols.
     */
    class ControlPlaneProtocolsModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        ControlPlaneProtocolsModuleChangeCb(std::shared_ptr<RoutingModuleChangesContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path /ietf-routing:routing/control-plane-protocols.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param event Type of the event that has occured.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * SR_EV_DONE, for example) have the same request ID.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<RoutingModuleChangesContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path /ietf-routing:routing/ribs/rib[name='%s']/name.
     */
    class RibNameModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        RibNameModuleChangeCb(std::shared_ptr<RoutingModuleChangesContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path /ietf-routing:routing/ribs/rib[name='%s']/name.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param event Type of the event that has occured.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * SR_EV_DONE, for example) have the same request ID.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<RoutingModuleChangesContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path /ietf-routing:routing/ribs/rib[name='%s']/address-family.
     */
    class AddressFamilyModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        AddressFamilyModuleChangeCb(std::shared_ptr<RoutingModuleChangesContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path /ietf-routing:routing/ribs/rib[name='%s']/address-family.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param event Type of the event that has occured.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * SR_EV_DONE, for example) have the same request ID.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<RoutingModuleChangesContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path /ietf-routing:routing/ribs/rib[name='%s']/description.
     */
    class RibDescriptionModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        RibDescriptionModuleChangeCb(std::shared_ptr<RoutingModuleChangesContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path /ietf-routing:routing/ribs/rib[name='%s']/description.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param event Type of the event that has occured.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * SR_EV_DONE, for example) have the same request ID.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<RoutingModuleChangesContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path /ietf-routing:routing/ribs/rib[name='%s'].
     */
    class RibModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        RibModuleChangeCb(std::shared_ptr<RoutingModuleChangesContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path /ietf-routing:routing/ribs/rib[name='%s'].
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param event Type of the event that has occured.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * SR_EV_DONE, for example) have the same request ID.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<RoutingModuleChangesContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path /ietf-routing:routing/ribs.
     */
    class RibsModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        RibsModuleChangeCb(std::shared_ptr<RoutingModuleChangesContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path /ietf-routing:routing/ribs.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param event Type of the event that has occured.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * SR_EV_DONE, for example) have the same request ID.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<RoutingModuleChangesContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path /ietf-routing:routing.
     */
    class RoutingModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        RoutingModuleChangeCb(std::shared_ptr<RoutingModuleChangesContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path /ietf-routing:routing.
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param event Type of the event that has occured.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * SR_EV_DONE, for example) have the same request ID.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<RoutingModuleChangesContext> m_ctx;
    };

}
}
