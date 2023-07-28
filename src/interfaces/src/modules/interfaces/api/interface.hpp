#pragma once

#include "nl.hpp"
#include "cache.hpp"
#include <memory>

class Address;

class Interface {
public:
    friend class NlContext; ///< Allow NlContext to use the private constructor.
    friend class Cache<Interface>; ///< Allow Cache to use the private constructor.

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
    Address getAddress() const;

    /**
     * @brief Returns the speed of the interface.
     */
    uint64_t getSpeed() const;

private:
    using RtnlLink = struct rtnl_link; ///< Route link type alias.
    using RtnlLinkDeleter = NlDeleter<RtnlLink>; ///< Deleter type alias.
    using RtnlLinkPtr = std::unique_ptr<RtnlLink, RtnlLinkDeleter>; ///< Unique pointer type alias.

    /**
     * @brief Private constructor accessible only to netlink context. Stores a reference to a link for later access of link members.
     */
    Interface(struct rtnl_link* link);

    /**
     * @brief Private constructor accessible only to netlink context. Stores a reference to a link for later access of link members.
     */
    Interface(struct nl_object* link);

    RtnlLinkPtr m_link; ///< Link reference.
};
