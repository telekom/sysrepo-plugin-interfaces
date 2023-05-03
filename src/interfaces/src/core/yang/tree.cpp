#include "tree.hpp"

namespace ietf::ifc {
namespace yang::tree {
    /**
     * sysrepo-plugin-generator: Generated yang tree function for path /ietf-interfaces:interfaces
     *
     * @return Returns the first created node. If no nodes were created, returns std::nullopt.
     *
     */
    std::optional<ly::DataNode> createInterfacesContainer()
    {
        std::optional<ly::DataNode> new_node = std::nullopt;
        return new_node;
    }

    /**
     * sysrepo-plugin-generator: Generated yang tree function for path /ietf-interfaces:interfaces/interface[name='%s']
     *
     * @param interfaces Parent of the node being created.
     * @param name Key value for key name.
     *
     * @return Returns the first created node. If no nodes were created, returns std::nullopt.
     *
     */
    std::optional<ly::DataNode> createInterfaceList(ly::DataNode& interfaces, const std::string& name)
    {
        std::optional<ly::DataNode> new_node = std::nullopt;
        return new_node;
    }

}
}