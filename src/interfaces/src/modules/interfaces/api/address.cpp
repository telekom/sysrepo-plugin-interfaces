#include "address.hpp"
#include "modules/interfaces/api/nl.hpp"
#include "netlink/addr.h"
#include "netlink/route/addr.h"
#include "sysrepo.h"
#include <linux/if_addr.h>
#include <stdexcept>

/**
 * @brief Convert address origin to string.
 */
std::string originToString(AddressOrigin origin)
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
 * @brief Private constructor accessible only to friend classes. Stores a reference to nl_addr for later access of address members.
 */
AddressRef::AddressRef(struct nl_addr* addr)
    : m_addr(addr, NlEmptyDeleter<NlAddr>)
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
RouteAddressRef::RouteAddressRef(struct rtnl_addr* addr)
    : m_addr(addr, NlEmptyDeleter<RtnlAddr>)
{
}

/**
 * @brief Private constructor accessible only to friend classes. Stores a reference to rtnl_addr for later access of address members.
 */
RouteAddressRef::RouteAddressRef(struct nl_object* addr)
    : m_addr(reinterpret_cast<RtnlAddr*>(addr), NlEmptyDeleter<RtnlAddr>)
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
 * @brief Get the IP address portion of the route address.
 */
std::string RouteAddressRef::getIPAddress() const
{
    auto local = AddressRef(rtnl_addr_get_local(m_addr.get()));
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
