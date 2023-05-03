#pragma once

#include <optional>
#include <string>

#include <libyang-cpp/DataNode.hpp>

namespace ly = libyang;

namespace ietf::ifc {
namespace yang::tree {
    /**
     * sysrepo-plugin-generator: Generated yang tree function for path /ietf-interfaces:interfaces
     *
     * @return Returns the first created node. If no nodes were created, returns std::nullopt.
     *
     */
    std::optional<ly::DataNode> createInterfacesContainer();

    /**
     * sysrepo-plugin-generator: Generated yang tree function for path /ietf-interfaces:interfaces/interface[name='%s']
     *
     * @param interfaces Parent of the node being created.
     * @param name Key value for key name.
     *
     * @return Returns the first created node. If no nodes were created, returns std::nullopt.
     *
     */
    std::optional<ly::DataNode> createInterfaceList(ly::DataNode& interfaces, const std::string& name);

}
}