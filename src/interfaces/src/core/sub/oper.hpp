#pragma once

#include "interfaces/src/core/context.hpp"

#include <iostream>
#include <optional>
#include <string_view>

#include <sysrepo-cpp/Session.hpp>
#include <libyang-cpp/Context.hpp>

namespace sr = sysrepo;
namespace ly = libyang;

namespace ietf::ifc {
namespace sub::oper {
    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-interfaces:interfaces/interface[name='%s']/name.
     */
    class InterfaceNameOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        InterfaceNameOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-interfaces:interfaces/interface[name='%s']/name.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-interfaces:interfaces/interface[name='%s']/description.
     */
    class InterfaceDescriptionOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        InterfaceDescriptionOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-interfaces:interfaces/interface[name='%s']/description.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-interfaces:interfaces/interface[name='%s']/type.
     */
    class InterfaceTypeOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        InterfaceTypeOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-interfaces:interfaces/interface[name='%s']/type.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-interfaces:interfaces/interface[name='%s']/enabled.
     */
    class InterfaceEnabledOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        InterfaceEnabledOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-interfaces:interfaces/interface[name='%s']/enabled.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path
     * /ietf-interfaces:interfaces/interface[name='%s']/link-up-down-trap-enable.
     */
    class InterfaceLinkUpDownTrapEnableOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        InterfaceLinkUpDownTrapEnableOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path
         * /ietf-interfaces:interfaces/interface[name='%s']/link-up-down-trap-enable.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-interfaces:interfaces/interface[name='%s']/admin-status.
     */
    class InterfaceAdminStatusOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        InterfaceAdminStatusOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-interfaces:interfaces/interface[name='%s']/admin-status.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-interfaces:interfaces/interface[name='%s']/oper-status.
     */
    class InterfaceOperStatusOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        InterfaceOperStatusOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-interfaces:interfaces/interface[name='%s']/oper-status.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-interfaces:interfaces/interface[name='%s']/last-change.
     */
    class InterfaceLastChangeOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        InterfaceLastChangeOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-interfaces:interfaces/interface[name='%s']/last-change.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-interfaces:interfaces/interface[name='%s']/if-index.
     */
    class InterfaceIfIndexOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        InterfaceIfIndexOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-interfaces:interfaces/interface[name='%s']/if-index.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-interfaces:interfaces/interface[name='%s']/phys-address.
     */
    class InterfacePhysAddressOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        InterfacePhysAddressOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-interfaces:interfaces/interface[name='%s']/phys-address.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-interfaces:interfaces/interface[name='%s']/higher-layer-if.
     */
    class InterfaceHigherLayerIfOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        InterfaceHigherLayerIfOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-interfaces:interfaces/interface[name='%s']/higher-layer-if.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-interfaces:interfaces/interface[name='%s']/lower-layer-if.
     */
    class InterfaceLowerLayerIfOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        InterfaceLowerLayerIfOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-interfaces:interfaces/interface[name='%s']/lower-layer-if.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-interfaces:interfaces/interface[name='%s']/speed.
     */
    class InterfaceSpeedOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        InterfaceSpeedOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-interfaces:interfaces/interface[name='%s']/speed.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path
     * /ietf-interfaces:interfaces/interface[name='%s']/statistics/discontinuity-time.
     */
    class StatsDiscontinuityTimeOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        StatsDiscontinuityTimeOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path
         * /ietf-interfaces:interfaces/interface[name='%s']/statistics/discontinuity-time.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path
     * /ietf-interfaces:interfaces/interface[name='%s']/statistics/in-octets.
     */
    class StatsInOctetsOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        StatsInOctetsOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path
         * /ietf-interfaces:interfaces/interface[name='%s']/statistics/in-octets.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path
     * /ietf-interfaces:interfaces/interface[name='%s']/statistics/in-unicast-pkts.
     */
    class StatsInUnicastPktsOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        StatsInUnicastPktsOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path
         * /ietf-interfaces:interfaces/interface[name='%s']/statistics/in-unicast-pkts.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path
     * /ietf-interfaces:interfaces/interface[name='%s']/statistics/in-broadcast-pkts.
     */
    class StatsInBroadcastPktsOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        StatsInBroadcastPktsOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path
         * /ietf-interfaces:interfaces/interface[name='%s']/statistics/in-broadcast-pkts.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path
     * /ietf-interfaces:interfaces/interface[name='%s']/statistics/in-multicast-pkts.
     */
    class StatsInMulticastPktsOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        StatsInMulticastPktsOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path
         * /ietf-interfaces:interfaces/interface[name='%s']/statistics/in-multicast-pkts.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path
     * /ietf-interfaces:interfaces/interface[name='%s']/statistics/in-discards.
     */
    class StatsInDiscardsOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        StatsInDiscardsOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path
         * /ietf-interfaces:interfaces/interface[name='%s']/statistics/in-discards.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path
     * /ietf-interfaces:interfaces/interface[name='%s']/statistics/in-errors.
     */
    class StatsInErrorsOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        StatsInErrorsOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path
         * /ietf-interfaces:interfaces/interface[name='%s']/statistics/in-errors.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path
     * /ietf-interfaces:interfaces/interface[name='%s']/statistics/in-unknown-protos.
     */
    class StatsInUnknownProtosOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        StatsInUnknownProtosOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path
         * /ietf-interfaces:interfaces/interface[name='%s']/statistics/in-unknown-protos.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path
     * /ietf-interfaces:interfaces/interface[name='%s']/statistics/out-octets.
     */
    class StatsOutOctetsOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        StatsOutOctetsOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path
         * /ietf-interfaces:interfaces/interface[name='%s']/statistics/out-octets.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path
     * /ietf-interfaces:interfaces/interface[name='%s']/statistics/out-unicast-pkts.
     */
    class StatsOutUnicastPktsOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        StatsOutUnicastPktsOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path
         * /ietf-interfaces:interfaces/interface[name='%s']/statistics/out-unicast-pkts.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path
     * /ietf-interfaces:interfaces/interface[name='%s']/statistics/out-broadcast-pkts.
     */
    class StatsOutBroadcastPktsOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        StatsOutBroadcastPktsOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path
         * /ietf-interfaces:interfaces/interface[name='%s']/statistics/out-broadcast-pkts.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path
     * /ietf-interfaces:interfaces/interface[name='%s']/statistics/out-multicast-pkts.
     */
    class StatsOutMulticastPktsOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        StatsOutMulticastPktsOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path
         * /ietf-interfaces:interfaces/interface[name='%s']/statistics/out-multicast-pkts.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path
     * /ietf-interfaces:interfaces/interface[name='%s']/statistics/out-discards.
     */
    class StatsOutDiscardsOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        StatsOutDiscardsOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path
         * /ietf-interfaces:interfaces/interface[name='%s']/statistics/out-discards.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path
     * /ietf-interfaces:interfaces/interface[name='%s']/statistics/out-errors.
     */
    class StatsOutErrorsOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        StatsOutErrorsOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path
         * /ietf-interfaces:interfaces/interface[name='%s']/statistics/out-errors.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-interfaces:interfaces/interface[name='%s']/statistics.
     */
    class InterfaceStatsOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        InterfaceStatsOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-interfaces:interfaces/interface[name='%s']/statistics.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path
     * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv4/enabled.
     */
    class Ipv4EnabledOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        Ipv4EnabledOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path
         * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv4/enabled.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path
     * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv4/forwarding.
     */
    class Ipv4ForwardingOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        Ipv4ForwardingOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path
         * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv4/forwarding.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv4/mtu.
     */
    class Ipv4MtuOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        Ipv4MtuOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv4/mtu.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path
     * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv4/address[ip='%s']/ip.
     */
    class IPv4AddrIpOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        IPv4AddrIpOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path
         * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv4/address[ip='%s']/ip.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path
     * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv4/address[ip='%s']/prefix-length.
     */
    class IPv4AddrPrefixLengthOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        IPv4AddrPrefixLengthOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path
         * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv4/address[ip='%s']/prefix-length.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path
     * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv4/address[ip='%s']/netmask.
     */
    class IPv4AddrNetmaskOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        IPv4AddrNetmaskOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path
         * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv4/address[ip='%s']/netmask.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path
     * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv4/address[ip='%s']/origin.
     */
    class IPv4AddrOriginOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        IPv4AddrOriginOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path
         * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv4/address[ip='%s']/origin.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path
     * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv4/address[ip='%s'].
     */
    class Ipv4AddrOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        Ipv4AddrOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path
         * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv4/address[ip='%s'].
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path
     * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv4/neighbor[ip='%s']/ip.
     */
    class IPv4NeighIpOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        IPv4NeighIpOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path
         * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv4/neighbor[ip='%s']/ip.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path
     * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv4/neighbor[ip='%s']/link-layer-address.
     */
    class IPv4NeighLinkLayerAddressOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        IPv4NeighLinkLayerAddressOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path
         * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv4/neighbor[ip='%s']/link-layer-address.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path
     * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv4/neighbor[ip='%s']/origin.
     */
    class IPv4NeighOriginOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        IPv4NeighOriginOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path
         * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv4/neighbor[ip='%s']/origin.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path
     * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv4/neighbor[ip='%s'].
     */
    class Ipv4NeighOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        Ipv4NeighOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path
         * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv4/neighbor[ip='%s'].
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv4.
     */
    class InterfaceIpv4OperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        InterfaceIpv4OperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv4.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path
     * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/enabled.
     */
    class Ipv6EnabledOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        Ipv6EnabledOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path
         * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/enabled.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path
     * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/forwarding.
     */
    class Ipv6ForwardingOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        Ipv6ForwardingOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path
         * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/forwarding.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/mtu.
     */
    class Ipv6MtuOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        Ipv6MtuOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/mtu.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path
     * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/address[ip='%s']/ip.
     */
    class IPv6AddrIpOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        IPv6AddrIpOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path
         * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/address[ip='%s']/ip.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path
     * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/address[ip='%s']/prefix-length.
     */
    class IPv6AddrPrefixLengthOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        IPv6AddrPrefixLengthOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path
         * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/address[ip='%s']/prefix-length.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path
     * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/address[ip='%s']/origin.
     */
    class IPv6AddrOriginOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        IPv6AddrOriginOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path
         * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/address[ip='%s']/origin.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path
     * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/address[ip='%s']/status.
     */
    class IPv6AddrStatusOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        IPv6AddrStatusOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path
         * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/address[ip='%s']/status.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path
     * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/address[ip='%s'].
     */
    class Ipv6AddrOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        Ipv6AddrOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path
         * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/address[ip='%s'].
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path
     * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/neighbor[ip='%s']/ip.
     */
    class IPv6NeighIpOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        IPv6NeighIpOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path
         * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/neighbor[ip='%s']/ip.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path
     * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/neighbor[ip='%s']/link-layer-address.
     */
    class IPv6NeighLinkLayerAddressOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        IPv6NeighLinkLayerAddressOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path
         * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/neighbor[ip='%s']/link-layer-address.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path
     * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/neighbor[ip='%s']/origin.
     */
    class IPv6NeighOriginOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        IPv6NeighOriginOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path
         * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/neighbor[ip='%s']/origin.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path
     * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/neighbor[ip='%s']/is-router.
     */
    class IPv6NeighIsRouterOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        IPv6NeighIsRouterOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path
         * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/neighbor[ip='%s']/is-router.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path
     * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/neighbor[ip='%s']/state.
     */
    class IPv6NeighStateOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        IPv6NeighStateOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path
         * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/neighbor[ip='%s']/state.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path
     * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/neighbor[ip='%s'].
     */
    class Ipv6NeighOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        Ipv6NeighOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path
         * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/neighbor[ip='%s'].
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path
     * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/dup-addr-detect-transmits.
     */
    class Ipv6DupAddrDetectTransmitsOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        Ipv6DupAddrDetectTransmitsOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path
         * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/dup-addr-detect-transmits.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path
     * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/autoconf/create-global-addresses.
     */
    class Ipv6AutoconfCreateGlobalAddressesOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        Ipv6AutoconfCreateGlobalAddressesOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path
         * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/autoconf/create-global-addresses.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path
     * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/autoconf/create-temporary-addresses.
     */
    class Ipv6AutoconfCreateTemporaryAddressesOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        Ipv6AutoconfCreateTemporaryAddressesOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path
         * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/autoconf/create-temporary-addresses.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path
     * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/autoconf/temporary-valid-lifetime.
     */
    class Ipv6AutoconfTemporaryValidLifetimeOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        Ipv6AutoconfTemporaryValidLifetimeOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path
         * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/autoconf/temporary-valid-lifetime.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path
     * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/autoconf/temporary-preferred-lifetime.
     */
    class Ipv6AutoconfTemporaryPreferredLifetimeOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        Ipv6AutoconfTemporaryPreferredLifetimeOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path
         * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/autoconf/temporary-preferred-lifetime.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path
     * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/autoconf.
     */
    class Ipv6AutoconfOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        Ipv6AutoconfOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path
         * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/autoconf.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6.
     */
    class InterfaceIpv6OperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        InterfaceIpv6OperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-interfaces:interfaces/interface[name='%s'].
     */
    class InterfaceOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        InterfaceOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-interfaces:interfaces/interface[name='%s'].
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-interfaces:interfaces.
     */
    class InterfacesOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        InterfacesOperGetCb(std::shared_ptr<ietf::ifc::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-interfaces:interfaces.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::ifc::OperContext> m_ctx;
    };

}
}