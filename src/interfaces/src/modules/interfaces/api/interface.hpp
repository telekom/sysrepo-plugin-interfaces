#pragma once

#include "nl.hpp"
#include <memory>

class Interface {
public:
    friend class NLContext; ///< Allow NLContext to use the private constructor.

    /**
     * @breif Wrapper function for rtnl_link_get_name().
     */
    std::string getName() const;

    /**
     * @breif Wrapper function for rtnl_link_get_group().
     */
    std::uint32_t getGroup() const;

    /**
     * @breif Wrapper function for rtnl_link_get_flags().
     */
    std::uint32_t getFlags() const;

    /**
     * @breif Wrapper function for rtnl_link_get_family().
     */
    std::int32_t getFamily() const;

    /**
     * @breif Wrapper function for rtnl_link_get_ifindex().
     */
    std::int32_t getIndex() const;

    /**
     * @breif Wrapper function for rtnl_link_get_mtu().
     */
    std::uint32_t getMTU() const;

    /**
     * @breif Wrapper function for rtnl_link_get_master().
     */
    std::int32_t getMaster() const;

    /**
     * @breif Wrapper function for rtnl_link_get_operstate().
     */
    std::uint8_t getOperationalStatus() const;

    /**
     * @breif Wrapper function for rtnl_link_get_linkmode().
     */
    std::uint8_t getLinkMode() const;

private:
    using RtnlLink = struct rtnl_link; ///< Route link type alias.
    using RtnlLinkDeleter = NLDeleter<RtnlLink>; ///< Deleter type alias.
    using RtnlLinkPtr = std::unique_ptr<RtnlLink, RtnlLinkDeleter>; ///< Unique pointer type alias.

    /**
     * @brief Private constructor accessible only to netlink context. Stores a reference to a link for later access of link members.
     */
    Interface(struct rtnl_link* link);

    RtnlLinkPtr m_link; ///< Link reference.
};
