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

            const auto& value = change.node.asTerm().value();
            const auto& name_value = std::get<std::string>(value);

            std::string type_xpath = "/ietf-interfaces:interfaces/interface[name='" + name_value + "']/type";

            switch (change.operation) {
            case sysrepo::ChangeOperation::Created: {
                // create a new interface using the NetlinkContext API
                try {
                    std::string type_str = change.node.findPath(type_xpath)->asTerm().valueStr().data();
                    nl_ctx.createInterface(name_value, type_str, true);
                } catch (std::exception& e) {
                    SRPLG_LOG_ERR(getModuleLogPrefix(), "Error creating interface: %s", e.what());
                    return sr::ErrorCode::OperationFailed;
                }

                SRPLG_LOG_DBG(getModuleLogPrefix(), "Created interface %s", name_value.c_str());
                break;
            }
            case sysrepo::ChangeOperation::Deleted:
                // delete interface with 'name' = 'name_value'
                try {
                    nl_ctx.deleteInterface(name_value);
                } catch (std::exception& e) {
                    SRPLG_LOG_ERR(getModuleLogPrefix(), "Cannot remove %s, reason: %s", name_value.c_str(), e.what());
                    return sr::ErrorCode::OperationFailed;
                }

                SRPLG_LOG_DBG(getModuleLogPrefix(), "Deleted interface %s", name_value.c_str());
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
            };

            switch (change.operation) {
            case sysrepo::ChangeOperation::Created:
            case sysrepo::ChangeOperation::Modified:
                // apply 'enabled_value' value for interface 'interface_name'
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

            const auto& value = change.node.asTerm().value();
            const auto& name_value = std::get<std::string>(value);

            switch (change.operation) {
            case sysrepo::ChangeOperation::Created:
            case sysrepo::ChangeOperation::Modified:

                break;
            case sysrepo::ChangeOperation::Deleted:
                // delete interface with 'name' = 'name_value'
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
            const auto& enabled_value = std::get<bool>(value);

            auto& ctx = m_ctx->getNetlinkContext();
            const auto& interface_name = srpc::extractListKeyFromXPath("interface", "name", change.node.path());

            switch (change.operation) {
            case sysrepo::ChangeOperation::Created:
                break;
            case sysrepo::ChangeOperation::Modified: {
                ctx.refillCache();
                auto interface_opt = ctx.getInterfaceByName(interface_name);

                auto data = session.getData("/ietf-interfaces:interfaces/interface/ietf-ip:ipv4");
                if (!data.has_value())
                    return sr::ErrorCode::NotFound;

                auto leaf = data->findXPath("/ietf-interfaces:interfaces/interface[name='" + interface_name + "']/ietf-ip:ipv4/address");

                for (libyang::DataNode&& address_node : leaf) {
                    std::string address = "";
                    int prefix_len = 0;

                    for (libyang::DataNode&& child : address_node.childrenDfs()) {
                        if (std::string(child.schema().name().data()).compare("prefix-length") == 0) {
                            prefix_len = std::stoi(child.asTerm().valueStr().data());
                        } else if (std::string(child.schema().name().data()).compare("ip") == 0) {
                            address = child.asTerm().valueStr().data();
                        }
                    }

                    if (enabled_value) {
                        ctx.createAddress(interface_name, address, prefix_len, AddressFamily::V4);
                    } else {
                        ctx.deleteAddress(interface_name, address, prefix_len, AddressFamily::V4);
                    }
                }

                break;
            }
            case sysrepo::ChangeOperation::Deleted:
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

            const auto& value = change.node.asTerm().value();
            const auto& forwarding_value = std::get<bool>(value);

            // Netlink context
            NlContext& ctx = m_ctx->getNetlinkContext();
            const auto& interface_name = srpc::extractListKeyFromXPath("interface", "name", change.node.path());
            // get interface reference
            std::optional<InterfaceRef> if_ref = ctx.getInterfaceByName(interface_name);

            if (!if_ref.has_value()) {
                SRPLG_LOG_ERR(getModuleLogPrefix(), "Interface %s not found!", interface_name.c_str());
                break; // breaks the getChanges for loop since interface is deleted in upper node
            };

            switch (change.operation) {
            case sysrepo::ChangeOperation::Created:
            case sysrepo::ChangeOperation::Modified: {

                try {
                    if_ref->setForwarding(forwarding_value, AddressFamily::V4);
                } catch (std::exception& e) {
                    SRPLG_LOG_ERR(getModuleLogPrefix(), e.what());
                }

                break;
            }
            case sysrepo::ChangeOperation::Deleted:
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
                try {
                    if_ref ? if_ref->setMtu(mtu_value) : throw std::bad_optional_access();
                } catch (std::exception& e) {
                    SRPLG_LOG_ERR(getModuleLogPrefix(), "Cannot change MTU: %s", e.what());
                }

                break;
            case sysrepo::ChangeOperation::Deleted:
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
        for (sysrepo::Change change : session.getChanges("/ietf-interfaces:interfaces/interface/ipv4/address/ip")) {

            const auto& value = change.node.asTerm().value();
            const auto& address_value = std::get<std::string>(value);
            const auto& interface_name = srpc::extractListKeyFromXPath("interface", "name", change.node.path());

            auto& ctx = m_ctx->getNetlinkContext();

            std::string prefix_len_xpath = "/ietf-interfaces:interfaces/interface[name='" + interface_name + "']/ietf-ip:ipv4/address[ip='"
                + address_value + "']/prefix-length";

            // Enabled node from running DS

            // As default value
            bool enabled_running_ds = true;
            const auto& enabled_data = session.getData("/ietf-interfaces:interfaces/interface[name='" + interface_name + "']/ietf-ip:ipv4");

            if (!enabled_data.has_value()) {
                enabled_running_ds = false;
            } else {
                const auto& enabled_opt
                    = enabled_data->findPath("/ietf-interfaces:interfaces/interface[name='" + interface_name + "']/ietf-ip:ipv4/enabled");
                if (!enabled_data.has_value()) {
                    enabled_running_ds = false;
                } else {
                    enabled_running_ds = std::get<bool>(enabled_opt->asTerm().value());
                }
            }

            switch (change.operation) {
            case sysrepo::ChangeOperation::Created:

                try {
                    ctx.refillCache();
                    const auto& interface_opt = ctx.getInterfaceByName(interface_name);

                    const auto& prefix_len_val = change.node.findPath(prefix_len_xpath)->asTerm().value();
                    int prefix_len = std::get<uint8_t>(prefix_len_val);

                    if (enabled_running_ds) {
                        ctx.createAddress(interface_name, address_value, prefix_len, AddressFamily::V4);
                    }

                } catch (std::exception& e) {
                    SRPLG_LOG_ERR(getModuleLogPrefix(), "Cannot create address: %s", e.what());
                    return sr::ErrorCode::OperationFailed;
                }

                break;
            case sysrepo::ChangeOperation::Deleted:

                try {
                    ctx.refillCache();
                    const auto& interface_opt = ctx.getInterfaceByName(interface_name);

                    // if interface does not exist, its been deleted previously in <interface><name> node
                    if (!interface_opt.has_value()) {
                        SRPLG_LOG_WRN(getModuleLogPrefix(), "Interface %s is allready deleted!", interface_name.c_str());
                        break;
                    };

                    const auto& prefix_len_val = change.node.findPath(prefix_len_xpath)->asTerm().value();
                    int prefix_len = std::get<uint8_t>(prefix_len_val);
                    if (enabled_running_ds) {
                        ctx.deleteAddress(interface_name, address_value, prefix_len, AddressFamily::V4);
                    }

                } catch (std::exception& e) {
                    SRPLG_LOG_ERR(getModuleLogPrefix(), "Cannot delete address: %s", e.what());
                    return sr::ErrorCode::OperationFailed;
                }

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
        for (sysrepo::Change change : session.getChanges("/ietf-interfaces:interfaces/interface/ipv4/address/prefix-length")) {
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.path().c_str());
            SRPLG_LOG_DBG(getModuleLogPrefix(), "Value of %s modified.", change.node.schema().name().data());

            const auto& value = change.node.asTerm().value();
            const auto& prefix_value = std::get<uint8_t>(value);
            const auto& interface_name = srpc::extractListKeyFromXPath("interface", "name", change.node.path());
            const auto& ip_addr = srpc::extractListKeyFromXPath("address", "ip", change.node.path());

            // Netlink context
            NlContext& nl_ctx = m_ctx->getNetlinkContext();

            switch (change.operation) {
            case sysrepo::ChangeOperation::Modified:
                // prefix len is mandatory, can only be modified
                try {
                    nl_ctx.refillCache();
                    int old_prefix_len = std::stoi(change.previousValue->data());
                    bool found = false;

                    for (auto&& addr : nl_ctx.getAddressCache()) {
                        if ((addr.getFamily() == AddressFamily::V4) && (addr.getIPAddress() == ip_addr) && (addr.getPrefix() == old_prefix_len)) {

                            addr.setPrefix(prefix_value);
                            found = true;
                            break;
                        };
                    };

                    if (!found) {
                        SRPLG_LOG_INF(
                            getModuleLogPrefix(), "Address %s not found", std::string(ip_addr + "/" + change.previousValue->data()).c_str());
                        return sr::ErrorCode::NotFound;
                    }

                } catch (std::exception& e) {
                    SRPLG_LOG_ERR(getModuleLogPrefix(), "Cannot modify prefix-length: %s", e.what());
                    return sr::ErrorCode::OperationFailed;
                }

                SRPLG_LOG_DBG(getModuleLogPrefix(), "Prefix length: %d", prefix_value);

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

    switch (event) {
    case sysrepo::Event::Change:
        // apply interface changes to the netlink context received from module changes context
        for (sysrepo::Change change : session.getChanges("/ietf-interfaces:interfaces/interface/ipv4/address")) {

            switch (change.operation) {
            case sysrepo::ChangeOperation::Created:
            case sysrepo::ChangeOperation::Modified:

                break;
            case sysrepo::ChangeOperation::Deleted:
                // delete interface with 'name' = 'name_value'

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

            const auto& value = change.node.asTerm().value();
            const auto& address_value = std::get<std::string>(value);
            const auto& interface_name = srpc::extractListKeyFromXPath("interface", "name", change.node.path());

            auto& ctx = m_ctx->getNetlinkContext();

            std::string lladdr_xpath = "/ietf-interfaces:interfaces/interface[name='" + interface_name + "']/ietf-ip:ipv4/neighbor[ip='"
                + address_value + "']/link-layer-address";

            switch (change.operation) {
            case sysrepo::ChangeOperation::Created:

                try {
                    ctx.refillCache();
                    const auto& interface_opt = ctx.getInterfaceByName(interface_name);

                    const auto& lladdr_val = change.node.findPath(lladdr_xpath)->asTerm().value();
                    std::string ll_addr = std::get<std::string>(lladdr_val);

                    ctx.neighbor(interface_name, address_value, ll_addr, AddressFamily::V4, NeighborOperations::Create);

                } catch (std::exception& e) {
                    SRPLG_LOG_ERR(getModuleLogPrefix(), "Cannot create address: %s", e.what());
                    return sr::ErrorCode::OperationFailed;
                }

                break;
            case sysrepo::ChangeOperation::Deleted:

                try {
                    ctx.refillCache();
                    const auto& interface_opt = ctx.getInterfaceByName(interface_name);

                    // if interface does not exist, its been deleted previously in <interface><name> node
                    if (!interface_opt.has_value()) {
                        SRPLG_LOG_WRN(getModuleLogPrefix(), "Interface %s is allready deleted!", interface_name.c_str());
                        break;
                    };

                    const auto& lladdr_val = change.node.findPath(lladdr_xpath)->asTerm().value();
                    std::string ll_addr = std::get<std::string>(lladdr_val);

                    ctx.neighbor(interface_name, address_value, ll_addr, AddressFamily::V4, NeighborOperations::Delete);

                } catch (std::exception& e) {
                    SRPLG_LOG_ERR(getModuleLogPrefix(), "Cannot delete address: %s", e.what());
                    return sr::ErrorCode::OperationFailed;
                }

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

            const auto& lladdr_value = change.node.asTerm().value();
            const auto& lladdr = std::get<std::string>(lladdr_value);

            const auto& interface_name = srpc::extractListKeyFromXPath("interface", "name", change.node.path());
            const auto& neigh_addr = srpc::extractListKeyFromXPath("neighbor", "ip", change.node.path());

            // Netlink context
            NlContext& nl_ctx = m_ctx->getNetlinkContext();

            switch (change.operation) {
            case sysrepo::ChangeOperation::Modified:
                SRPLG_LOG_DBG(getModuleLogPrefix(), "Link layer address: %s", lladdr.c_str());

                try {

                    nl_ctx.refillCache();
                    nl_ctx.neighbor(interface_name, neigh_addr, lladdr, AddressFamily::V4, NeighborOperations::Modify);

                } catch (std::exception& e) {
                    return sr::ErrorCode::CallbackFailed;
                    SRPLG_LOG_ERR(getModuleLogPrefix(), e.what());
                }

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

            const auto& value = change.node.asTerm().value();
            const auto& enabled_value = std::get<bool>(value);

            auto& ctx = m_ctx->getNetlinkContext();
            const auto& interface_name = srpc::extractListKeyFromXPath("interface", "name", change.node.path());

            switch (change.operation) {
            case sysrepo::ChangeOperation::Created:
                break;
            case sysrepo::ChangeOperation::Modified: {
                SRPLG_LOG_DBG(getModuleLogPrefix(), "Ipv6 enabled: %s", enabled_value ? "true" : "false");
                ctx.refillCache();
                auto interface_opt = ctx.getInterfaceByName(interface_name);

                auto data = session.getData("/ietf-interfaces:interfaces/interface/ietf-ip:ipv6");
                if (!data.has_value())
                    return sr::ErrorCode::NotFound;

                auto leaf = data->findXPath("/ietf-interfaces:interfaces/interface[name='" + interface_name + "']/ietf-ip:ipv6/address");

                for (libyang::DataNode&& address_node : leaf) {
                    std::string address = "";
                    int prefix_len = 0;

                    for (libyang::DataNode&& child : address_node.childrenDfs()) {
                        if (std::string(child.schema().name().data()).compare("prefix-length") == 0) {
                            prefix_len = std::stoi(child.asTerm().valueStr().data());
                        } else if (std::string(child.schema().name().data()).compare("ip") == 0) {
                            address = child.asTerm().valueStr().data();
                        }
                    }

                    if (enabled_value) {
                        ctx.createAddress(interface_name, address, prefix_len, AddressFamily::V6);
                    } else {
                        ctx.deleteAddress(interface_name, address, prefix_len, AddressFamily::V6);
                    }
                }
                break;
            }
            case sysrepo::ChangeOperation::Deleted:
                // delete interface with 'name' = 'name_value'
                SRPLG_LOG_DBG(getModuleLogPrefix(), "Deleted Ipv6 enabled %s", enabled_value ? "true" : "false");
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

            const auto& value = change.node.asTerm().value();
            const auto& name_value = std::get<uint32_t>(value);

            switch (change.operation) {
            case sysrepo::ChangeOperation::Created:
            case sysrepo::ChangeOperation::Modified:

                SRPLG_LOG_DBG(getModuleLogPrefix(), "Mtu: %d", name_value);
                break;
            case sysrepo::ChangeOperation::Deleted:
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

            const auto& value = change.node.asTerm().value();
            const auto& address_value = std::get<std::string>(value);
            const auto& interface_name = srpc::extractListKeyFromXPath("interface", "name", change.node.path());

            auto& ctx = m_ctx->getNetlinkContext();

            std::string prefix_len_xpath = "/ietf-interfaces:interfaces/interface[name='" + interface_name + "']/ietf-ip:ipv6/address[ip='"
                + address_value + "']/prefix-length";

            // Enabled node from running DS

            // As default value
            bool enabled_running_ds = true;
            const auto& enabled_data = session.getData("/ietf-interfaces:interfaces/interface[name='" + interface_name + "']/ietf-ip:ipv6");

            if (!enabled_data.has_value()) {
                enabled_running_ds = false;
            } else {
                const auto& enabled_opt
                    = enabled_data->findPath("/ietf-interfaces:interfaces/interface[name='" + interface_name + "']/ietf-ip:ipv6/enabled");
                if (!enabled_data.has_value()) {
                    enabled_running_ds = false;
                } else {
                    enabled_running_ds = std::get<bool>(enabled_opt->asTerm().value());
                }
            }

            switch (change.operation) {
            case sysrepo::ChangeOperation::Created:

                try {
                    ctx.refillCache();
                    const auto& interface_opt = ctx.getInterfaceByName(interface_name);

                    const auto& prefix_len_val = change.node.findPath(prefix_len_xpath)->asTerm().value();
                    int prefix_len = std::get<uint8_t>(prefix_len_val);

                    if (enabled_running_ds) {
                        ctx.createAddress(interface_name, address_value, prefix_len, AddressFamily::V6);
                    }

                } catch (std::exception& e) {
                    SRPLG_LOG_ERR(getModuleLogPrefix(), "Cannot create address: %s", e.what());
                    return sr::ErrorCode::OperationFailed;
                }
                break;
            case sysrepo::ChangeOperation::Deleted:
                try {
                    ctx.refillCache();
                    const auto& interface_opt = ctx.getInterfaceByName(interface_name);

                    // if interface does not exist, its been deleted previously in <interface><name> node
                    if (!interface_opt.has_value()) {
                        SRPLG_LOG_WRN(getModuleLogPrefix(), "Interface %s is allready deleted!", interface_name.c_str());
                        break;
                    };

                    const auto& prefix_len_val = change.node.findPath(prefix_len_xpath)->asTerm().value();
                    int prefix_len = std::get<uint8_t>(prefix_len_val);

                    if (enabled_running_ds) {
                        ctx.deleteAddress(interface_name, address_value, prefix_len, AddressFamily::V6);
                    }

                } catch (std::exception& e) {
                    SRPLG_LOG_ERR(getModuleLogPrefix(), "Cannot delete address: %s", e.what());
                    return sr::ErrorCode::OperationFailed;
                }
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

            const auto& value = change.node.asTerm().value();
            const auto& prefix_value = std::get<uint8_t>(value);
            const auto& interface_name = srpc::extractListKeyFromXPath("interface", "name", change.node.path());
            const auto& ip_addr = srpc::extractListKeyFromXPath("address", "ip", change.node.path());

            // Netlink context
            NlContext& nl_ctx = m_ctx->getNetlinkContext();

            switch (change.operation) {

            case sysrepo::ChangeOperation::Modified:
                // prefix len is mandatory, can only be modified
                try {
                    nl_ctx.refillCache();
                    int old_prefix_len = std::stoi(change.previousValue->data());
                    bool found = false;

                    for (auto&& addr : nl_ctx.getAddressCache()) {
                        if ((addr.getFamily() == AddressFamily::V6) && (addr.getIPAddress() == ip_addr) && (addr.getPrefix() == old_prefix_len)) {
                            addr.setPrefix(prefix_value);
                            found = true;
                            break;
                        };
                    };

                    if (!found) {
                        SRPLG_LOG_INF(
                            getModuleLogPrefix(), "Address %s not found", std::string(ip_addr + "/" + change.previousValue->data()).c_str());
                        return sr::ErrorCode::NotFound;
                    }

                } catch (std::exception& e) {
                    SRPLG_LOG_ERR(getModuleLogPrefix(), "Cannot modify prefix-length: %s", e.what());
                    return sr::ErrorCode::OperationFailed;
                }

                SRPLG_LOG_DBG(getModuleLogPrefix(), "Prefix length: %d", prefix_value);

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

            const auto& value = change.node.asTerm().value();
            const auto& address_value = std::get<std::string>(value);
            const auto& interface_name = srpc::extractListKeyFromXPath("interface", "name", change.node.path());

            auto& ctx = m_ctx->getNetlinkContext();

            std::string lladdr_xpath = "/ietf-interfaces:interfaces/interface[name='" + interface_name + "']/ietf-ip:ipv6/neighbor[ip='"
                + address_value + "']/link-layer-address";

            switch (change.operation) {
            case sysrepo::ChangeOperation::Created:

                try {
                    ctx.refillCache();
                    const auto& interface_opt = ctx.getInterfaceByName(interface_name);

                    const auto& lladdr_val = change.node.findPath(lladdr_xpath)->asTerm().value();
                    std::string ll_addr = std::get<std::string>(lladdr_val);

                    ctx.neighbor(interface_name, address_value, ll_addr, AddressFamily::V6, NeighborOperations::Create);

                } catch (std::exception& e) {
                    SRPLG_LOG_ERR(getModuleLogPrefix(), "Cannot create address: %s", e.what());
                    return sr::ErrorCode::OperationFailed;
                }

                break;
            case sysrepo::ChangeOperation::Deleted:
                try {
                    ctx.refillCache();
                    const auto& interface_opt = ctx.getInterfaceByName(interface_name);

                    // if interface does not exist, its been deleted previously in <interface><name> node
                    if (!interface_opt.has_value()) {
                        SRPLG_LOG_WRN(getModuleLogPrefix(), "Interface %s is allready deleted!", interface_name.c_str());
                        break;
                    };

                    const auto& lladdr_val = change.node.findPath(lladdr_xpath)->asTerm().value();
                    std::string ll_addr = std::get<std::string>(lladdr_val);

                    ctx.neighbor(interface_name, address_value, ll_addr, AddressFamily::V6, NeighborOperations::Delete);

                } catch (std::exception& e) {
                    SRPLG_LOG_ERR(getModuleLogPrefix(), "Cannot delete address: %s", e.what());
                    return sr::ErrorCode::OperationFailed;
                }
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

            const auto& lladdr_value = change.node.asTerm().value();
            const auto& lladdr = std::get<std::string>(lladdr_value);

            const auto& interface_name = srpc::extractListKeyFromXPath("interface", "name", change.node.path());
            const auto& neigh_addr = srpc::extractListKeyFromXPath("neighbor", "ip", change.node.path());

            // Netlink context
            NlContext& nl_ctx = m_ctx->getNetlinkContext();

            switch (change.operation) {
            case sysrepo::ChangeOperation::Modified:
                SRPLG_LOG_DBG(getModuleLogPrefix(), "Link layer address: %s", lladdr.c_str());

                try {

                    nl_ctx.refillCache();
                    nl_ctx.neighbor(interface_name, neigh_addr, lladdr, AddressFamily::V6, NeighborOperations::Modify);

                } catch (std::exception& e) {
                    return sr::ErrorCode::CallbackFailed;
                    SRPLG_LOG_ERR(getModuleLogPrefix(), e.what());
                }

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
    switch (event) {
    case sysrepo::Event::Change:
        // apply interface changes to the netlink context received from module changes context
        for (sysrepo::Change change : session.getChanges("/ietf-interfaces:interfaces/interface")) {

            std::string value = change.node.asTerm().valueStr().data();

            switch (change.operation) {
            case sysrepo::ChangeOperation::Created:
            case sysrepo::ChangeOperation::Modified:
                SRPLG_LOG_DBG(getModuleLogPrefix(), "Interface: %d", value.c_str());
                break;
            case sysrepo::ChangeOperation::Deleted:
                // delete interface with 'name' = 'name_value'
                SRPLG_LOG_DBG(getModuleLogPrefix(), "Deleted Interface: %d", value.c_str());
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

/**
 * sysrepo-plugin-generator: Generated module change operator() for path
 * /ietf-interfaces:interfaces/interface[name='%s']/ietf-if-extensions:carrier-delay/down.
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
sr::ErrorCode InterfaceCarrierDelayDownModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
InterfaceCarrierDelayUpModuleChangeCb::InterfaceCarrierDelayUpModuleChangeCb(std::shared_ptr<InterfacesModuleChangesContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated module change operator() for path
 * /ietf-interfaces:interfaces/interface[name='%s']/ietf-if-extensions:carrier-delay/up.
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
sr::ErrorCode InterfaceCarrierDelayUpModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
InterfaceCarrierDelayModuleChangeCb::InterfaceCarrierDelayModuleChangeCb(std::shared_ptr<InterfacesModuleChangesContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated module change operator() for path
 * /ietf-interfaces:interfaces/interface[name='%s']/ietf-if-extensions:carrier-delay.
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
sr::ErrorCode InterfaceCarrierDelayModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
InterfaceDampeningHalfLifeModuleChangeCb::InterfaceDampeningHalfLifeModuleChangeCb(std::shared_ptr<InterfacesModuleChangesContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path
 * /ietf-interfaces:interfaces/interface[name='%s']/ietf-if-extensions:dampening/half-life.
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
sr::ErrorCode InterfaceDampeningHalfLifeModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
InterfaceDampeningReuseModuleChangeCb::InterfaceDampeningReuseModuleChangeCb(std::shared_ptr<InterfacesModuleChangesContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated module change operator() for path
 * /ietf-interfaces:interfaces/interface[name='%s']/ietf-if-extensions:dampening/reuse.
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
sr::ErrorCode InterfaceDampeningReuseModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
InterfaceDampeningSuppressModuleChangeCb::InterfaceDampeningSuppressModuleChangeCb(std::shared_ptr<InterfacesModuleChangesContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path
 * /ietf-interfaces:interfaces/interface[name='%s']/ietf-if-extensions:dampening/suppress.
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
sr::ErrorCode InterfaceDampeningSuppressModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
InterfaceDampeningMaxSuppressTimeModuleChangeCb::InterfaceDampeningMaxSuppressTimeModuleChangeCb(std::shared_ptr<InterfacesModuleChangesContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path
 * /ietf-interfaces:interfaces/interface[name='%s']/ietf-if-extensions:dampening/max-suppress-time.
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
sr::ErrorCode InterfaceDampeningMaxSuppressTimeModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
InterfaceDampeningModuleChangeCb::InterfaceDampeningModuleChangeCb(std::shared_ptr<InterfacesModuleChangesContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated module change operator() for path
 * /ietf-interfaces:interfaces/interface[name='%s']/ietf-if-extensions:dampening.
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
sr::ErrorCode InterfaceDampeningModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
InterfaceEncapsulationDot1QVlanOuterTagTagTypeModuleChangeCb::InterfaceEncapsulationDot1QVlanOuterTagTagTypeModuleChangeCb(
    std::shared_ptr<InterfacesModuleChangesContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path
 * /ietf-interfaces:interfaces/interface[name='%s']/ietf-if-extensions:encapsulation/ietf-if-vlan-encapsulation:dot1q-vlan/outer-tag/tag-type.
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
sr::ErrorCode InterfaceEncapsulationDot1QVlanOuterTagTagTypeModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId,
    std::string_view moduleName, std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
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
InterfaceEncapsulationDot1QVlanOuterTagVlanIdModuleChangeCb::InterfaceEncapsulationDot1QVlanOuterTagVlanIdModuleChangeCb(
    std::shared_ptr<InterfacesModuleChangesContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path
 * /ietf-interfaces:interfaces/interface[name='%s']/ietf-if-extensions:encapsulation/ietf-if-vlan-encapsulation:dot1q-vlan/outer-tag/vlan-id.
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
sr::ErrorCode InterfaceEncapsulationDot1QVlanOuterTagVlanIdModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId,
    std::string_view moduleName, std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
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
InterfaceEncapsulationDot1QVlanOuterTagModuleChangeCb::InterfaceEncapsulationDot1QVlanOuterTagModuleChangeCb(
    std::shared_ptr<InterfacesModuleChangesContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path
 * /ietf-interfaces:interfaces/interface[name='%s']/ietf-if-extensions:encapsulation/ietf-if-vlan-encapsulation:dot1q-vlan/outer-tag.
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
sr::ErrorCode InterfaceEncapsulationDot1QVlanOuterTagModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId,
    std::string_view moduleName, std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
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
InterfaceEncapsulationDot1QVlanSecondTagTagTypeModuleChangeCb::InterfaceEncapsulationDot1QVlanSecondTagTagTypeModuleChangeCb(
    std::shared_ptr<InterfacesModuleChangesContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path
 * /ietf-interfaces:interfaces/interface[name='%s']/ietf-if-extensions:encapsulation/ietf-if-vlan-encapsulation:dot1q-vlan/second-tag/tag-type.
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
sr::ErrorCode InterfaceEncapsulationDot1QVlanSecondTagTagTypeModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId,
    std::string_view moduleName, std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
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
InterfaceEncapsulationDot1QVlanSecondTagVlanIdModuleChangeCb::InterfaceEncapsulationDot1QVlanSecondTagVlanIdModuleChangeCb(
    std::shared_ptr<InterfacesModuleChangesContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path
 * /ietf-interfaces:interfaces/interface[name='%s']/ietf-if-extensions:encapsulation/ietf-if-vlan-encapsulation:dot1q-vlan/second-tag/vlan-id.
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
sr::ErrorCode InterfaceEncapsulationDot1QVlanSecondTagVlanIdModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId,
    std::string_view moduleName, std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
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
InterfaceEncapsulationDot1QVlanSecondTagModuleChangeCb::InterfaceEncapsulationDot1QVlanSecondTagModuleChangeCb(
    std::shared_ptr<InterfacesModuleChangesContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path
 * /ietf-interfaces:interfaces/interface[name='%s']/ietf-if-extensions:encapsulation/ietf-if-vlan-encapsulation:dot1q-vlan/second-tag.
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
sr::ErrorCode InterfaceEncapsulationDot1QVlanSecondTagModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId,
    std::string_view moduleName, std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
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
InterfaceEncapsulationDot1QVlanModuleChangeCb::InterfaceEncapsulationDot1QVlanModuleChangeCb(std::shared_ptr<InterfacesModuleChangesContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path
 * /ietf-interfaces:interfaces/interface[name='%s']/ietf-if-extensions:encapsulation/ietf-if-vlan-encapsulation:dot1q-vlan.
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
sr::ErrorCode InterfaceEncapsulationDot1QVlanModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
InterfaceEncapsulationModuleChangeCb::InterfaceEncapsulationModuleChangeCb(std::shared_ptr<InterfacesModuleChangesContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated module change operator() for path
 * /ietf-interfaces:interfaces/interface[name='%s']/ietf-if-extensions:encapsulation.
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
sr::ErrorCode InterfaceEncapsulationModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
InterfaceLoopbackModuleChangeCb::InterfaceLoopbackModuleChangeCb(std::shared_ptr<InterfacesModuleChangesContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-interfaces:interfaces/interface[name='%s']/ietf-if-extensions:loopback.
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
sr::ErrorCode InterfaceLoopbackModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
InterfaceMaxFrameSizeModuleChangeCb::InterfaceMaxFrameSizeModuleChangeCb(std::shared_ptr<InterfacesModuleChangesContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated module change operator() for path
 * /ietf-interfaces:interfaces/interface[name='%s']/ietf-if-extensions:max-frame-size.
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
sr::ErrorCode InterfaceMaxFrameSizeModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
InterfaceParentInterfaceModuleChangeCb::InterfaceParentInterfaceModuleChangeCb(std::shared_ptr<InterfacesModuleChangesContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated module change operator() for path
 * /ietf-interfaces:interfaces/interface[name='%s']/ietf-if-extensions:parent-interface.
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
sr::ErrorCode InterfaceParentInterfaceModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}
