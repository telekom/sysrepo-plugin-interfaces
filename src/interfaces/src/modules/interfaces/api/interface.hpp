#pragma once

#include "nl.hpp"
#include "cache.hpp"
#include <cstdint>
#include <memory>
#include <unordered_map>

class AddressRef;

/**
 * @brief Interface operational statistics container data structure.
 */
struct InterfaceStats {
    uint64_t InOctets;
    uint64_t InUnicastPkts;
    uint64_t InBroadcastPkts;
    uint64_t InMulticastPkts;
    uint32_t InDiscards;
    uint32_t InErrors;
    uint32_t InUnknownProtos;
    uint64_t OutOctets;
    uint64_t OutUnicastPkts;
    uint64_t OutBroadcastPkts;
    uint64_t OutMulticastPkts;
    uint32_t OutDiscards;
    uint32_t OutErrors;
};

const std::unordered_map<std::string, std::string> InterfaceTypes
    = { { "iana-if-type:ethernetCsmacd", "veth" }, 
        { "iana-if-type:softwareLoopback", "vcan" }, 
        //{ "iana-if-type:l2vlan", "vlan" },
        { "iana-if-type:other", "dummy" }, 
        { "iana-if-type:bridge", "bridge" } };

class InterfaceRef {
public:
    friend class NlContext; ///< Allow NlContext to use the private constructor.
    friend class CacheRef<InterfaceRef>; ///< Allow Cache to use the private constructor.

    /**
     * @brief Wrapper function for rtnl_link_get_name().
     */
    std::string getName() const;

    /**
     * @brief Wrapper function for rtnl_link_get_group().
     */
    std::uint32_t getGroup() const;

    /**
     * @brief Wrapper function for rtnl_link_get_flags().
     */
    std::uint32_t getFlags() const;

    /**
     * @brief Wrapper function for rtnl_link_get_family().
     */
    std::int32_t getFamily() const;

    /**
     * @brief Wrapper function for rtnl_link_get_ifindex().
     */
    std::int32_t getIndex() const;

    /**
     * @brief Wrapper function for rtnl_link_get_mtu().
     */
    std::uint32_t getMTU() const;

    /**
     * @brief Wrapper function for rtnl_link_get_master().
     */
    std::int32_t getMaster() const;

    /**
     * @brief Wrapper function for rtnl_link_get_operstate().
     */
    std::uint8_t getOperationalStatus() const;

    /**
     * @brief Wrapper function for rtnl_link_get_linkmode().
     */
    std::uint8_t getLinkMode() const;

    /**
     * @brief Returns address of the interface.
     */
    AddressRef getAddress() const;

    /**
     * @brief Returns the speed of the interface.
     */
    uint64_t getSpeed() const;

    /**
     * @brief Get interface statistics.
     */
    InterfaceStats getStatistics() const;

    /**
     * @brief Enable and disable an interface.
     */
    void setEnabled(bool enabled);

    /**
     * @brief Changes the MTU of an interface.
     */
    void setMtu(uint16_t mtu);

    /**
     * @brief Enable/Dissable forwarding of an interface.
     */
    void setForwarding(bool enabled, AddressFamily fam);

private:
    using RtnlLink = struct rtnl_link; ///< Route link type alias.
    using RtnlLinkDeleter = NlDeleter<RtnlLink>; ///< Deleter type alias.
    using RtnlLinkPtr = std::unique_ptr<RtnlLink, RtnlLinkDeleter>; ///< Unique pointer type alias.
    using NlSocketPtr = std::unique_ptr<struct nl_sock, NlDeleter<struct nl_sock>>; // Socket type alias.

    /**
     * @brief Private constructor accessible only to netlink context. Stores a reference to a link for later access of link members.
     */
    InterfaceRef(struct rtnl_link* link, struct nl_sock* socket);

    /**
     * @brief Private constructor accessible only to netlink context. Stores a reference to a link for later access of link members.
     */
    InterfaceRef(struct nl_object* link, struct nl_sock* socket);

    RtnlLinkPtr m_link; ///< Link reference.
    NlSocketPtr m_socket; /// socket reference.
};
