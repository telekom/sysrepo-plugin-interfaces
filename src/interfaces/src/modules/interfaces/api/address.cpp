#include "address.hpp"
#include "netlink/addr.h"
#include "netlink/route/addr.h"
#include <linux/if_addr.h>
#include <stdexcept>

/**
 * @brief Convert address origin to string.
 */
std::string addressOriginToString(AddressOrigin origin)
{
    auto str = std::string();

    switch (origin) {
    case AddressOrigin::Static:
        str = "static";
        break;
    case AddressOrigin::Dhcp:
        str = "dhcp";
        break;
    case AddressOrigin::LinkLayer:
        str = "link-layer";
        break;
    case AddressOrigin::Random:
        str = "random";
        break;
    }

    return str;
}

/**
 * @brief Convert address status to string.
 */
std::string addressStatusToString(AddressStatus status)
{
    auto str = std::string();

    switch (status) {
    case AddressStatus::Preferred:
        str = "preferred";
        break;
    case AddressStatus::Deprecated:
        str = "deprecated";
        break;
    case AddressStatus::Invalid:
        str = "invalid";
        break;
    case AddressStatus::Inaccessible:
        str = "inaccessible";
        break;
    case AddressStatus::Unknown:
        str = "unknown";
        break;
    case AddressStatus::Tentative:
        str = "tentative";
        break;
    case AddressStatus::Duplicate:
        str = "duplicate";
        break;
    case AddressStatus::Optimistic:
        str = "optimistic";
        break;
        break;
    }

    return str;
}

/**
 * @brief Private constructor accessible only to friend classes. Stores a reference to nl_addr for later access of address members.
 */
AddressRef::AddressRef(struct nl_addr* addr, struct nl_sock* socket)
    : m_addr(addr, NlEmptyDeleter<NlAddr>)
    , m_socket(socket, NlEmptyDeleter<struct nl_sock>)
{
}

/**
 * @breif Convert the address to string.
 */
std::string AddressRef::toString() const
{
    char buffer[100] = { 0 };
    void* error = 0;

    error = nl_addr2str(m_addr.get(), buffer, sizeof(buffer));
    if (error == nullptr) {
        throw std::runtime_error("Unable to convert address to text format");
    }

    return std::string(buffer);
}

/**
 * @brief Private constructor accessible only to friend classes. Stores a reference to rtnl_addr for later access of address members.
 */
RouteAddressRef::RouteAddressRef(struct rtnl_addr* addr, struct nl_sock* socket)
    : m_addr(addr, NlEmptyDeleter<RtnlAddr>)
    , m_socket(socket, NlEmptyDeleter<struct nl_sock>)
{
}

/**
 * @brief Private constructor accessible only to friend classes. Stores a reference to rtnl_addr for later access of address members.
 */
RouteAddressRef::RouteAddressRef(struct nl_object* addr, struct nl_sock* socket)
    : m_addr(reinterpret_cast<RtnlAddr*>(addr), NlEmptyDeleter<RtnlAddr>)
    , m_socket(reinterpret_cast<struct nl_sock*>(socket), NlEmptyDeleter<struct nl_sock>)
{
}

/**
 * @brief Return the interface index of an address.
 */
uint32_t RouteAddressRef::getInterfaceIndex() const { return rtnl_addr_get_ifindex(m_addr.get()); }

/**
 * @brief Return the address family.
 */
AddressFamily RouteAddressRef::getFamily() const
{
    auto family = rtnl_addr_get_family(m_addr.get());

    if (family == AF_INET) {
        return AddressFamily::V4;
    } else if (family == AF_INET6) {
        return AddressFamily::V6;
    }
    return AddressFamily::Other;
}

/**
 * @brief Return the origin of the address.
 */
AddressOrigin RouteAddressRef::getOrigin() const
{
    auto flags = rtnl_addr_get_flags(m_addr.get());

    if ((flags & IFA_F_PERMANENT) > 0) {
        return AddressOrigin::Static;
    }

    return AddressOrigin::Dhcp;
}

/**
 * @brief Return the status of the address.
 */
AddressStatus RouteAddressRef::getStatus() const { return AddressStatus::Preferred; }

/**
 * @brief Get the IP address portion of the route address.
 */
std::string RouteAddressRef::getIPAddress() const
{
    auto local = AddressRef(rtnl_addr_get_local(m_addr.get()), m_socket.get());
    auto str = local.toString();
    auto slash_pos = str.find('/');
    auto ip_address = str;

    if (slash_pos != std::string::npos) {
        ip_address = str.substr(0, slash_pos);
    }

    return ip_address;
}

/**
 * @brief Get the prefix portion of the route address.
 */
int RouteAddressRef::getPrefix() const { return rtnl_addr_get_prefixlen(m_addr.get()); }

/**
 * @brief Get the prefix portion of the route address.
 */
#include <iostream>
void RouteAddressRef::setPrefix(int prefix_len) const
{
    rtnl_addr* address = m_addr.get();
    int err = 0;

    err = rtnl_addr_delete(m_socket.get(), address, 0);

    if (err < 0)
        throw std::runtime_error(nl_geterror(err));

    rtnl_addr_set_prefixlen(address, prefix_len);

    err = rtnl_addr_add(m_socket.get(), address, 0);
    if (err < 0)
        throw std::runtime_error(nl_geterror(err));
}
