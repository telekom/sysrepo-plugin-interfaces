#pragma once

#include <optional>
#include <string>

#include <libyang-cpp/DataNode.hpp>

namespace ly = libyang;

namespace ietf::rt {
namespace yang::tree {
/**
 * sysrepo-plugin-generator: Generated yang tree function for path /ietf-routing:routing
 *
 * @return Returns the first created node. If no nodes were created, returns std::nullopt.
 *
 */
std::optional<ly::DataNode> createRoutingContainer();

/**
 * sysrepo-plugin-generator: Generated yang tree function for path /ietf-routing:routing/ribs
 *
 * @param routing Parent of the node being created.
 *
 * @return Returns the first created node. If no nodes were created, returns std::nullopt.
 *
 */
std::optional<ly::DataNode> createRibsContainer(ly::DataNode &routing);

/**
 * sysrepo-plugin-generator: Generated yang tree function for path /ietf-routing:routing/ribs/rib[name='%s']
 *
 * @param ribs Parent of the node being created.
 * @param name Key value for key name.
 *
 * @return Returns the first created node. If no nodes were created, returns std::nullopt.
 *
 */
std::optional<ly::DataNode> createRibList(
        ly::DataNode &ribs
            , const std::string& name
);

/**
 * sysrepo-plugin-generator: Generated yang tree function for path /ietf-routing:routing/control-plane-protocols
 *
 * @param routing Parent of the node being created.
 *
 * @return Returns the first created node. If no nodes were created, returns std::nullopt.
 *
 */
std::optional<ly::DataNode> createControlPlaneProtocolsContainer(ly::DataNode &routing);

/**
 * sysrepo-plugin-generator: Generated yang tree function for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']
 *
 * @param control_plane_protocols Parent of the node being created.
 * @param type Key value for key type.
 * @param name Key value for key name.
 *
 * @return Returns the first created node. If no nodes were created, returns std::nullopt.
 *
 */
std::optional<ly::DataNode> createControlPlaneProtocolList(
        ly::DataNode &control_plane_protocols
            , const std::string& type
            , const std::string& name
);

/**
 * sysrepo-plugin-generator: Generated yang tree function for path /ietf-routing:routing/interfaces
 *
 * @param routing Parent of the node being created.
 *
 * @return Returns the first created node. If no nodes were created, returns std::nullopt.
 *
 */
std::optional<ly::DataNode> createInterfacesContainer(ly::DataNode &routing);

/**
 * sysrepo-plugin-generator: Generated yang tree function for path /ietf-routing:routing/interfaces/interface
 *
 * @param interfaces Parent of the node being created.
 * @param interface Value of the leaf-list element.
 *
 * @return Returns the first created node. If no nodes were created, returns std::nullopt.
 *
 */
std::optional<ly::DataNode> createInterfaceLeafList(
    ly::DataNode &interfaces, 
    const std::string& interface
);

/**
 * sysrepo-plugin-generator: Generated yang tree function for path /ietf-routing:routing/router-id
 *
 * @param routing Parent of the node being created.
 * @param router_id Value of the leaf element.
 *
 * @return Returns the first created node. If no nodes were created, returns std::nullopt.
 *
 */
std::optional<ly::DataNode> createRouterIdLeaf(
    ly::DataNode &routing, 
    const std::string& router_id
);

}
}