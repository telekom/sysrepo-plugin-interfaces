#include "interface.hpp"
#include "address.hpp"
#include <netlink/route/link.h>
#include <netlink/route/qdisc.h>
#include <netlink/route/tc.h>
#include <memory>
#include <stdexcept>
#include <linux/if.h>

/**
 * @brief Private constructor accessible only to netlink context. Stores a reference to a link for later access of link members.
 */
InterfaceRef::InterfaceRef(struct rtnl_link* link, struct nl_sock* socket)
{
    m_link = RtnlLinkPtr(link, NlEmptyDeleter<RtnlLink>), m_socket = NlSocketPtr(socket, NlEmptyDeleter<struct nl_sock>);
}

/**
 * @brief Private constructor accessible only to netlink context. Stores a reference to a link for later access of link members.
 */
InterfaceRef::InterfaceRef(struct nl_object* link, struct nl_sock* socket)
{
    m_link = RtnlLinkPtr(reinterpret_cast<RtnlLink*>(link), NlEmptyDeleter<RtnlLink>),
    m_socket = NlSocketPtr(reinterpret_cast<struct nl_sock*>(socket), NlEmptyDeleter<struct nl_sock>);
};

/**
 * @breif Wrapper function for rtnl_link_get_name().
 */
std::string InterfaceRef::getName() const { return std::string(rtnl_link_get_name(m_link.get())); }

/**
 * @breif Wrapper function for rtnl_link_get_group().
 */
std::uint32_t InterfaceRef::getGroup() const { return rtnl_link_get_group(m_link.get()); }

/**
 * @breif Wrapper function for rtnl_link_get_flags().
 */
std::uint32_t InterfaceRef::getFlags() const { return rtnl_link_get_flags(m_link.get()); }

/**
 * @breif Wrapper function for rtnl_link_get_familt().
 */
std::int32_t InterfaceRef::getFamily() const { return rtnl_link_get_family(m_link.get()); }

/**
 * @breif Wrapper function for rtnl_link_get_ifindex().
 */
std::int32_t InterfaceRef::getIndex() const { return rtnl_link_get_ifindex(m_link.get()); }

/**
 * @breif Wrapper function for rtnl_link_get_mtu().
 */
std::uint32_t InterfaceRef::getMTU() const { return rtnl_link_get_mtu(m_link.get()); }

/**
 * @breif Wrapper function for rtnl_link_get_master().
 */
std::int32_t InterfaceRef::getMaster() const { return rtnl_link_get_master(m_link.get()); }

/**
 * @breif Wrapper function for rtnl_link_get_operstate().
 */
std::uint8_t InterfaceRef::getOperationalStatus() const { return rtnl_link_get_operstate(m_link.get()); }

/**
 * @breif Wrapper function for rtnl_link_get_linkmode().
 */
std::uint8_t InterfaceRef::getLinkMode() const { return rtnl_link_get_linkmode(m_link.get()); }

/**
 * @brief Returns address of the interface.
 */
AddressRef InterfaceRef::getAddress() const { return AddressRef(rtnl_link_get_addr(m_link.get()), m_socket.get()); }

/**
 * @brief Returns the speed of the interface.
 */
uint64_t InterfaceRef::getSpeed() const
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
/**
 * @brief Get interface statistics.
 */
InterfaceStats InterfaceRef::getStatistics() const
{
    RtnlLink* link = m_link.get();

    // get discontinuity-time

    // get input data
    const auto in_octets = rtnl_link_get_stat(link, RTNL_LINK_RX_BYTES);
    const auto in_pkts = rtnl_link_get_stat(link, RTNL_LINK_RX_PACKETS);
    const auto in_broadcast_pkts = rtnl_link_get_stat(link, RTNL_LINK_IP6_INBCASTPKTS);
    const auto in_multicast_pkts = rtnl_link_get_stat(link, RTNL_LINK_IP6_INMCASTPKTS);
    const auto in_unicast_pkts = in_pkts - in_broadcast_pkts - in_multicast_pkts;
    const auto in_discards = (uint32_t)rtnl_link_get_stat(link, RTNL_LINK_RX_DROPPED);
    const auto in_errors = (uint32_t)rtnl_link_get_stat(link, RTNL_LINK_RX_ERRORS);
    const auto in_unknown_protos = (uint32_t)rtnl_link_get_stat(link, RTNL_LINK_IP6_INUNKNOWNPROTOS);

    // get output data
    const auto out_octets = rtnl_link_get_stat(link, RTNL_LINK_TX_BYTES);
    const auto out_pkts = rtnl_link_get_stat(link, RTNL_LINK_TX_PACKETS);
    const auto out_broadcast_pkts = rtnl_link_get_stat(link, RTNL_LINK_IP6_OUTBCASTPKTS);
    const auto out_multicast_pkts = rtnl_link_get_stat(link, RTNL_LINK_IP6_OUTMCASTPKTS);
    const auto out_unicast_pkts = out_pkts - out_broadcast_pkts - out_multicast_pkts;
    const auto out_discards = (uint32_t)rtnl_link_get_stat(link, RTNL_LINK_TX_DROPPED);
    const auto out_errors = (uint32_t)rtnl_link_get_stat(link, RTNL_LINK_TX_DROPPED);

    return InterfaceStats {
        .InOctets = in_octets,
        .InUnicastPkts = in_unicast_pkts,
        .InBroadcastPkts = in_broadcast_pkts,
        .InMulticastPkts = in_multicast_pkts,
        .InDiscards = in_discards,
        .InErrors = in_errors,
        .InUnknownProtos = in_unknown_protos,
        .OutOctets = out_octets,
        .OutUnicastPkts = out_unicast_pkts,
        .OutBroadcastPkts = out_broadcast_pkts,
        .OutMulticastPkts = out_multicast_pkts,
        .OutDiscards = out_discards,
        .OutErrors = out_errors,
    };
}

/**
 * @brief Enable and disable an interface.
 */
void InterfaceRef::setEnabled(bool enabled)
{

    RtnlLink* current_link = m_link.get();

    // allocate change link
    RtnlLink* change_link = rtnl_link_alloc();

    // set enabled flags to link
    enabled ? rtnl_link_set_flags(change_link, IFF_UP) : rtnl_link_unset_flags(change_link, IFF_UP);

    // change link
    int err = rtnl_link_change(m_socket.get(), current_link, change_link, 0);

    if (err < 0) {
        rtnl_link_put(change_link);
        throw std::runtime_error(std::string(nl_geterror(err)));
    }

    // deallocate change link
    rtnl_link_put(change_link);
}

/**
 * @brief Changes the MTU of an interface.
 */
void InterfaceRef::setMtu(uint16_t mtu)
{

    RtnlLink* current_link = m_link.get();

    // allocate change link
    RtnlLink* change_link = rtnl_link_alloc();

    // set mtu
    rtnl_link_set_mtu(change_link, mtu);

    // change link
    int err = rtnl_link_change(m_socket.get(), current_link, change_link, 0);

    if (err < 0) {
        rtnl_link_put(change_link);
        throw std::runtime_error(std::string(nl_geterror(err)));
    }

    // deallocate change link
    rtnl_link_put(change_link);
}


