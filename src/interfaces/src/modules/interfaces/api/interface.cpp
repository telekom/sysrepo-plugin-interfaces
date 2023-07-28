#include "interface.hpp"
#include "address.hpp"
#include <netlink/route/link.h>
#include <netlink/route/qdisc.h>
#include <netlink/route/tc.h>
#include <memory>

/**
 * @brief Private constructor accessible only to netlink context. Stores a reference to a link for later access of link members.
 */
Interface::Interface(struct rtnl_link* link) { m_link = RtnlLinkPtr(link, NlEmptyDeleter<RtnlLink>); }

/**
 * @brief Private constructor accessible only to netlink context. Stores a reference to a link for later access of link members.
 */
Interface::Interface(struct nl_object* link) { m_link = RtnlLinkPtr(reinterpret_cast<RtnlLink*>(link), NlEmptyDeleter<RtnlLink>); }

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

/**
 * @brief Returns address of the interface.
 */
Address Interface::getAddress() const { return Address(rtnl_link_get_addr(m_link.get())); }

/**
 * @brief Returns the speed of the interface.
 */
uint64_t Interface::getSpeed() const
{
    struct rtnl_qdisc* qdisc = NULL;
    struct rtnl_tc* tc = NULL;
    uint64_t speed = 0;

    // setup traffic control
    qdisc = rtnl_qdisc_alloc();
    tc = TC_CAST(qdisc);
    rtnl_tc_set_link(tc, m_link.get());

    // get speed
    speed = rtnl_tc_get_stat(tc, RTNL_TC_RATE_BPS);

    // free memory
    rtnl_qdisc_put(qdisc);

    return speed;
}
