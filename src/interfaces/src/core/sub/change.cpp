#include "change.hpp"
#include <iostream>
#include <optional>
#include <libyang-cpp/DataNode.hpp>
#include <interfaces/src/core/system/interface.hpp>
#include <sysrepo.h>

// just for debug- delete after

#define DEBUG(x) (std::cout << "DEBUG: " << x << std::endl)

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
    InterfaceModuleChangeCb::InterfaceModuleChangeCb(std::shared_ptr<ietf::ifc::ModuleChangeContext> ctx) { m_ctx = ctx; }
    InterfaceModuleNameChangeCb::InterfaceModuleNameChangeCb(std::shared_ptr<ietf::ifc::ModuleChangeContext> ctx) { m_ctx = ctx; }
    InterfaceModuleTypeChangeCb::InterfaceModuleTypeChangeCb(std::shared_ptr<ietf::ifc::ModuleChangeContext> ctx) { m_ctx = ctx; }
    InterfaceModuleIPV4EnableChangeCb::InterfaceModuleIPV4EnableChangeCb(std::shared_ptr<ietf::ifc::ModuleChangeContext> ctx) { m_ctx = ctx; }
    InterfaceModuleIPV4MtuChangeCb::InterfaceModuleIPV4MtuChangeCb(std::shared_ptr<ietf::ifc::ModuleChangeContext> ctx) { m_ctx = ctx; }
    InterfaceModuleIPV4AddressChangeCb::InterfaceModuleIPV4AddressChangeCb(std::shared_ptr<ietf::ifc::ModuleChangeContext> ctx) { m_ctx = ctx; }
    InterfaceModuleIPV4PrefixChangeCb::InterfaceModuleIPV4PrefixChangeCb(std::shared_ptr<ietf::ifc::ModuleChangeContext> ctx) { m_ctx = ctx; }
    InterfaceModuleIPV4NeighbourIpChangeCb::InterfaceModuleIPV4NeighbourIpChangeCb(std::shared_ptr<ietf::ifc::ModuleChangeContext> ctx)
    {
        m_ctx = ctx;
    }
    InterfaceModuleIPV4NeighbourLLAddressChangeCb::InterfaceModuleIPV4NeighbourLLAddressChangeCb(std::shared_ptr<ietf::ifc::ModuleChangeContext> ctx)
    {
        m_ctx = ctx;
    }
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
                    break;
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
                    break;
                case sysrepo::ChangeOperation::Modified: {

                    libyang::DataNode tmp(change.node.firstSibling());
                    std::string name = tmp.asTerm().valueStr().data();

                    std::string value = change.node.asTerm().valueStr().data();

                    int ifindex = getIfindexFromName(name);

                    if (ifindex == 0) {
                        SRPLG_LOG_ERR("ietf_interfaces", "non-existing ifindex at interface %s", name.c_str());
                        return sr::ErrorCode::OperationFailed;
                    } else {
                        try {
                            Interface(ifindex).setType(value);
                        } catch (std::runtime_error& e) {
                            SRPLG_LOG_ERR("ietf_interfaces", "Error changing interface type, reason:  %s", e.what());
                            return sr::ErrorCode::OperationFailed;
                        }
                    }
                    break;
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
                    // std::cout << "Created prefix " << ip_prefix << " Created if_name " << if_name << " Created addr " << val << std::endl;

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
                    bool if_has_changes = false;
                    for (auto&& i : session.getChanges("/ietf-interfaces:interfaces/interface")) {
                        if_has_changes = true;
                        break;
                    }

                    try {
                        int ifindex = getIfindexFromName(if_name);
                        if (!if_has_changes) {
                            // no point of deleting address since its allredy deleted
                            IPV4(ifindex).removeAddress(Address(val, std::stoi(ip_prefix)));
                        }

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
                    break;
                case sysrepo::ChangeOperation::Modified:

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
                        SRPLG_LOG_ERR("ietf-interfaces-plugin: ", "Modify prefix length error: %s ", e.what());
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

    sr::ErrorCode InterfaceModuleIPV4NeighbourIpChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
        std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
    {

        sr::ErrorCode error = sr::ErrorCode::Ok;

        switch (event) {
        case sysrepo::Event::Change:
            for (sysrepo::Change change : session.getChanges(subXPath->data())) {
                switch (change.operation) {
                case sysrepo::ChangeOperation::Created: {

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

                    libyang::DataNode lladdr_node(change.node);

                    // set the link layer addres by checking siblings
                    for (libyang::DataNode&& i : lladdr_node.siblings()) {
                        if (i.schema().name() == "link-layer-address") {
                            lladdr_node = i;
                            break;
                        }
                    }

                    // this means that link layer address node is not found
                    if (std::string(lladdr_node.schema().name().data()) != "link-layer-address") {
                        SRPLG_LOG_ERR("ietf_interfaces", "Error finding the value of link layer address! ");
                        return sr::ErrorCode::OperationFailed;
                    }

                    // so we got what we need, lets construct the variables
                    std::string neigh_addr = change.node.asTerm().valueStr().data();
                    std::string if_name = name_node.value().asTerm().valueStr().data();
                    std::string ll_addr = lladdr_node.asTerm().valueStr().data();
                    int ifindex = 0;

                    // add the neighbour
                    try {
                        usleep(3000);
                        ifindex = getIfindexFromName(if_name);
                        IPV4(ifindex).addNeighbour(Neighbour(neigh_addr, ll_addr));
                    } catch (std::runtime_error& e) {
                        SRPLG_LOG_ERR("ietf_interfaces", "Neighbor Add failed: %s", e.what());
                        return sr::ErrorCode::OperationFailed;
                    }

                    break;
                }
                case sysrepo::ChangeOperation::Modified:
                    // key node cannot be modified

                    break;
                case sysrepo::ChangeOperation::Deleted: {

                    // create a node from changed node
                    libyang::DataNode tmp(change.node);

                    // traverse till interface
                    while (tmp.schema().name() != "interface") {

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
                    std::string val = change.node.asTerm().valueStr().data();
                    int ifindex = 0;
                    // for the removal of neighbour, you just need the address

                    // check if interface node has changes, or only neighbour node
                    bool if_has_changes = false;
                    for (auto&& i : session.getChanges("/ietf-interfaces:interfaces/interface")) {
                        if_has_changes = true;
                        break;
                    }

                    try {
                        ifindex = getIfindexFromName(name);
                        if (!if_has_changes) {

                            IPV4(ifindex).removeNeighbor(val);
                        }

                    } catch (std::runtime_error& e) {
                        SRPLG_LOG_ERR("ietf_interfaces", "Failed to remove neighbour, reason: %s", e.what());
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

    sr::ErrorCode InterfaceModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
        std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
    {

        sr::ErrorCode error = sr::ErrorCode::Ok;

        switch (event) {
        case sysrepo::Event::Change:
            for (sysrepo::Change change : session.getChanges(subXPath->data())) {
                switch (change.operation) {
                case sysrepo::ChangeOperation::Created: {

                    std::string path = change.node.path().data();
                    std::optional<libyang::DataNode> node;

                    node = change.node.findPath(std::string(path + "/name"));
                    if (!node.has_value()) {
                        SRPLG_LOG_ERR("ietf_interfaces", "Cannot find value of node: name");
                        return sr::ErrorCode::OperationFailed;
                    }

                    std::string name = node.value().asTerm().valueStr().data();

                    node = change.node.findPath(std::string(path + "/type"));
                    if (!node.has_value()) {
                        SRPLG_LOG_ERR("ietf_interfaces", "Cannot find value of node: type");
                        return sr::ErrorCode::OperationFailed;
                    }
                    std::string type = node.value().asTerm().valueStr().data();

                    node = change.node.findPath(std::string(path + "/enabled"));
                    if (!node.has_value()) {
                        SRPLG_LOG_ERR("ietf_interfaces", "Cannot find value of node: enabled");
                        return sr::ErrorCode::OperationFailed;
                    }
                    auto enabled = node.value().asTerm().value();
                    bool if_enabled = std::get<bool>(enabled);
                    try {
                        Interface::create(name, type, if_enabled);
                    } catch (std::runtime_error& e) {
                        SRPLG_LOG_ERR("ietf_interfaces", "Cannot create interface, reason: %s", e.what());
                        return sr::ErrorCode::OperationFailed;
                    }

                    break;
                }
                case sysrepo::ChangeOperation::Modified:
                    break;
                case sysrepo::ChangeOperation::Deleted:

                    std::optional<libyang::DataNode> node = change.node.findPath(std::string(change.node.path() + "/name"));
                    if (!node.has_value()) {
                        SRPLG_LOG_ERR("ietf_interfaces", "Cannot find value of node: name");
                        return sr::ErrorCode::OperationFailed;
                    }

                    std::string name = node.value().asTerm().valueStr().data();

                    try {
                        int ifindex = getIfindexFromName(name);
                        Interface(ifindex).remove();

                    } catch (std::runtime_error& e) {
                        SRPLG_LOG_ERR("ietf_interfaces", "Cannot remove interface, reason: %s", e.what());
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

    sr::ErrorCode InterfaceModuleIPV4NeighbourLLAddressChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
        std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
    {

        sr::ErrorCode error = sr::ErrorCode::Ok;

        switch (event) {
        case sysrepo::Event::Change:
            for (sysrepo::Change change : session.getChanges(subXPath->data())) {
                switch (change.operation) {
                case sysrepo::ChangeOperation::Created:
                    break;
                case sysrepo::ChangeOperation::Modified: {

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

                    std::string if_name = name_node.value().asTerm().valueStr().data();

                    libyang::DataNode ip_node = change.node.firstSibling();

                    std::string addr = ip_node.asTerm().valueStr().data();
                    std::string ll_addr = change.node.asTerm().valueStr().data();
                    try {

                        int ifindex = getIfindexFromName(if_name);
                        IPV4(ifindex).modifyNeighbourLinkLayer(Neighbour(addr, ll_addr));

                    } catch (std::runtime_error& e) {
                        SRPLG_LOG_ERR("ietf_interfaces", "Error modifying ll address! %s ", e.what());
                        return sr::ErrorCode::OperationFailed;
                    }
                    break;
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

}
}
