#pragma once

#include "interfaces/src/core/context.hpp"

#include <iostream>
#include <optional>
#include <string_view>

#include <sysrepo-cpp/Session.hpp>

namespace sr = sysrepo;

namespace ietf::ifc {
namespace sub::change {
    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path /ietf-interfaces:interfaces/interface[name='%s'].
     */
    class InterfaceModuleEnabledChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        InterfaceModuleEnabledChangeCb(std::shared_ptr<ietf::ifc::ModuleChangeContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path /ietf-interfaces:interfaces/interface[name='%s'].
         *
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param moduleName The module name used for subscribing.
         * @param subXPath The optional xpath used at the time of subscription.
         * @param event Type of the event that has occured.
         * @param requestId Request ID unique for the specific . Connected events for one request (SR_EV_CHANGE and
         * SR_EV_DONE, for example) have the same request ID.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<ietf::ifc::ModuleChangeContext> m_ctx;
    };

    class InterfaceModuleChangeCb {
    public:
        InterfaceModuleChangeCb(std::shared_ptr<ietf::ifc::ModuleChangeContext> ctx);

        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<ietf::ifc::ModuleChangeContext> m_ctx;
    };

    class InterfaceModuleNameChangeCb {
    public:
        InterfaceModuleNameChangeCb(std::shared_ptr<ietf::ifc::ModuleChangeContext> ctx);

        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<ietf::ifc::ModuleChangeContext> m_ctx;
    };

    class InterfaceModuleTypeChangeCb {
    public:
        InterfaceModuleTypeChangeCb(std::shared_ptr<ietf::ifc::ModuleChangeContext> ctx);

        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<ietf::ifc::ModuleChangeContext> m_ctx;
    };

    class InterfaceModuleIPV4EnableChangeCb {
    public:
        InterfaceModuleIPV4EnableChangeCb(std::shared_ptr<ietf::ifc::ModuleChangeContext> ctx);

        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<ietf::ifc::ModuleChangeContext> m_ctx;
    };

    class InterfaceModuleIPV4MtuChangeCb {
    public:
        InterfaceModuleIPV4MtuChangeCb(std::shared_ptr<ietf::ifc::ModuleChangeContext> ctx);

        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<ietf::ifc::ModuleChangeContext> m_ctx;
    };

    class InterfaceModuleIPV4AddressChangeCb {
    public:
        InterfaceModuleIPV4AddressChangeCb(std::shared_ptr<ietf::ifc::ModuleChangeContext> ctx);

        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<ietf::ifc::ModuleChangeContext> m_ctx;
    };

    class InterfaceModuleIPV4PrefixChangeCb {
    public:
        InterfaceModuleIPV4PrefixChangeCb(std::shared_ptr<ietf::ifc::ModuleChangeContext> ctx);

        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<ietf::ifc::ModuleChangeContext> m_ctx;
    };

    class InterfaceModuleIPV4NeighbourIpChangeCb {
    public:
        InterfaceModuleIPV4NeighbourIpChangeCb(std::shared_ptr<ietf::ifc::ModuleChangeContext> ctx);

        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<ietf::ifc::ModuleChangeContext> m_ctx;
    };

}
}