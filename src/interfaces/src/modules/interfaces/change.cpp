#include "change.hpp"
#include "modules/interfaces/common.hpp"
#include "sysrepo-cpp/Enum.hpp"

#include <stdexcept>
#include <sysrepo.h>

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
InterfaceNameModuleChangeCb::InterfaceNameModuleChangeCb(std::shared_ptr<InterfacesModuleChangesContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-interfaces:interfaces/interface[name='%s']/name.
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
sr::ErrorCode InterfaceNameModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;

    // use netlink context to
    auto& nl_ctx = m_ctx->getNetlinkContext();

    try {
        // update cache
        nl_ctx.refillCache();
    } catch (const std::runtime_error& err) {
        SRPLG_LOG_ERR(getModuleLogPrefix(), "Error refilling cache: %s", err.what());
        return sr::ErrorCode::OperationFailed;
    }

    switch (event) {
    case sysrepo::Event::Change:
        // apply interface changes to the netlink context received from module changes context
        for (sysrepo::Change change : session.getChanges("/ietf-interfaces:interfaces/interface/name")) {
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.path().c_str());
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.schema().name().data());

            const auto& value = change.node.asTerm().value();
            const auto& name_value = std::get<std::string>(value);

            std::string type_xpath = "/ietf-interfaces:interfaces/interface[name='" + name_value + "']/type";

            switch (change.operation) {
            case sysrepo::ChangeOperation::Created: {

                std::string type_str = change.node.findPath(type_xpath)->asTerm().valueStr().data();

                // create a new interface using the NetlinkContext API
                try {
                    nl_ctx.createInterface(name_value, type_str, false);
                } catch (std::exception& e) {
                    SRPLG_LOG_ERR(getModuleLogPrefix(), "Error creating interface: %s", e.what());
                }

                SRPLG_LOG_DBG(getModuleLogPrefix(), "Creating interface %s", name_value.c_str());
                break;
            }

            case sysrepo::ChangeOperation::Deleted:
                // delete interface with 'name' = 'name_value'
                SRPLG_LOG_DBG(getModuleLogPrefix(), "Deleting interface %s", name_value.c_str());
                break;
            default:
                // other options not needed
                break;
            }
        }
        break;
    default:
        break;
    }

    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
InterfaceDescriptionModuleChangeCb::InterfaceDescriptionModuleChangeCb(std::shared_ptr<InterfacesModuleChangesContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-interfaces:interfaces/interface[name='%s']/description.
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
sr::ErrorCode InterfaceDescriptionModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;

    switch (event) {
    case sysrepo::Event::Change:
        // apply interface changes to the netlink context received from module changes context
        for (auto& change : session.getChanges("/ietf-interfaces:interfaces/interface/description")) {
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.path().c_str());
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.schema().name().data());

            const auto& value = change.node.asTerm().value();
            const auto& name_value = std::get<std::string>(value);

            switch (change.operation) {
            case sysrepo::ChangeOperation::Created:
            case sysrepo::ChangeOperation::Modified:

                SRPLG_LOG_DBG(getModuleLogPrefix(), "Description: %s", name_value.c_str());
                break;
            case sysrepo::ChangeOperation::Deleted:
                // delete interface with 'name' = 'name_value'
                SRPLG_LOG_DBG(getModuleLogPrefix(), "Deleted Description %s", name_value.c_str());
                break;
            default:
                // other options not needed
                break;
            }
        }
        break;
    default:
        break;
    }
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
InterfaceTypeModuleChangeCb::InterfaceTypeModuleChangeCb(std::shared_ptr<InterfacesModuleChangesContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-interfaces:interfaces/interface[name='%s']/type.
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
sr::ErrorCode InterfaceTypeModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    // use netlink context to
    auto& nl_ctx = m_ctx->getNetlinkContext();

    try {
        // update cache
        nl_ctx.refillCache();
    } catch (const std::runtime_error& err) {
        SRPLG_LOG_ERR(getModuleLogPrefix(), "Error refilling cache: %s", err.what());
        return sr::ErrorCode::OperationFailed;
    }

    switch (event) {
    case sysrepo::Event::Change:
        // apply interface changes to the netlink context received from module changes context
        for (sysrepo::Change change : session.getChanges("/ietf-interfaces:interfaces/interface/type")) {
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.path().c_str());
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.schema().name().data());

            const std::string& name_value = change.node.asTerm().valueStr().data();

            switch (change.operation) {
            case sysrepo::ChangeOperation::Created:
                // create a new interface using the NetlinkContext API
                SRPLG_LOG_DBG(getModuleLogPrefix(), "Type: %s", name_value.c_str());
                break;
            case sysrepo::ChangeOperation::Deleted:
                // delete interface with 'name' = 'name_value'
                SRPLG_LOG_DBG(getModuleLogPrefix(), "Deleted Type: %s", name_value.c_str());
                break;
            default:
                // other options not needed
                break;
            }
        }
        break;
    default:
        break;
    }
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
InterfaceEnabledModuleChangeCb::InterfaceEnabledModuleChangeCb(std::shared_ptr<InterfacesModuleChangesContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-interfaces:interfaces/interface[name='%s']/enabled.
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
sr::ErrorCode InterfaceEnabledModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;

    switch (event) {
    case sysrepo::Event::Change:
        // apply enabled leaf changes to the netlink context for a specific interface
        for (sysrepo::Change change : session.getChanges("/ietf-interfaces:interfaces/interface/enabled")) {
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.path().c_str());
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.schema().name().data());
            SRPLG_LOG_DBG(getModuleLogPrefix(), "\n%s", change.node.printStr(libyang::DataFormat::XML, libyang::PrintFlags::WithDefaultsAll)->data());

            const auto& value = change.node.asTerm().value();
            const bool enabled_value = std::get<bool>(value);
            const auto& interface_name = srpc::extractListKeyFromXPath("interface", "name", change.node.path());

            std::optional<InterfaceRef> if_ref = std::nullopt;

            try {

                // Netlink context
                NlContext& nl_ctx = m_ctx->getNetlinkContext();
                nl_ctx.refillCache();

                // get interface reference
                if_ref = nl_ctx.getInterfaceByName(interface_name);
            } catch (std::exception& e) {
                SRPLG_LOG_ERR(getModuleLogPrefix(), "Error: %s", e.what());
            }

            switch (change.operation) {
            case sysrepo::ChangeOperation::Created:
            case sysrepo::ChangeOperation::Modified:
                // apply 'enabled_value' value for interface 'interface_name'
                SRPLG_LOG_DBG(getModuleLogPrefix(), "Setting enabled value for interface %s to %s", interface_name.c_str(), enabled_value ? "true" : "false");

                try {
                    if_ref.has_value() ? if_ref->setEnabled(enabled_value) : throw std::bad_optional_access();
                } catch (std::exception& e) {
                    SRPLG_LOG_ERR(getModuleLogPrefix(), "Cannot change Enabled: %s", e.what());
                    return sr::ErrorCode::OperationFailed;
                }
                break;
            case sysrepo::ChangeOperation::Deleted:
                // when deleting this node, the plugin will automaticaly modify this node to default value
                break;
            }
        }
        break;
    default:
        break;
    }

    // disable callback until implemented
    error = sr::ErrorCode::Ok;

    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
InterfaceLinkUpDownTrapEnableModuleChangeCb::InterfaceLinkUpDownTrapEnableModuleChangeCb(std::shared_ptr<InterfacesModuleChangesContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path
 * /ietf-interfaces:interfaces/interface[name='%s']/link-up-down-trap-enable.
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
sr::ErrorCode InterfaceLinkUpDownTrapEnableModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;

    switch (event) {
    case sysrepo::Event::Change:
        // apply interface changes to the netlink context received from module changes context
        for (auto& change : session.getChanges("/ietf-interfaces:interfaces/interface/link-up-down-trap-enable")) {
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.path().c_str());
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.schema().name().data());

            const auto& value = change.node.asTerm().value();
            const auto& name_value = std::get<std::string>(value);

            switch (change.operation) {
            case sysrepo::ChangeOperation::Created:
            case sysrepo::ChangeOperation::Modified:

                SRPLG_LOG_DBG(getModuleLogPrefix(), "link-up-down-trap-enable: %s", name_value.c_str());
                break;
            case sysrepo::ChangeOperation::Deleted:
                // delete interface with 'name' = 'name_value'
                SRPLG_LOG_DBG(getModuleLogPrefix(), "Deleted link-up-down-trap-enable %s", name_value.c_str());
                break;
            default:
                // other options not needed
                break;
            }
        }
        break;
    default:
        break;
    }

    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
Ipv4EnabledModuleChangeCb::Ipv4EnabledModuleChangeCb(std::shared_ptr<InterfacesModuleChangesContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv4/enabled.
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
sr::ErrorCode Ipv4EnabledModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;

    switch (event) {
    case sysrepo::Event::Change:
        // apply interface changes to the netlink context received from module changes context
        for (auto& change : session.getChanges("/ietf-interfaces:interfaces/interface/ipv4/enabled")) {
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.path().c_str());
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.schema().name().data());

            const auto& value = change.node.asTerm().value();
            const auto& name_value = std::get<bool>(value);

            switch (change.operation) {
            case sysrepo::ChangeOperation::Created:
            case sysrepo::ChangeOperation::Modified:

                SRPLG_LOG_DBG(getModuleLogPrefix(), "Ipv4 enabled: %s", name_value ? "true" : "false");
                break;
            case sysrepo::ChangeOperation::Deleted:
                // delete interface with 'name' = 'name_value'
                SRPLG_LOG_DBG(getModuleLogPrefix(), "Deleted Ipv4 enabled %s", name_value ? "true" : "false");
                break;
            default:
                // other options not needed
                break;
            }
        }
        break;
    default:
        break;
    }

    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
Ipv4ForwardingModuleChangeCb::Ipv4ForwardingModuleChangeCb(std::shared_ptr<InterfacesModuleChangesContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv4/forwarding.
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
sr::ErrorCode Ipv4ForwardingModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;

    switch (event) {
    case sysrepo::Event::Change:
        // apply interface changes to the netlink context received from module changes context
        for (auto& change : session.getChanges("/ietf-interfaces:interfaces/interface/ipv4/forwarding")) {
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.path().c_str());
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.schema().name().data());

            const auto& value = change.node.asTerm().value();
            const auto& name_value = std::get<bool>(value);

            switch (change.operation) {
            case sysrepo::ChangeOperation::Created:
            case sysrepo::ChangeOperation::Modified:

                SRPLG_LOG_DBG(getModuleLogPrefix(), "Forwadring: %s", name_value ? "true" : "false");
                break;
            case sysrepo::ChangeOperation::Deleted:
                // delete interface with 'name' = 'name_value'
                SRPLG_LOG_DBG(getModuleLogPrefix(), "Deleted Forwarding %s", name_value ? "true" : "false");
                break;
            default:
                // other options not needed
                break;
            }
        }
        break;
    default:
        break;
    }

    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
Ipv4MtuModuleChangeCb::Ipv4MtuModuleChangeCb(std::shared_ptr<InterfacesModuleChangesContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv4/mtu.
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
sr::ErrorCode Ipv4MtuModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    switch (event) {
    case sysrepo::Event::Change:
        // apply interface changes to the netlink context received from module changes context
        for (auto& change : session.getChanges("/ietf-interfaces:interfaces/interface/ipv4/mtu")) {
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.path().c_str());
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.schema().name().data());

            const auto& value = change.node.asTerm().value();
            const auto& mtu_value = std::get<uint16_t>(value);
            const auto& interface_name = srpc::extractListKeyFromXPath("interface", "name", change.node.path());

            // Netlink context
            NlContext& nl_ctx = m_ctx->getNetlinkContext();

            // get interface reference
            std::optional<InterfaceRef> if_ref = nl_ctx.getInterfaceByName(interface_name);

            switch (change.operation) {
            case sysrepo::ChangeOperation::Created:
            case sysrepo::ChangeOperation::Modified:

                SRPLG_LOG_DBG(getModuleLogPrefix(), "Mtu: %d", mtu_value);

                try {
                    if_ref ? if_ref->setMtu(mtu_value) : throw std::bad_optional_access();
                } catch (std::exception& e) {
                    SRPLG_LOG_ERR(getModuleLogPrefix(), "Cannot change MTU: %s", e.what());
                }

                break;
            case sysrepo::ChangeOperation::Deleted:

                SRPLG_LOG_DBG(getModuleLogPrefix(), "Deleted Mtu %d", mtu_value);
                break;
            default:
                // other options not needed
                break;
            }
        }
        break;
    default:
        break;
    }
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
Ipv4AddrIpModuleChangeCb::Ipv4AddrIpModuleChangeCb(std::shared_ptr<InterfacesModuleChangesContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated module change operator() for path
 * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv4/address[ip='%s']/ip.
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
sr::ErrorCode Ipv4AddrIpModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;

    switch (event) {
    case sysrepo::Event::Change:
        // apply interface changes to the netlink context received from module changes context
        for (auto& change : session.getChanges("/ietf-interfaces:interfaces/interface/ipv4/address/ip")) {
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.path().c_str());
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.schema().name().data());

            const auto& value = change.node.asTerm().value();
            const auto& name_value = std::get<std::string>(value);

            switch (change.operation) {
            case sysrepo::ChangeOperation::Created:
            case sysrepo::ChangeOperation::Modified:

                SRPLG_LOG_DBG(getModuleLogPrefix(), "Ip address: %s", name_value.c_str());
                break;
            case sysrepo::ChangeOperation::Deleted:
                // delete interface with 'name' = 'name_value'
                SRPLG_LOG_DBG(getModuleLogPrefix(), "Deleted Ip address %s", name_value.c_str());
                break;
            default:
                // other options not needed
                break;
            }
        }
        break;
    default:
        break;
    }

    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
Ipv4AddrPrefixLengthModuleChangeCb::Ipv4AddrPrefixLengthModuleChangeCb(std::shared_ptr<InterfacesModuleChangesContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated module change operator() for path
 * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv4/address[ip='%s']/prefix-length.
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
sr::ErrorCode Ipv4AddrPrefixLengthModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;

    switch (event) {
    case sysrepo::Event::Change:
        // apply interface changes to the netlink context received from module changes context
        for (auto& change : session.getChanges("/ietf-interfaces:interfaces/interface/ipv4/address/prefix-length")) {
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.path().c_str());
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.schema().name().data());

            const auto& value = change.node.asTerm().value();
            const auto& name_value = std::get<uint8_t>(value);

            switch (change.operation) {
            case sysrepo::ChangeOperation::Created:
            case sysrepo::ChangeOperation::Modified:

                SRPLG_LOG_DBG(getModuleLogPrefix(), "Prefix length: %d", name_value);
                break;
            case sysrepo::ChangeOperation::Deleted:
                // delete interface with 'name' = 'name_value'
                SRPLG_LOG_DBG(getModuleLogPrefix(), "Deleted Prefix length %d", name_value);
                break;
            default:
                // other options not needed
                break;
            }
        }
        break;
    default:
        break;
    }
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
Ipv4AddrNetmaskModuleChangeCb::Ipv4AddrNetmaskModuleChangeCb(std::shared_ptr<InterfacesModuleChangesContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated module change operator() for path
 * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv4/address[ip='%s']/netmask.
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
sr::ErrorCode Ipv4AddrNetmaskModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;

    switch (event) {
    case sysrepo::Event::Change:
        // apply interface changes to the netlink context received from module changes context
        for (auto& change : session.getChanges("/ietf-interfaces:interfaces/interface/ipv4/address/netmask")) {
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.path().c_str());
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.schema().name().data());

            const auto& value = change.node.asTerm().value();
            const auto& name_value = std::get<std::string>(value);

            switch (change.operation) {
            case sysrepo::ChangeOperation::Created:
            case sysrepo::ChangeOperation::Modified:

                SRPLG_LOG_DBG(getModuleLogPrefix(), "Netmask: %s", name_value.c_str());
                break;
            case sysrepo::ChangeOperation::Deleted:
                // delete interface with 'name' = 'name_value'
                SRPLG_LOG_DBG(getModuleLogPrefix(), "Deleted Netmask %s", name_value.c_str());
                break;
            default:
                // other options not needed
                break;
            }
        }
        break;
    default:
        break;
    }

    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
Ipv4AddrModuleChangeCb::Ipv4AddrModuleChangeCb(std::shared_ptr<InterfacesModuleChangesContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated module change operator() for path
 * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv4/address[ip='%s'].
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
sr::ErrorCode Ipv4AddrModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
Ipv4NeighIpModuleChangeCb::Ipv4NeighIpModuleChangeCb(std::shared_ptr<InterfacesModuleChangesContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated module change operator() for path
 * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv4/neighbor[ip='%s']/ip.
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
sr::ErrorCode Ipv4NeighIpModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;

    switch (event) {
    case sysrepo::Event::Change:
        // apply interface changes to the netlink context received from module changes context
        for (auto& change : session.getChanges("/ietf-interfaces:interfaces/interface/ipv4/neighbor/ip")) {
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.path().c_str());
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.schema().name().data());

            const auto& value = change.node.asTerm().value();
            const auto& name_value = std::get<std::string>(value);

            switch (change.operation) {
            case sysrepo::ChangeOperation::Created:
            case sysrepo::ChangeOperation::Modified:

                SRPLG_LOG_DBG(getModuleLogPrefix(), "Neighbor ip: %s", name_value.c_str());
                break;
            case sysrepo::ChangeOperation::Deleted:
                // delete interface with 'name' = 'name_value'
                SRPLG_LOG_DBG(getModuleLogPrefix(), "Deleted Neighbor ip %s", name_value.c_str());
                break;
            default:
                // other options not needed
                break;
            }
        }
        break;
    default:
        break;
    }

    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
Ipv4NeighLinkLayerAddressModuleChangeCb::Ipv4NeighLinkLayerAddressModuleChangeCb(std::shared_ptr<InterfacesModuleChangesContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated module change operator() for path
 * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv4/neighbor[ip='%s']/link-layer-address.
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
sr::ErrorCode Ipv4NeighLinkLayerAddressModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;

    switch (event) {
    case sysrepo::Event::Change:
        // apply interface changes to the netlink context received from module changes context
        for (auto& change : session.getChanges("/ietf-interfaces:interfaces/interface/ipv4/neighbor/link-layer-address")) {
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.path().c_str());
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.schema().name().data());

            const auto& value = change.node.asTerm().value();
            const auto& name_value = std::get<std::string>(value);

            switch (change.operation) {
            case sysrepo::ChangeOperation::Created:
            case sysrepo::ChangeOperation::Modified:

                SRPLG_LOG_DBG(getModuleLogPrefix(), "Link layer address: %s", name_value.c_str());
                break;
            case sysrepo::ChangeOperation::Deleted:
                // delete interface with 'name' = 'name_value'
                SRPLG_LOG_DBG(getModuleLogPrefix(), "Deleted Link layer address %s", name_value.c_str());
                break;
            default:
                // other options not needed
                break;
            }
        }
        break;
    default:
        break;
    }
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
Ipv4NeighModuleChangeCb::Ipv4NeighModuleChangeCb(std::shared_ptr<InterfacesModuleChangesContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated module change operator() for path
 * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv4/neighbor[ip='%s'].
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
sr::ErrorCode Ipv4NeighModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    // per leaf subscription, no need for whole node ?
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
InterfaceIpv4ModuleChangeCb::InterfaceIpv4ModuleChangeCb(std::shared_ptr<InterfacesModuleChangesContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv4.
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
sr::ErrorCode InterfaceIpv4ModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
Ipv6EnabledModuleChangeCb::Ipv6EnabledModuleChangeCb(std::shared_ptr<InterfacesModuleChangesContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/enabled.
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
sr::ErrorCode Ipv6EnabledModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;

    switch (event) {
    case sysrepo::Event::Change:
        // apply interface changes to the netlink context received from module changes context
        for (auto& change : session.getChanges("/ietf-interfaces:interfaces/interface/ipv6/enabled")) {
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.path().c_str());
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.schema().name().data());

            const auto& value = change.node.asTerm().value();
            const auto& name_value = std::get<bool>(value);

            switch (change.operation) {
            case sysrepo::ChangeOperation::Created:
            case sysrepo::ChangeOperation::Modified:

                SRPLG_LOG_DBG(getModuleLogPrefix(), "Ipv6 enabled: %s", name_value ? "true" : "false");
                break;
            case sysrepo::ChangeOperation::Deleted:
                // delete interface with 'name' = 'name_value'
                SRPLG_LOG_DBG(getModuleLogPrefix(), "Deleted Ipv6 enabled %s", name_value ? "true" : "false");
                break;
            default:
                // other options not needed
                break;
            }
        }
        break;
    default:
        break;
    }

    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
Ipv6ForwardingModuleChangeCb::Ipv6ForwardingModuleChangeCb(std::shared_ptr<InterfacesModuleChangesContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/forwarding.
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
sr::ErrorCode Ipv6ForwardingModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    switch (event) {
    case sysrepo::Event::Change:
        // apply interface changes to the netlink context received from module changes context
        for (auto& change : session.getChanges("/ietf-interfaces:interfaces/interface/ipv6/forwarding")) {
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.path().c_str());
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.schema().name().data());

            const auto& value = change.node.asTerm().value();
            const auto& name_value = std::get<bool>(value);

            switch (change.operation) {
            case sysrepo::ChangeOperation::Created:
            case sysrepo::ChangeOperation::Modified:

                SRPLG_LOG_DBG(getModuleLogPrefix(), "Ipv6 forwarding: %s", name_value ? "true" : "false");
                break;
            case sysrepo::ChangeOperation::Deleted:
                // delete interface with 'name' = 'name_value'
                SRPLG_LOG_DBG(getModuleLogPrefix(), "Deleted Ipv6 forwarding %s", name_value ? "true" : "false");
                break;
            default:
                // other options not needed
                break;
            }
        }
        break;
    default:
        break;
    }
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
Ipv6MtuModuleChangeCb::Ipv6MtuModuleChangeCb(std::shared_ptr<InterfacesModuleChangesContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/mtu.
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
sr::ErrorCode Ipv6MtuModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;

    switch (event) {
    case sysrepo::Event::Change:
        // apply interface changes to the netlink context received from module changes context
        for (auto& change : session.getChanges("/ietf-interfaces:interfaces/interface/ipv6/mtu")) {
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.path().c_str());
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.schema().name().data());

            const auto& value = change.node.asTerm().value();
            const auto& name_value = std::get<uint32_t>(value);

            switch (change.operation) {
            case sysrepo::ChangeOperation::Created:
            case sysrepo::ChangeOperation::Modified:

                SRPLG_LOG_DBG(getModuleLogPrefix(), "Mtu: %d", name_value);
                break;
            case sysrepo::ChangeOperation::Deleted:
                // delete interface with 'name' = 'name_value'
                SRPLG_LOG_DBG(getModuleLogPrefix(), "Deleted Mtu: %d", name_value);
                break;
            default:
                // other options not needed
                break;
            }
        }
        break;
    default:
        break;
    }
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
Ipv6AddrIpModuleChangeCb::Ipv6AddrIpModuleChangeCb(std::shared_ptr<InterfacesModuleChangesContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated module change operator() for path
 * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/address[ip='%s']/ip.
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
sr::ErrorCode Ipv6AddrIpModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;

    switch (event) {
    case sysrepo::Event::Change:
        // apply interface changes to the netlink context received from module changes context
        for (auto& change : session.getChanges("/ietf-interfaces:interfaces/interface/ipv6/address/ip")) {
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.path().c_str());
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.schema().name().data());

            const auto& value = change.node.asTerm().value();
            const auto& name_value = std::get<std::string>(value);

            switch (change.operation) {
            case sysrepo::ChangeOperation::Created:
            case sysrepo::ChangeOperation::Modified:

                SRPLG_LOG_DBG(getModuleLogPrefix(), "Ip Address: %s", name_value.c_str());
                break;
            case sysrepo::ChangeOperation::Deleted:
                // delete interface with 'name' = 'name_value'
                SRPLG_LOG_DBG(getModuleLogPrefix(), "Deleted Ip Address: %s", name_value.c_str());
                break;
            default:
                // other options not needed
                break;
            }
        }
        break;
    default:
        break;
    }

    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
Ipv6AddrPrefixLengthModuleChangeCb::Ipv6AddrPrefixLengthModuleChangeCb(std::shared_ptr<InterfacesModuleChangesContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated module change operator() for path
 * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/address[ip='%s']/prefix-length.
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
sr::ErrorCode Ipv6AddrPrefixLengthModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;

    switch (event) {
    case sysrepo::Event::Change:
        // apply interface changes to the netlink context received from module changes context
        for (auto& change : session.getChanges("/ietf-interfaces:interfaces/interface/ipv6/address/prefix-length")) {
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.path().c_str());
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.schema().name().data());

            const auto& value = change.node.asTerm().value();
            const auto& name_value = std::get<uint8_t>(value);

            switch (change.operation) {
            case sysrepo::ChangeOperation::Created:
            case sysrepo::ChangeOperation::Modified:

                SRPLG_LOG_DBG(getModuleLogPrefix(), "Prefix length: %d", name_value);
                break;
            case sysrepo::ChangeOperation::Deleted:
                // delete interface with 'name' = 'name_value'
                SRPLG_LOG_DBG(getModuleLogPrefix(), "Deleted Prefix length %d", name_value);
                break;
            default:
                // other options not needed
                break;
            }
        }
        break;
    default:
        break;
    }
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
Ipv6AddrModuleChangeCb::Ipv6AddrModuleChangeCb(std::shared_ptr<InterfacesModuleChangesContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated module change operator() for path
 * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/address[ip='%s'].
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
sr::ErrorCode Ipv6AddrModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
Ipv6NeighIpModuleChangeCb::Ipv6NeighIpModuleChangeCb(std::shared_ptr<InterfacesModuleChangesContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated module change operator() for path
 * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/neighbor[ip='%s']/ip.
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
sr::ErrorCode Ipv6NeighIpModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;

    switch (event) {
    case sysrepo::Event::Change:
        // apply interface changes to the netlink context received from module changes context
        for (auto& change : session.getChanges("/ietf-interfaces:interfaces/interface/ipv6/neighbor/ip")) {
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.path().c_str());
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.schema().name().data());

            const auto& value = change.node.asTerm().value();
            const auto& name_value = std::get<std::string>(value);

            switch (change.operation) {
            case sysrepo::ChangeOperation::Created:
            case sysrepo::ChangeOperation::Modified:

                SRPLG_LOG_DBG(getModuleLogPrefix(), "Neighbor ip: %s", name_value.c_str());
                break;
            case sysrepo::ChangeOperation::Deleted:
                // delete interface with 'name' = 'name_value'
                SRPLG_LOG_DBG(getModuleLogPrefix(), "Deleted Neighbor ip: %s", name_value.c_str());
                break;
            default:
                // other options not needed
                break;
            }
        }
        break;
    default:
        break;
    }

    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
Ipv6NeighLinkLayerAddressModuleChangeCb::Ipv6NeighLinkLayerAddressModuleChangeCb(std::shared_ptr<InterfacesModuleChangesContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated module change operator() for path
 * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/neighbor[ip='%s']/link-layer-address.
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
sr::ErrorCode Ipv6NeighLinkLayerAddressModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;

    switch (event) {
    case sysrepo::Event::Change:
        // apply interface changes to the netlink context received from module changes context
        for (auto& change : session.getChanges("/ietf-interfaces:interfaces/interface/ipv6/neighbor/link-layer-address")) {
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.path().c_str());
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.schema().name().data());

            const auto& value = change.node.asTerm().value();
            const auto& name_value = std::get<std::string>(value);

            switch (change.operation) {
            case sysrepo::ChangeOperation::Created:
            case sysrepo::ChangeOperation::Modified:

                SRPLG_LOG_DBG(getModuleLogPrefix(), "Link layer address: %s", name_value.c_str());
                break;
            case sysrepo::ChangeOperation::Deleted:
                // delete interface with 'name' = 'name_value'
                SRPLG_LOG_DBG(getModuleLogPrefix(), "Deleted link layer address: %s", name_value.c_str());
                break;
            default:
                // other options not needed
                break;
            }
        }
        break;
    default:
        break;
    }

    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
Ipv6NeighModuleChangeCb::Ipv6NeighModuleChangeCb(std::shared_ptr<InterfacesModuleChangesContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated module change operator() for path
 * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/neighbor[ip='%s'].
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
sr::ErrorCode Ipv6NeighModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
Ipv6DupAddrDetectTransmitsModuleChangeCb::Ipv6DupAddrDetectTransmitsModuleChangeCb(std::shared_ptr<InterfacesModuleChangesContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path
 * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/dup-addr-detect-transmits.
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
sr::ErrorCode Ipv6DupAddrDetectTransmitsModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    switch (event) {
    case sysrepo::Event::Change:
        // apply interface changes to the netlink context received from module changes context
        for (auto& change : session.getChanges("/ietf-interfaces:interfaces/interface/ipv6/dup-addr-detect-transmits")) {
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.path().c_str());
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.schema().name().data());

            const auto& value = change.node.asTerm().value();
            const auto& name_value = std::get<uint32_t>(value);

            switch (change.operation) {
            case sysrepo::ChangeOperation::Created:
            case sysrepo::ChangeOperation::Modified:

                SRPLG_LOG_DBG(getModuleLogPrefix(), "Dup address detect transmits: %d", name_value);
                break;
            case sysrepo::ChangeOperation::Deleted:
                // delete interface with 'name' = 'name_value'
                SRPLG_LOG_DBG(getModuleLogPrefix(), "Deleted dup address detect transmits: %d", name_value);
                break;
            default:
                // other options not needed
                break;
            }
        }
        break;
    default:
        break;
    }
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
Ipv6AutoconfCreateGlobalAddressesModuleChangeCb::Ipv6AutoconfCreateGlobalAddressesModuleChangeCb(std::shared_ptr<InterfacesModuleChangesContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path
 * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/autoconf/create-global-addresses.
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
sr::ErrorCode Ipv6AutoconfCreateGlobalAddressesModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    switch (event) {
    case sysrepo::Event::Change:
        // apply interface changes to the netlink context received from module changes context
        for (auto& change : session.getChanges("/ietf-interfaces:interfaces/interface/ipv6/autoconf/create-global-addresses")) {
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.path().c_str());
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.schema().name().data());

            const auto& value = change.node.asTerm().value();
            const auto& name_value = std::get<bool>(value);

            switch (change.operation) {
            case sysrepo::ChangeOperation::Created:
            case sysrepo::ChangeOperation::Modified:

                SRPLG_LOG_DBG(getModuleLogPrefix(), "create-global-addresses: %s", name_value ? "true" : "false");
                break;
            case sysrepo::ChangeOperation::Deleted:
                // delete interface with 'name' = 'name_value'
                SRPLG_LOG_DBG(getModuleLogPrefix(), " Deleted create-global-addresses: %s", name_value ? "true" : "false");
                break;
            default:
                // other options not needed
                break;
            }
        }
        break;
    default:
        break;
    }

    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
Ipv6AutoconfCreateTemporaryAddressesModuleChangeCb::Ipv6AutoconfCreateTemporaryAddressesModuleChangeCb(
    std::shared_ptr<InterfacesModuleChangesContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path
 * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/autoconf/create-temporary-addresses.
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
sr::ErrorCode Ipv6AutoconfCreateTemporaryAddressesModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId,
    std::string_view moduleName, std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;

    switch (event) {
    case sysrepo::Event::Change:
        // apply interface changes to the netlink context received from module changes context
        for (auto& change : session.getChanges("/ietf-interfaces:interfaces/interface/ipv6/autoconf/create-temporary-addresses")) {
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.path().c_str());
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.schema().name().data());

            const auto& value = change.node.asTerm().value();
            const auto& name_value = std::get<bool>(value);

            switch (change.operation) {
            case sysrepo::ChangeOperation::Created:
            case sysrepo::ChangeOperation::Modified:

                SRPLG_LOG_DBG(getModuleLogPrefix(), "create-temporary-addresses: %s", name_value ? "true" : "false");
                break;
            case sysrepo::ChangeOperation::Deleted:
                // delete interface with 'name' = 'name_value'
                SRPLG_LOG_DBG(getModuleLogPrefix(), "Deleted create-temporary-addresses: %s", name_value ? "true" : "false");
                break;
            default:
                // other options not needed
                break;
            }
        }
        break;
    default:
        break;
    }
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
Ipv6AutoconfTemporaryValidLifetimeModuleChangeCb::Ipv6AutoconfTemporaryValidLifetimeModuleChangeCb(
    std::shared_ptr<InterfacesModuleChangesContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path
 * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/autoconf/temporary-valid-lifetime.
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
sr::ErrorCode Ipv6AutoconfTemporaryValidLifetimeModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;

    switch (event) {
    case sysrepo::Event::Change:
        // apply interface changes to the netlink context received from module changes context
        for (auto& change : session.getChanges("/ietf-interfaces:interfaces/interface/ipv6/autoconf/temporary-valid-lifetime")) {
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.path().c_str());
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.schema().name().data());

            const auto& value = change.node.asTerm().value();
            const auto& name_value = std::get<uint32_t>(value);

            switch (change.operation) {
            case sysrepo::ChangeOperation::Created:
            case sysrepo::ChangeOperation::Modified:

                SRPLG_LOG_DBG(getModuleLogPrefix(), "temporary-valid-lifetime: %d", name_value);
                break;
            case sysrepo::ChangeOperation::Deleted:
                // delete interface with 'name' = 'name_value'
                SRPLG_LOG_DBG(getModuleLogPrefix(), "Deleted temporary-valid-lifetime: %d", name_value);
                break;
            default:
                // other options not needed
                break;
            }
        }
        break;
    default:
        break;
    }

    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
Ipv6AutoconfTemporaryPreferredLifetimeModuleChangeCb::Ipv6AutoconfTemporaryPreferredLifetimeModuleChangeCb(
    std::shared_ptr<InterfacesModuleChangesContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path
 * /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/autoconf/temporary-preferred-lifetime.
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
sr::ErrorCode Ipv6AutoconfTemporaryPreferredLifetimeModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId,
    std::string_view moduleName, std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    switch (event) {
    case sysrepo::Event::Change:
        // apply interface changes to the netlink context received from module changes context
        for (auto& change : session.getChanges("/ietf-interfaces:interfaces/interface/ipv6/autoconf/temporary-preferred-lifetime")) {
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.path().c_str());
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.schema().name().data());

            const auto& value = change.node.asTerm().value();
            const auto& name_value = std::get<uint32_t>(value);

            switch (change.operation) {
            case sysrepo::ChangeOperation::Created:
            case sysrepo::ChangeOperation::Modified:

                SRPLG_LOG_DBG(getModuleLogPrefix(), "temporary-preferred-lifetime: %d", name_value);
                break;
            case sysrepo::ChangeOperation::Deleted:
                // delete interface with 'name' = 'name_value'
                SRPLG_LOG_DBG(getModuleLogPrefix(), "Deleted temporary-preferred-lifetime: %d", name_value);
                break;
            default:
                // other options not needed
                break;
            }
        }
        break;
    default:
        break;
    }

    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
Ipv6AutoconfModuleChangeCb::Ipv6AutoconfModuleChangeCb(std::shared_ptr<InterfacesModuleChangesContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6/autoconf.
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
sr::ErrorCode Ipv6AutoconfModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
InterfaceIpv6ModuleChangeCb::InterfaceIpv6ModuleChangeCb(std::shared_ptr<InterfacesModuleChangesContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-interfaces:interfaces/interface[name='%s']/ietf-ip:ipv6.
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
sr::ErrorCode InterfaceIpv6ModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
InterfaceModuleChangeCb::InterfaceModuleChangeCb(std::shared_ptr<InterfacesModuleChangesContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-interfaces:interfaces/interface[name='%s'].
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
sr::ErrorCode InterfaceModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
InterfacesModuleChangeCb::InterfacesModuleChangeCb(std::shared_ptr<InterfacesModuleChangesContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-interfaces:interfaces.
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
sr::ErrorCode InterfacesModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}
