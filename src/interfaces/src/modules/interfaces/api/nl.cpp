#include "nl.hpp"
#include <memory>
#include <stdexcept>

// libnl
#include <netlink/route/addr.h>
#include <netlink/route/link.h>

// include types
#include "interface.hpp"
#include "address.hpp"
#include "cache.hpp"

/**
 * @brief Default constructor. Allocates each member of the class.
 */
NlContext::NlContext()
{
    int error = 0;
    struct nl_sock* sock = nullptr;
    struct nl_cache* link_cache = nullptr;
    struct nl_cache* addr_cache = nullptr;

    sock = nl_socket_alloc();
    if (!sock) {
        throw std::runtime_error("Unable to create netlink socket");
    }

    // connect to netlink route and get all links and addresses
    m_sock = std::unique_ptr<struct nl_sock, NlDeleter<struct nl_sock>>(sock, nl_socket_free);

    error = nl_connect(m_sock.get(), NETLINK_ROUTE);
    if (error != 0) {
        throw std::runtime_error("Unable to connect to NETLINK_ROUTE");
    }

    error = rtnl_link_alloc_cache(m_sock.get(), AF_UNSPEC, &link_cache);
    if (error != 0) {
        throw std::runtime_error("Unable to alloc link cache");
    }

    m_linkCache = std::unique_ptr<struct nl_cache, NlDeleter<struct nl_cache>>(link_cache, nl_cache_free);

    error = rtnl_addr_alloc_cache(m_sock.get(), &addr_cache);
    if (error != 0) {
        throw std::runtime_error("Unable to alloc addr cache");
    }

    m_addressCache = std::unique_ptr<struct nl_cache, NlDeleter<struct nl_cache>>(addr_cache, nl_cache_free);
}

/**
 * @brief Refill each cache.
 */
void NlContext::refillCache(void)
{
    nl_cache_refill(m_sock.get(), m_linkCache.get());
    nl_cache_refill(m_sock.get(), m_addressCache.get());
}

/**
 * @brief Return names of all links found in the link cache.
 *
 * @return Names of links in the cache.
 */
std::vector<std::string> NlContext::getLinkNames()
{
    std::vector<std::string> names;
    struct rtnl_link* iter = (struct rtnl_link*)nl_cache_get_first(m_linkCache.get());

    while (iter != nullptr) {
        const char* name = rtnl_link_get_name(iter);

        names.push_back(name);

        iter = (struct rtnl_link*)nl_cache_get_next((struct nl_object*)iter);
    }

    return names;
}

/**
 * @brief Return an interface found in cache by name.
 */
std::optional<InterfaceRef> NlContext::getInterfaceByName(const std::string& name)
{
    struct rtnl_link* iter = (struct rtnl_link*)nl_cache_get_first(m_linkCache.get());

    while (iter != nullptr) {
        const char* link_name = rtnl_link_get_name(iter);

        if (name == link_name) {
            return InterfaceRef(iter);
        }

        iter = (struct rtnl_link*)nl_cache_get_next((struct nl_object*)iter);
    }

    return std::nullopt;
}

/**
 * @brief Return an interface found in cache by its index.
 */
std::optional<InterfaceRef> NlContext::getInterfaceByIndex(const uint32_t index)
{
    auto link = rtnl_link_get(m_linkCache.get(), index);

    // can return NULL - check for invalid link ptr
    if (link) {
        return InterfaceRef(link);
    }

    return std::nullopt;
}

/**
 * @brief Get the links cache.
 */
CacheRef<InterfaceRef> NlContext::getLinkCache() { return CacheRef<InterfaceRef>(m_linkCache.get()); }
