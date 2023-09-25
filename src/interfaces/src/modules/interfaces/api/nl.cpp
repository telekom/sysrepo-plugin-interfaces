#include "nl.hpp"

// libnl
#include <netlink/route/addr.h>
#include <netlink/route/link.h>
#include <linux/if.h>

// include types
#include "interface.hpp"
#include "address.hpp"
#include "neighbor.hpp"
#include "cache.hpp"
#include "netlink/route/neighbour.h"

/**
 * @brief Default constructor. Allocates each member of the class.
 */
NlContext::NlContext()
{
    int error = 0;
    struct nl_sock* sock = nullptr;
    struct nl_cache* link_cache = nullptr;
    struct nl_cache* addr_cache = nullptr;
    struct nl_cache* neigh_cache = nullptr;

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

    error = rtnl_neigh_alloc_cache(m_sock.get(), &neigh_cache);
    if (error != 0) {
        throw std::runtime_error("Unable to alloc neighbor cache");
    }

    m_neighCache = std::unique_ptr<struct nl_cache, NlDeleter<struct nl_cache>>(neigh_cache, nl_cache_free);
}

/**
 * @brief Refill each cache.
 */
void NlContext::refillCache(void)
{
    if (nl_cache_refill(m_sock.get(), m_linkCache.get()) < 0) {
        throw std::runtime_error("Unable to refill links cache");
    }

    if (nl_cache_refill(m_sock.get(), m_addressCache.get())) {
        throw std::runtime_error("Unable to refill address cache");
    }

    if (nl_cache_refill(m_sock.get(), m_neighCache.get())) {
        throw std::runtime_error("Unable to refill neighbors cache");
    }
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

        names.push_back(std::string(name));

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

        if (name == std::string(link_name)) {
            return InterfaceRef(iter, m_sock.get());
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
        return InterfaceRef(link, m_sock.get());
    }
    return std::nullopt;
}

/**
 * @brief Create interface.
 */
void NlContext::createInterface(std::string name, std::string type, bool enabled)
{

    // name cannot be more than 15 characters
    if (name.size() > 15)
        throw std::runtime_error("Name to long!");
    // check if type exists
    if (InterfaceTypes.find(type) == InterfaceTypes.end())
        throw std::runtime_error("Unsupported type");

    // allocate link
    rtnl_link* link = rtnl_link_alloc();
    if (link == NULL)
        throw std::runtime_error("link alloc error!");

    // set name to link
    rtnl_link_set_name(link, name.c_str());

    // set type to link
    if (rtnl_link_set_type(link, InterfaceTypes.at(type).c_str()) < 0) {
        rtnl_link_put(link);
        throw std::runtime_error("rtnl_link_set_type error");
    };

    enabled ? rtnl_link_set_flags(link, IFF_UP) : rtnl_link_unset_flags(link, IFF_UP);

    // add link to socket
    if (rtnl_link_add(m_sock.get(), link, NLM_F_CREATE) < 0) {
        rtnl_link_put(link);
        throw std::runtime_error("rtnl_link_add error");
    };
}

/**
 * @brief Delete existing interface, if not existant, throws an exception.
 */
void NlContext::deleteInterface(const std::string& name)
{
    refillCache();
    struct rtnl_link* iter = (struct rtnl_link*)nl_cache_get_first(m_linkCache.get());

    while (iter != nullptr) {
        const char* link_name = rtnl_link_get_name(iter);

        if (name == std::string(link_name)) {
            int error = rtnl_link_delete(m_sock.get(), iter);
            error < 0 ? throw std::runtime_error(nl_geterror(error)) : NULL;
            break;
        }

        iter = (struct rtnl_link*)nl_cache_get_next((struct nl_object*)iter);
    };
}

/**
 * @brief Create new address.
 */
void NlContext::createAddress(std::string interface_name, std::string address, int prefix_length, AddressFamily fam)
{
    auto interface = getInterfaceByName(interface_name);

    rtnl_addr* rtnl_address = NULL;
    nl_addr* nl_address = NULL;
    int err = 0;

    auto clean = [&]() {
        if (rtnl_address != NULL)
            rtnl_addr_put(rtnl_address);
        if (nl_address != NULL)
            nl_addr_put(nl_address);
    };

    if (!interface.has_value())
        throw std::bad_optional_access();

    rtnl_address = rtnl_addr_alloc();

    err = nl_addr_parse(address.c_str(), (int)fam, &nl_address);

    if (err < 0) {
        clean();
        throw std::runtime_error(nl_geterror(err));
    }

    err = rtnl_addr_set_local(rtnl_address, nl_address);

    if (err < 0) {
        clean();
        throw std::runtime_error(nl_geterror(err));
    }

    rtnl_addr_set_link(rtnl_address, interface->m_link.get());
    rtnl_addr_set_family(rtnl_address, (int)fam);
    rtnl_addr_set_prefixlen(rtnl_address, prefix_length);

    err = rtnl_addr_add(m_sock.get(), rtnl_address, 0);

    if (err < 0) {
        clean();
        throw std::runtime_error(nl_geterror(err));
    }
    clean();
}

/**
 * @brief Delete existing Address, if not existant, throws an exception.
 */
void NlContext::deleteAddress(std::string interface_name, std::string address, int prefix_length, AddressFamily fam)
{

    auto interface = getInterfaceByName(interface_name);

    rtnl_addr* rtnl_address = NULL;
    nl_addr* nl_address = NULL;
    int err = 0;

    auto clean = [&]() {
        if (rtnl_address != NULL)
            rtnl_addr_put(rtnl_address);
        if (nl_address != NULL)
            nl_addr_put(nl_address);
    };

    if (!interface.has_value())
        throw std::bad_optional_access();

    rtnl_address = rtnl_addr_alloc();
    err = nl_addr_parse(address.c_str(), (int)fam, &nl_address);

    if (err < 0) {
        clean();
        throw std::runtime_error(nl_geterror(err));
    }

    err = rtnl_addr_set_local(rtnl_address, nl_address);

    if (err < 0) {
        clean();
        throw std::runtime_error(nl_geterror(err));
    }

    rtnl_addr_set_link(rtnl_address, interface->m_link.get());
    rtnl_addr_set_family(rtnl_address, (int)fam);
    rtnl_addr_set_prefixlen(rtnl_address, prefix_length);

    err = rtnl_addr_delete(m_sock.get(), rtnl_address, 0);

    if (err < 0) {
        clean();
        throw std::runtime_error(nl_geterror(err));
    }
    clean();
}

/**
 * @brief Create Neighbor
 */
void NlContext::neighbor(std::string interface_name, std::string address, std::string ll_addr, AddressFamily fam, NeighborOperations oper)
{
    auto interface = getInterfaceByName(interface_name);

    if (!interface.has_value())
        throw std::bad_optional_access();

    rtnl_neigh* neighbor = NULL;
    nl_addr* lo_addr = NULL;
    nl_addr* addr = NULL;
    int err = 0;

    auto clean = [&]() {
        if (neighbor != NULL)
            rtnl_neigh_put(neighbor);
        if (lo_addr != NULL)
            nl_addr_put(lo_addr);
        if (addr != NULL)
            nl_addr_put(addr);
    };

    int ifindex = interface->getIndex();

    neighbor = rtnl_neigh_alloc();

    err = nl_addr_parse(address.c_str(), (int)fam, &addr);

    if (err < 0) {
        clean();
        throw std::runtime_error(nl_geterror(err));
    }

    err = nl_addr_parse(ll_addr.c_str(), AF_LLC, &lo_addr);

    if (err < 0) {
        clean();
        throw std::runtime_error(nl_geterror(err));
    }

    rtnl_neigh_set_lladdr(neighbor, lo_addr);
    rtnl_neigh_set_ifindex(neighbor, ifindex);
    rtnl_neigh_set_state(neighbor, NUD_PERMANENT);
    rtnl_neigh_set_family(neighbor, (int)fam);

    err = rtnl_neigh_set_dst(neighbor, addr);

    if (err < 0) {
        clean();
        throw std::runtime_error(nl_geterror(err));
    }

    switch (oper) {
    case NeighborOperations::Create:
        err = rtnl_neigh_add(m_sock.get(), neighbor, NLM_F_CREATE);
        break;
    case NeighborOperations::Modify:
        err = rtnl_neigh_add(m_sock.get(), neighbor, NLM_F_REPLACE);
        break;
    case NeighborOperations::Delete:
        err = rtnl_neigh_delete(m_sock.get(), neighbor, NLM_F_NONREC | NLM_F_MATCH);
        break;
    }

    if (err < 0) {
        clean();
        throw std::runtime_error(nl_geterror(err));
    }

    clean();
}

/**
 * @brief Get the links cache.
 */
CacheRef<InterfaceRef> NlContext::getLinkCache() { return CacheRef<InterfaceRef>(m_linkCache.get(), m_sock.get()); }

/**
 * @brief Get the address cache.
 */
CacheRef<RouteAddressRef> NlContext::getAddressCache() { return CacheRef<RouteAddressRef>(m_addressCache.get(), m_sock.get()); }

/**
 * @brief Get the neighbors cache.
 */
CacheRef<NeighborRef> NlContext::getNeighborCache() { return CacheRef<NeighborRef>(m_neighCache.get(), m_sock.get()); }
