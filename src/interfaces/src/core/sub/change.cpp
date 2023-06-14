#include "change.hpp"
#include <iostream>
#include <optional>
#include <libyang-cpp/DataNode.hpp>
#include <interfaces/src/core/system/interface.hpp>
#include <sysrepo.h>

using std::cout;
using std::endl;

namespace ietf::ifc {
namespace sub::change {
    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin module change context.
     *
     */
    InterfaceModuleEnabledChangeCb::InterfaceModuleEnabledChangeCb(std::shared_ptr<ietf::ifc::ModuleChangeContext> ctx) { m_ctx = ctx; }

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

    InterfaceModuleTypeChangeCb::InterfaceModuleTypeChangeCb(std::shared_ptr<ietf::ifc::ModuleChangeContext> ctx) { m_ctx = ctx; }
    InterfaceModuleIPV4EnableChangeCb::InterfaceModuleIPV4EnableChangeCb(std::shared_ptr<ietf::ifc::ModuleChangeContext> ctx) { m_ctx = ctx; }
    InterfaceModuleIPV4MtuChangeCb::InterfaceModuleIPV4MtuChangeCb(std::shared_ptr<ietf::ifc::ModuleChangeContext> ctx) { m_ctx = ctx; }
    InterfaceModuleIPV4AddressChangeCb::InterfaceModuleIPV4AddressChangeCb(std::shared_ptr<ietf::ifc::ModuleChangeContext> ctx) { m_ctx = ctx; }
    InterfaceModuleIPV4PrefixChangeCb::InterfaceModuleIPV4PrefixChangeCb(std::shared_ptr<ietf::ifc::ModuleChangeContext> ctx) { m_ctx = ctx; }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    sr::ErrorCode InterfaceModuleEnabledChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
        std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
    {

        sr::ErrorCode error = sr::ErrorCode::Ok;

        switch (event) {
        case sysrepo::Event::Change:
            for (sysrepo::Change change : session.getChanges(subXPath->data())) {
                switch (change.operation) {
                case sysrepo::ChangeOperation::Created:
                case sysrepo::ChangeOperation::Modified: {

                    libyang::DataNode tmp(change.node.firstSibling());
                    std::string name = tmp.asTerm().valueStr().data();

                    libyang::Value value = change.node.asTerm().value();
                    bool enabled = std::get<bool>(value);
                    int ifindex = getIfindexFromName(name);

                    if (ifindex == 0) {
                        SRPLG_LOG_ERR("ietf_interfaces", "non-existing ifindex at interface %s", name);
                        return sr::ErrorCode::OperationFailed;
                    } else {
                        try {
                            Interface(ifindex).setEnabled(enabled);
                        } catch (std::runtime_error& e) {
                            SRPLG_LOG_ERR("ietf_interfaces", "Error changing interface state, reason:  %s", e.what());
                            return sr::ErrorCode::OperationFailed;
                        }
                    }
                }
                case sysrepo::ChangeOperation::Deleted:
                    break;
                }
            }
            break;
        default:
            break;
        }

        return error;
    }

    sr::ErrorCode InterfaceModuleTypeChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
        std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
    {

        sr::ErrorCode error = sr::ErrorCode::Ok;

        switch (event) {
        case sysrepo::Event::Change:
            for (sysrepo::Change change : session.getChanges(subXPath->data())) {
                switch (change.operation) {
                case sysrepo::ChangeOperation::Created:
                case sysrepo::ChangeOperation::Modified: {

                    libyang::DataNode tmp(change.node.firstSibling());
                    std::string name = tmp.asTerm().valueStr().data();

                    std::string value = change.node.asTerm().valueStr().data();

                    // libyang::Value value = change.node.asTerm().value();
                    // int type = std::get<int>(value);

                    int ifindex = getIfindexFromName(name);

                    if (ifindex == 0) {
                        SRPLG_LOG_ERR("ietf_interfaces", "non-existing ifindex at interface %s", name.c_str());
                        return sr::ErrorCode::OperationFailed;
                    } else {
                        try {
                            cout << "Previous: " << Interface(ifindex).getType() << endl;
                            Interface(ifindex).setType(value);
                            cout << "After change: " << Interface(ifindex).getType() << endl;
                        } catch (std::runtime_error& e) {
                            SRPLG_LOG_ERR("ietf_interfaces", "Error changing interface type, reason:  %s", e.what());
                            return sr::ErrorCode::OperationFailed;
                        }
                    }
                }
                case sysrepo::ChangeOperation::Deleted:
                    break;
                }
            }
            break;
        default:
            break;
        }

        return error;
    }

    sr::ErrorCode InterfaceModuleIPV4EnableChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
        std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
    {

        sr::ErrorCode error = sr::ErrorCode::Ok;

        switch (event) {
        case sysrepo::Event::Change:
            for (sysrepo::Change change : session.getChanges(subXPath->data())) {
                switch (change.operation) {
                case sysrepo::ChangeOperation::Created:
                case sysrepo::ChangeOperation::Modified: {

                    libyang::DataNode tmp(change.node);
                    while (std::string(tmp.schema().name().data()) != "interface") {

                        if (tmp.parent().has_value()) {
                            tmp = tmp.parent().value();
                        } else {
                            break;
                        }
                    }
                    std::string name_path = tmp.path().append("/name");
                    std::optional<libyang::DataNode> name_node = tmp.findPath(name_path);
                    if (!name_node.has_value()) {
                        SRPLG_LOG_ERR("ietf_interfaces", "Error traversing node! ");
                        return sr::ErrorCode::OperationFailed;
                    };

                    std::string name = name_node.value().asTerm().valueStr().data();
                    // set the enable disable ipv4 here
                }
                case sysrepo::ChangeOperation::Deleted:
                    break;
                }
            }
            break;
        default:
            break;
        }

        return error;
    }

    sr::ErrorCode InterfaceModuleIPV4MtuChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
        std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
    {

        sr::ErrorCode error = sr::ErrorCode::Ok;

        switch (event) {
        case sysrepo::Event::Change:
            for (sysrepo::Change change : session.getChanges(subXPath->data())) {
                switch (change.operation) {
                case sysrepo::ChangeOperation::Created:
                case sysrepo::ChangeOperation::Modified: {

                    libyang::DataNode tmp(change.node);
                    while (std::string(tmp.schema().name().data()) != "interface") {

                        if (tmp.parent().has_value()) {
                            tmp = tmp.parent().value();
                        } else {
                            break;
                        }
                    }
                    std::string name_path = tmp.path().append("/name");
                    std::optional<libyang::DataNode> name_node = tmp.findPath(name_path);
                    if (!name_node.has_value()) {
                        SRPLG_LOG_ERR("ietf_interfaces", "Error traversing node! ");
                        return sr::ErrorCode::OperationFailed;
                    };

                    std::string name = name_node.value().asTerm().valueStr().data();
                    libyang::Value value = change.node.asTerm().value();
                    int mtu = std::get<uint16_t>(value);
                    IPV4(getIfindexFromName(name)).setMtu(mtu);
                }
                case sysrepo::ChangeOperation::Deleted:
                    break;
                }
            }
            break;
        default:
            break;
        }

        return error;
    }

    sr::ErrorCode InterfaceModuleIPV4AddressChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
        std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
    {

        sr::ErrorCode error = sr::ErrorCode::Ok;

        switch (event) {
        case sysrepo::Event::Change:
            for (sysrepo::Change change : session.getChanges(subXPath->data())) {
                switch (change.operation) {
                case sysrepo::ChangeOperation::Created: {
                    // changed value
                    std::string val = change.node.asTerm().valueStr().data();

                    // name of ifindex
                    libyang::DataNode name_tmp(change.node);
                    while (std::string(name_tmp.schema().name().data()) != "interface") {

                        if (name_tmp.parent().has_value()) {
                            name_tmp = name_tmp.parent().value();
                        } else {
                            break;
                        }
                    }
                    std::string name_path = name_tmp.path().append("/name");
                    std::optional<libyang::DataNode> name_node = name_tmp.findPath(name_path);
                    if (!name_node.has_value()) {
                        SRPLG_LOG_ERR("ietf_interfaces", "Error traversing node! ");
                        return sr::ErrorCode::OperationFailed;
                    };

                    // name of interface
                    std::string if_name = name_node.value().asTerm().valueStr().data();

                    libyang::DataNode tmp(change.node);
                    if (tmp.parent().has_value()) {
                        tmp = tmp.parent().value();
                    } else {
                        SRPLG_LOG_ERR("ietf_interfaces", "Failed to obtain parent node to prefix-length! ");
                        return sr::ErrorCode::OperationFailed;
                    }

                    std::string ip_path = tmp.path().append("/prefix-length");

                    std::optional<libyang::DataNode> ip_node = tmp.findPath(ip_path);

                    if (!ip_node.has_value()) {
                        SRPLG_LOG_ERR("ietf_interfaces", "Failed to find path to prefix-length! ");
                        return sr::ErrorCode::OperationFailed;
                    }

                    std::string ip_prefix = ip_node.value().asTerm().valueStr().data();

                    // uncomment for debug
                    // cout << "Created prefix " << ip_prefix << " Created if_name " << if_name << " Created addr " << val << endl;

                    try {
                        int ifindex = getIfindexFromName(if_name);
                        IPV4(ifindex).addAddress(Address(val, std::stoi(ip_prefix)));
                    } catch (std::runtime_error& e) {
                        SRPLG_LOG_ERR("ietf_interfaces", "%s", e.what());
                        return sr::ErrorCode::OperationFailed;
                    }

                    break;
                }
                case sysrepo::ChangeOperation::Deleted: {

                    // changed value
                    std::string val = change.node.asTerm().valueStr().data();

                    // name of ifindex
                    libyang::DataNode name_tmp(change.node);
                    while (std::string(name_tmp.schema().name().data()) != "interface") {

                        if (name_tmp.parent().has_value()) {
                            name_tmp = name_tmp.parent().value();
                        } else {
                            break;
                        }
                    }
                    std::string name_path = name_tmp.path().append("/name");
                    std::optional<libyang::DataNode> name_node = name_tmp.findPath(name_path);
                    if (!name_node.has_value()) {
                        SRPLG_LOG_ERR("ietf_interfaces", "Error traversing node! ");
                        return sr::ErrorCode::OperationFailed;
                    };

                    // name of interface
                    std::string if_name = name_node.value().asTerm().valueStr().data();

                    libyang::DataNode tmp(change.node);
                    if (tmp.parent().has_value()) {
                        tmp = tmp.parent().value();
                    } else {
                        SRPLG_LOG_ERR("ietf_interfaces", "Failed to obtain parent node to prefix-length! ");
                        return sr::ErrorCode::OperationFailed;
                    }

                    std::string ip_path = tmp.path().append("/prefix-length");

                    std::optional<libyang::DataNode> ip_node = tmp.findPath(ip_path);

                    if (!ip_node.has_value()) {
                        SRPLG_LOG_ERR("ietf_interfaces", "Failed to find path to prefix-length! ");
                        return sr::ErrorCode::OperationFailed;
                    }

                    std::string ip_prefix = ip_node.value().asTerm().valueStr().data();

                    // uncomment for debug
                    // cout << "Created prefix " << ip_prefix << " Created if_name " << if_name << " Created addr " << val << endl;

                    try {
                        int ifindex = getIfindexFromName(if_name);
                        IPV4(ifindex).removeAddress(Address(val, std::stoi(ip_prefix)));
                    } catch (std::runtime_error& e) {
                        SRPLG_LOG_ERR("ietf_interfaces", "%s", e.what());
                        return sr::ErrorCode::OperationFailed;
                    }

                    break;
                }
                }
            }
            break;
        default:
            break;
        }

        return error;
    }

    sr::ErrorCode InterfaceModuleIPV4PrefixChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
        std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
    {

        sr::ErrorCode error = sr::ErrorCode::Ok;

        switch (event) {
        case sysrepo::Event::Change:
            for (sysrepo::Change change : session.getChanges(subXPath->data())) {
                switch (change.operation) {
                case sysrepo::ChangeOperation::Created:
                    // cout << "created subnode" << change.previousDefault << endl;
                    // int ifindex = getIfindexFromName(name);
                    // IPV4(ifindex).addAddress() break;
                case sysrepo::ChangeOperation::Modified:
                    // cout << "modified subnode " << endl;

                    // create a node from changed node
                    libyang::DataNode tmp(change.node);

                    // traverse till interface
                    while (std::string(tmp.schema().name().data()) != "interface") {

                        if (tmp.parent().has_value()) {
                            tmp = tmp.parent().value();
                        } else {
                            break;
                        }
                    }

                    // set path
                    std::string name_path = tmp.path().append("/name");

                    // find name value
                    std::optional<libyang::DataNode> name_node = tmp.findPath(name_path);
                    if (!name_node.has_value()) {
                        SRPLG_LOG_ERR("ietf_interfaces", "Error traversing node! ");
                        return sr::ErrorCode::OperationFailed;
                    };

                    std::string name = name_node.value().asTerm().valueStr().data();
                    std::string value = change.node.asTerm().valueStr().data();
                    std::string address = change.node.firstSibling().asTerm().valueStr().data();
                    std::string previous_value;
                    if (change.previousValue.has_value()) {
                        previous_value = change.previousValue.value().data();
                    } else {
                        SRPLG_LOG_ERR("ietf_interfaces", "Cannot valorize previous prefix-length value! ");
                        return sr::ErrorCode::OperationFailed;
                    }

                    try {
                        int ifindex = getIfindexFromName(name);

                        // no way of modifying prefix length
                        // only delete address with previous, and add new address

                        IPV4(ifindex).removeAddress(Address(address, std::stoi(previous_value)));
                        IPV4(ifindex).addAddress(Address(address, std::stoi(value)));

                    } catch (std::runtime_error& e) {
                        SRPLG_LOG_ERR("ietf-interfaces-plugin: ","Modify prefix length error: %s ", e.what());
                        return sr::ErrorCode::OperationFailed;
                    }

                    break;
                }
            }
            break;
        default:
            break;
        }

        return error;
    }
}
}
