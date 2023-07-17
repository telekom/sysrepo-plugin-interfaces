#pragma once

#include <netlink/route/link.h>
#include <memory>
#include <functional>
#include <optional>

///< Type used for deleting libnl allocated structs
template <typename T> using NLDeleter = std::function<void(T*)>;

///< Empty deleter which does nothing with the type. Used for storing shared pointers.
template <typename T> void NLEmptyDeleter(T*) { }

// Predefined classes
class Interface;

/**
 * @brief Netlink context using the libnl library. Used for updating system networking configuration.
 */
class NLContext {
public:
    /**
     * @brief Default constructor. Allocates each member of the class.
     */
    NLContext();

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
    std::optional<Interface> getInterfaceByName(const std::string& name);

private:
    std::unique_ptr<struct nl_sock, NLDeleter<struct nl_sock>> m_sock; ///< Netlink socket.
    std::unique_ptr<struct nl_cache, NLDeleter<struct nl_cache>> m_linkCache; ///< Links cache.
    std::unique_ptr<struct nl_cache, NLDeleter<struct nl_cache>> m_addressCache; ///< Addresses cache.
    std::unique_ptr<struct nl_cache, NLDeleter<struct nl_cache>> m_neighCache; ///< Neighbors cache.
};
