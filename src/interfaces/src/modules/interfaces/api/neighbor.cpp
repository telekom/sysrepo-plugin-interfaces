#include "neighbor.hpp"
#include "netlink/route/neighbour.h"
#include <linux/neighbour.h>

/**
 * @brief Convert neighbor origin to string.
 */
std::string neighborOriginToString(NeighborOrigin origin)
{
    auto str = std::string();

    switch (origin) {
    case NeighborOrigin::Other:
        str = "other";
        break;
    case NeighborOrigin::Static:
        str = "static";
        break;
    case NeighborOrigin::Dynamic:
        str = "dynamic";
        break;
    }

    return str;
}

/**
 * @brief Convert neighbor state to string.
 */
std::string neighborStateToString(NeighborState state)
{
    auto str = std::string();

    switch (state) {
    case NeighborState::Incomplete:
        str = "incomplete";
        break;
    case NeighborState::Reachable:
        str = "reachable";
        break;
    case NeighborState::Stale:
        str = "stale";
        break;
    case NeighborState::Delay:
        str = "delay";
        break;
    case NeighborState::Probe:
        str = "probe";
        break;
    }

    return str;
}

/**
 * @brief Private constructor accessible only to friend classes. Stores a reference to rtnl_neigh for later access of address members.
 */
NeighborRef::NeighborRef(struct rtnl_neigh* neigh, struct nl_sock* socket)
    : m_neigh(neigh, NlEmptyDeleter<RtnlNeigh>)
    , m_socket(socket, NlEmptyDeleter<struct nl_sock>)
{
}

/**
 * @brief Private constructor accessible only to friend classes. Stores a reference to rtnl_neigh for later access of address members.
 */
NeighborRef::NeighborRef(struct nl_object* neigh, nl_sock* socket)
    : m_neigh(reinterpret_cast<RtnlNeigh*>(neigh), NlEmptyDeleter<RtnlNeigh>)
    , m_socket(reinterpret_cast<struct nl_sock*>(socket), NlEmptyDeleter<nl_sock>)
{
}

/**
 * @brief Return the interface index of a neighbor.
 */
uint32_t NeighborRef::getInterfaceIndex() const { return rtnl_neigh_get_ifindex(m_neigh.get()); }

/**
 * @brief Get the address family of a neighbor.
 */
AddressFamily NeighborRef::getAddressFamily() const
{
    auto family = rtnl_neigh_get_family(m_neigh.get());

    if (family == AF_INET) {
        return AddressFamily::V4;
    } else if (family == AF_INET6) {
        return AddressFamily::V6;
    }

    return AddressFamily::Other;
}

/**
 * @brief Get the origin of a neighbor.
 */
NeighborOrigin NeighborRef::getOrigin() const
{
    auto flags = rtnl_neigh_get_flags(m_neigh.get());

    if ((flags & NTF_ROUTER) > 0) {
        return NeighborOrigin::Dynamic;
    }

    return NeighborOrigin::Static;
}

/**
 * @brief Get the state of a neighbor.
 */
NeighborState NeighborRef::getState() const { return (NeighborState)rtnl_neigh_get_state(m_neigh.get()); }

/**
 * @brief Get the destination IP address of a neighbor.
 */
std::string NeighborRef::getDestinationIP() const
{
    auto dst = AddressRef(rtnl_neigh_get_dst(m_neigh.get()),m_socket.get());
    auto str = dst.toString();
    auto slash_pos = str.find('/');
    auto ip_address = str;

    if (slash_pos != std::string::npos) {
        ip_address = str.substr(0, slash_pos);
    }

    return ip_address;
}

/**
 * @brief Get the link-layer address of a neighbor.
 */
std::string NeighborRef::getLinkLayerAddress() const
{
    auto ll_addr = AddressRef(rtnl_neigh_get_lladdr(m_neigh.get()),m_socket.get());
    return ll_addr.toString();
}

/**
 * @brief Returns true if the neighbor acts as a router.
 */
bool NeighborRef::isRouter() const { return (rtnl_neigh_get_flags(m_neigh.get()) & NTF_ROUTER) > 0; }
