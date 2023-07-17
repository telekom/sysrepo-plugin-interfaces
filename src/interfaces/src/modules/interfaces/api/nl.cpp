#include "nl.hpp"
#include <memory>
#include <stdexcept>

// libnl
#include <netlink/route/addr.h>
#include <netlink/route/link.h>

// include types
#include "interface.hpp"

/**
 * @brief Default constructor. Allocates each member of the class.
 */
NLContext::NLContext()
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
    m_sock = std::unique_ptr<struct nl_sock, NLDeleter<struct nl_sock>>(sock, nl_socket_free);

    error = nl_connect(m_sock.get(), NETLINK_ROUTE);
    if (error != 0) {
        throw std::runtime_error("Unable to connect to NETLINK_ROUTE");
    }

    error = rtnl_link_alloc_cache(m_sock.get(), AF_UNSPEC, &link_cache);
    if (error != 0) {
        throw std::runtime_error("Unable to alloc link cache");
    }

    m_linkCache = std::unique_ptr<struct nl_cache, NLDeleter<struct nl_cache>>(link_cache, nl_cache_free);

    error = rtnl_addr_alloc_cache(m_sock.get(), &addr_cache);
    if (error != 0) {
        throw std::runtime_error("Unable to alloc addr cache");
    }

    m_addressCache = std::unique_ptr<struct nl_cache, NLDeleter<struct nl_cache>>(addr_cache, nl_cache_free);
}

/**
 * @brief Refill each cache.
 */
void NLContext::refillCache(void)
{
    nl_cache_refill(m_sock.get(), m_linkCache.get());
    nl_cache_refill(m_sock.get(), m_addressCache.get());
}

/**
 * @brief Return names of all links found in the link cache.
 *
 * @return Names of links in the cache.
 */
std::vector<std::string> NLContext::getLinkNames()
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
std::optional<Interface> NLContext::getInterfaceByName(const std::string& name)
{
    struct rtnl_link* iter = (struct rtnl_link*)nl_cache_get_first(m_linkCache.get());

    while (iter != nullptr) {
        const char* link_name = rtnl_link_get_name(iter);

        if (name == link_name) {
            return Interface(iter);
        }

        iter = (struct rtnl_link*)nl_cache_get_next((struct nl_object*)iter);
    }

    return std::nullopt;
}
