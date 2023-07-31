#pragma once

#include "cache.hpp"
#include "address.hpp"
#include <netlink/route/neighbour.h>
#include <linux/neighbour.h>

enum class NeighborOrigin {
    Other,
    Static,
    Dynamic,
};

/**
 * @brief Convert neighbor origin to string.
 */
std::string neighborOriginToString(NeighborOrigin origin);

enum class NeighborState {
    Incomplete = NUD_INCOMPLETE,
    Reachable = NUD_REACHABLE,
    Stale = NUD_STALE,
    Delay = NUD_DELAY,
    Probe = NUD_PROBE,
};

/**
 * @brief Convert neighbor state to string.
 */
std::string neighborStateToString(NeighborState state);

class NeighborRef {
public:
    using Self = NeighborRef;
    friend class CacheRef<Self>; ///< Allow cache class to use the private constructor.

    /**
     * @brief Return the interface index of a neighbor.
     */
    uint32_t getInterfaceIndex() const;

    /**
     * @brief Get the address family of a neighbor.
     */
    AddressFamily getAddressFamily() const;

    /**
     * @brief Get the origin of a neighbor.
     */
    NeighborOrigin getOrigin() const;

    /**
     * @brief Get the state of a neighbor.
     */
    NeighborState getState() const;

    /**
     * @brief Get the destination IP address of a neighbor.
     */
    std::string getDestinationIP() const;

    /**
     * @brief Get the link-layer address of a neighbor.
     */
    std::string getLinkLayerAddress() const;

    /**
     * @brief Returns true if the neighbor acts as a router.
     */
    bool isRouter() const;

private:
    using RtnlNeigh = struct rtnl_neigh; ///< Route NL neighbor type alias;
    using RtnlNeighDeleter = NlDeleter<RtnlNeigh>; ///< Deleter type alias.
    using RtnlNeighPtr = std::unique_ptr<RtnlNeigh, RtnlNeighDeleter>; ///< Unique pointer type alias.

    /**
     * @brief Private constructor accessible only to friend classes. Stores a reference to rtnl_neigh for later access of address members.
     */
    NeighborRef(struct rtnl_neigh* neigh);

    /**
     * @brief Private constructor accessible only to friend classes. Stores a reference to rtnl_neigh for later access of address members.
     */
    NeighborRef(struct nl_object* neigh);

    RtnlNeighPtr m_neigh; ///< Neighbor reference.
};
