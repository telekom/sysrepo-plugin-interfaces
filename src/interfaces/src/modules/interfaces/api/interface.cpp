#include "interface.hpp"
#include <memory>

/**
 * @brief Private constructor accessible only to netlink context. Stores a reference to a link for later access of link members.
 */
Interface::Interface(struct rtnl_link* link) { m_link = RtnlLinkPtr(link, NLEmptyDeleter<RtnlLink>); }

/**
 * @breif Wrapper function for rtnl_link_get_name().
 */
std::string Interface::getName() const { return std::string(rtnl_link_get_name(m_link.get())); }

/**
 * @breif Wrapper function for rtnl_link_get_group().
 */
std::uint32_t Interface::getGroup() const { return rtnl_link_get_group(m_link.get()); }

/**
 * @breif Wrapper function for rtnl_link_get_flags().
 */
std::uint32_t Interface::getFlags() const { return rtnl_link_get_flags(m_link.get()); }

/**
 * @breif Wrapper function for rtnl_link_get_familt().
 */
std::int32_t Interface::getFamily() const { return rtnl_link_get_family(m_link.get()); }

/**
 * @breif Wrapper function for rtnl_link_get_ifindex().
 */
std::int32_t Interface::getIndex() const { return rtnl_link_get_ifindex(m_link.get()); }

/**
 * @breif Wrapper function for rtnl_link_get_mtu().
 */
std::uint32_t Interface::getMTU() const { return rtnl_link_get_mtu(m_link.get()); }

/**
 * @breif Wrapper function for rtnl_link_get_master().
 */
std::int32_t Interface::getMaster() const { return rtnl_link_get_master(m_link.get()); }

/**
 * @breif Wrapper function for rtnl_link_get_operstate().
 */
std::uint8_t Interface::getOperationalStatus() const { return rtnl_link_get_operstate(m_link.get()); }

/**
 * @breif Wrapper function for rtnl_link_get_linkmode().
 */
std::uint8_t Interface::getLinkMode() const { return rtnl_link_get_linkmode(m_link.get()); }
