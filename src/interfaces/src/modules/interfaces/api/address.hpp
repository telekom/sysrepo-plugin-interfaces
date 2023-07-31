#pragma once

#include "interface.hpp"
#include "cache.hpp"
#include <string>

// AF_INET && AF_INET6
#include <sys/socket.h>

class RouteAddressRef;
class NeighborRef;

class AddressRef {
public:
    friend class InterfaceRef; ///< Allow Interface class to use the private constructor.
    friend class RouteAddressRef; ///< Allow RouteAddressRef to use the private constructor.
    friend class NeighborRef; ///< Allow NeighborRef to use the private constructor.

    /**
     * @breif Convert the address to string.
     */
    std::string toString() const;

private:
    using NlAddr = struct nl_addr; ///< NL address type alias;
    using NlAddrDeleter = NlDeleter<NlAddr>; ///< Deleter type alias.
    using NlAddrPtr = std::unique_ptr<NlAddr, NlAddrDeleter>; ///< Unique pointer type alias.

    /**
     * @brief Private constructor accessible only to friend classes. Stores a reference to nl_addr for later access of address members.
     */
    AddressRef(struct nl_addr* addr);

    NlAddrPtr m_addr; ///< Addr reference.
};

enum class AddressFamily {
    V4 = AF_INET,
    V6 = AF_INET6,
    Other,
};

enum class AddressOrigin {
    Static,
    Dhcp,
    LinkLayer,
    Random,
};

/**
 * @brief Convert address origin to string.
 */
std::string addressOriginToString(AddressOrigin origin);

class RouteAddressRef {
public:
    using Self = RouteAddressRef;
    friend class CacheRef<Self>; ///< Allow Interface class to use the private constructor.

    /**
     * @brief Return the interface index of an address.
     */
    uint32_t getInterfaceIndex() const;

    /**
     * @brief Return the address family.
     */
    AddressFamily getFamily() const;

    /**
     * @brief Return the origin of the address.
     */
    AddressOrigin getOrigin() const;

    /**
     * @brief Get the IP address portion of the route address.
     */
    std::string getIPAddress() const;

    /**
     * @brief Get the prefix portion of the route address.
     */
    int getPrefix() const;

private:
    using RtnlAddr = struct rtnl_addr; ///< Rout NL address type alias;
    using RtnlAddrDeleter = NlDeleter<RtnlAddr>; ///< Deleter type alias.
    using RtnlAddrPtr = std::unique_ptr<RtnlAddr, RtnlAddrDeleter>; ///< Unique pointer type alias.

    /**
     * @brief Private constructor accessible only to friend classes. Stores a reference to rtnl_addr for later access of address members.
     */
    RouteAddressRef(struct rtnl_addr* addr);

    /**
     * @brief Private constructor accessible only to friend classes. Stores a reference to rtnl_addr for later access of address members.
     */
    RouteAddressRef(struct nl_object* addr);

    RtnlAddrPtr m_addr; ///< Addr reference.
};
