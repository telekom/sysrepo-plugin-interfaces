#include <libyang-cpp/DataNode.hpp>
#include <libyang-cpp/Collection.hpp>
#include <unordered_map>
#include "IPV6.hpp"
#include <iostream>

namespace changeMethods {

void createdIPV6AddressNode(const libyang::DataNode& change_node, int ifindex)
{

    std::string ip_addr;
    int prefix_len = -1;

    std::unordered_map<std::string, std::string> map;
    for (libyang::DataNode&& nd : change_node.childrenDfs()) {
        std::string key = nd.schema().name().data();
        std::string value;
        if (nd.isTerm()) {
            value = nd.asTerm().valueStr().data();
        } else {
            value = "NULL";
        };

        map.insert(std::make_pair(key, value));
    };

    // first check if key - value exists
    if (!(map.find("ip") != map.end())) {
        throw std::runtime_error("Cannot find ip node!");
    };

    if (!(map.find("prefix-length") != map.end())) {
        throw std::runtime_error("Cannot find prefix-length node node!");
    };

    ip_addr = map["ip"];
    prefix_len = std::stoi(map["prefix-length"]);

    IPV6(ifindex).addAddress(Address(ip_addr, prefix_len));
}

void deletedIPV6AddressNode(const libyang::DataNode& change_node, int ifindex)
{
    std::string ip_path = change_node.path().append("/ip");
    std::string prefix_path = change_node.path().append("/prefix-length");

    std::optional<libyang::DataNode> ip_node = change_node.findPath(ip_path);

    std::optional<libyang::DataNode> prefix_node = change_node.findPath(prefix_path);

    if (!ip_node.has_value()) {
        throw std::runtime_error("Cannot find address/ip node");
    }
    if (!prefix_node.has_value()) {
        throw std::runtime_error("Cannot find prefix-lenght node");
    }

    std::string ip_address = ip_node.value().asTerm().valueStr().data();
    int prefix_length = std::stoi(prefix_node.value().asTerm().valueStr().data());

    IPV6(ifindex).removeAddress(Address(ip_address, prefix_length));
};

void deletedIPV6NeighborNode(const libyang::DataNode& change_node, int ifindex)
{

    std::string ip_path = change_node.path().append("/ip");

    std::optional<libyang::DataNode> ip_node = change_node.findPath(ip_path);

    if (!ip_node.has_value()) {
        throw std::runtime_error("Cannot find neighbor/ip node");
    }

    std::string ip_address = ip_node.value().asTerm().valueStr().data();

    IPV6(ifindex).removeNeighbor(ip_address);
}

void createdIPV6NeighborNode(const libyang::DataNode& change_node, int ifindex)
{
    std::string ip_addr;
    std::string link_layer_addr;

    std::unordered_map<std::string, std::string> map;
    for (libyang::DataNode&& nd : change_node.childrenDfs()) {
        std::string key = nd.schema().name().data();
        std::string value;
        if (nd.isTerm()) {
            value = nd.asTerm().valueStr().data();
        } else {
            value = "NULL";
        };

        map.insert(std::make_pair(key, value));
    };

    // first check if key - value exists
    if (!(map.find("ip") != map.end())) {
        throw std::runtime_error("Cannot find ip node!");
    };

    if (!(map.find("link-layer-address") != map.end())) {
        throw std::runtime_error("Cannot find link-layer-address node!");
    };

    ip_addr = map["ip"];
    link_layer_addr = map["link-layer-address"];

    IPV6(ifindex).addNeighbor(Neighbour(ip_addr, link_layer_addr));
}

void modifiedPrefixLength(const libyang::DataNode& change_node, int ifindex, int previous_prefix_len)
{
    // since there is no way to modify the address prefix length
    //, we have to delete the address and add with new prefix length

    std::string ip_addr;
    std::string new_prefix_len = change_node.asTerm().valueStr().data();

    libyang::DataNode tmp(change_node.firstSibling());

    for (libyang::DataNode&& i : tmp.siblings()) {
        if (i.schema().name() == "ip") {
            ip_addr = i.asTerm().valueStr().data();
            break;
        }
    }

    // check if ip_addr has been assigned
    if (ip_addr.empty()) {
        throw std::runtime_error("Failed to find ip node!");
    };

    IPV6(ifindex).removeAddress(Address(ip_addr, previous_prefix_len));
    IPV6(ifindex).addAddress(Address(ip_addr, std::stoi(new_prefix_len)));
};

void modifiedLinkLayerAddr(const libyang::DataNode& change_node, int ifindex)
{

    std::string ll_addr = change_node.asTerm().valueStr().data();
    std::string ip_addr;

    libyang::DataNode tmp(change_node.firstSibling());

    for (libyang::DataNode&& i : tmp.siblings()) {
        if (i.schema().name() == "ip") {
            ip_addr = i.asTerm().valueStr().data();
            break;
        }
    }

    // check if ip_addr has been assigned
    if (ip_addr.empty()) {
        throw std::runtime_error("Failed to find ip node!");
    };

    IPV6(ifindex).modifyNetworkLinkLayer(Neighbour(ip_addr, ll_addr));
};

};
