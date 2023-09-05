#pragma once

#include <memory>
#include <functional>
#include <optional>

///< Type used for deleting libnl allocated structs
template <typename T> using NlDeleter = std::function<void(T*)>;

///< Empty deleter which does nothing with the type. Used for storing shared pointers.
template <typename T> void NlEmptyDeleter(T*) { }

// Predefined classes
class InterfaceRef;
class AddressRef;
class RouteAddressRef;
class NeighborRef;
template <typename T> class CacheRef;

/**
 * @brief Netlink context using the libnl library. Used for updating system networking configuration.
 */
class NlContext {
public:
    /**
     * @brief Default constructor. Allocates each member of the class.
     */
    NlContext();

    /**
     * @brief Refill each cache.
     */
    void refillCache(void);

    /**
     * @brief Return names of all links found in the link cache.
     *
     * @return Names of links in the cache.
     */
    std::vector<std::string> getLinkNames();

    /**
     * @brief Return an interface found in cache by name.
     */
    std::optional<InterfaceRef> getInterfaceByName(const std::string& name);

    /**
     * @brief Return an interface found in cache by its index.
     */
    std::optional<InterfaceRef> getInterfaceByIndex(const uint32_t index);

    /**
     * @brief Create interface.
     */
    void createInterface(std::string name, std::string type, bool enabled);

    /**
     * @brief Delete existing interface, if not existant, throws an exception.
     */
    void deleteInterface(const std::string& name);

    /**
     * @brief Get the links cache.
     */
    CacheRef<InterfaceRef> getLinkCache();

    /**
     * @brief Get the address cache.
     */
    CacheRef<RouteAddressRef> getAddressCache();

    /**
     * @brief Get the neighbors cache.
     */
    CacheRef<NeighborRef> getNeighborCache();

private:
    std::unique_ptr<struct nl_sock, NlDeleter<struct nl_sock>> m_sock; ///< Netlink socket.
    std::unique_ptr<struct nl_cache, NlDeleter<struct nl_cache>> m_linkCache; ///< Links cache.
    std::unique_ptr<struct nl_cache, NlDeleter<struct nl_cache>> m_addressCache; ///< Addresses cache.
    std::unique_ptr<struct nl_cache, NlDeleter<struct nl_cache>> m_neighCache; ///< Neighbors cache.
};
