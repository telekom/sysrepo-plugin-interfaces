#include "IPV6.hpp"
#include <iostream>

IPV6::IPV6(int ifindex)
    : m_ifindex { ifindex }
{
}

void IPV6::addAddress(const Address& address) { removeOrAddAddress(address, false); }
void IPV6::addNeighbor(const Neighbour& neigh) { createOrModifyNeighbour(neigh, NLM_F_CREATE); }
void IPV6::modifyNetworkLinkLayer(const Neighbour& neigh) { createOrModifyNeighbour(neigh, NLM_F_REPLACE); };
void IPV6::removeAddress(const Address& address) { removeOrAddAddress(address, true); }

std::vector<Address> IPV6::getAddressList()
{

    nl_sock* socket = NULL;
    nl_cache* cache = NULL;

    std::vector<Address> list;

    auto clean = [&]() {
        if (socket != NULL)
            nl_socket_free(socket);
        if (cache != NULL)
            nl_cache_put(cache);
    };

    struct Arguments {
        std::vector<Address>* lst;
        int ifindex;
    } args;

    args.ifindex = this->m_ifindex;
    args.lst = &list;

    socket = nl_socket_alloc();
    if (!socket) {
        clean();
        throw std::runtime_error("Failed to initialize socket!");
    }

    if (nl_connect(socket, NETLINK_ROUTE) < 0) {
        clean();
        throw std::runtime_error("Failed to connect to socket!");
    }

    if (rtnl_addr_alloc_cache(socket, &cache) < 0) {
        clean();
        throw std::runtime_error("Failed to allocate address cache!");
    }

    nl_cache_foreach(
        cache,
        [](nl_object* obj, void* arg) {
            Arguments* arguments = static_cast<Arguments*>(arg);

            if ((rtnl_addr_get_ifindex((rtnl_addr*)obj) == arguments->ifindex) && (rtnl_addr_get_family((rtnl_addr*)obj) == AF_INET6)) {
                nl_addr* local_addr = rtnl_addr_get_local((rtnl_addr*)obj);
                char buffer[50];
                nl_addr2str(local_addr, buffer, 50);
                int prefix_length = rtnl_addr_get_prefixlen((rtnl_addr*)obj);

                arguments->lst->push_back(Address(buffer, prefix_length));
            }
        },
        &args);

    clean();
    return list;
}

std::vector<Neighbour> IPV6::getNeighborList()
{
    std::vector<Neighbour> list;

    nl_sock* socket = NULL;
    nl_cache* cache = NULL;

    struct Arguments {
        int ifindex;
        std::vector<Neighbour>* lst;
    } args;

    args.ifindex = this->m_ifindex;
    args.lst = &list;

    socket = nl_socket_alloc();
    if (!socket) {
        nl_socket_free(socket);
        throw std::runtime_error("Failed to initialize socket!");
    }

    if (nl_connect(socket, NETLINK_ROUTE) < 0) {
        nl_socket_free(socket);
        throw std::runtime_error("Failed to connect to socket!");
    }

    if (rtnl_neigh_alloc_cache(socket, &cache) < 0) {
        nl_socket_free(socket);
        throw std::runtime_error("Failed to allocate neighbor cache!");
    }

    nl_cache_foreach(
        cache,
        [](nl_object* obj, void* arg) {
            Arguments* arguments = static_cast<Arguments*>(arg);

            if ((rtnl_neigh_get_ifindex((rtnl_neigh*)obj) == arguments->ifindex) && (rtnl_neigh_get_family((rtnl_neigh*)obj) == AF_INET6)) {

                nl_addr* neigh_addr = rtnl_neigh_get_dst((rtnl_neigh*)obj);
                nl_addr* neigh_lladdr = rtnl_neigh_get_lladdr((rtnl_neigh*)obj);

                char buffer[50];

                nl_addr2str(neigh_addr, buffer, sizeof(buffer));

                const std::string address = buffer;

                nl_addr2str(neigh_lladdr, buffer, sizeof(buffer));
                const std::string ll_address = buffer;

                arguments->lst->push_back(Neighbour(address, ll_address));
            };
        },
        &args);

    nl_socket_free(socket);
    nl_cache_put(cache);
    return list;
}

void IPV6::removeOrAddAddress(const Address& address, bool remove)
{
    nl_sock* socket = NULL;
    nl_cache* link_cache = NULL;
    rtnl_addr* addr = NULL;
    nl_addr* lo_addr = NULL;
    rtnl_link* link = NULL;

    auto clean = [&]() {
        if (socket != NULL)
            nl_socket_free(socket);
        if (link_cache != NULL)
            nl_cache_put(link_cache);
        if (addr != NULL)
            rtnl_addr_put(addr);
        if (lo_addr != NULL)
            nl_addr_put(lo_addr);
        if (link != NULL)
            rtnl_link_put(link);
    };

    socket = nl_socket_alloc();
    if (!socket) {
        clean();
        throw std::runtime_error("Failed to initialize socket!");
    }

    if (nl_connect(socket, NETLINK_ROUTE) < 0) {
        clean();
        throw std::runtime_error("Failed to connect to socket!");
    }

    if (rtnl_link_alloc_cache(socket, AF_UNSPEC, &link_cache) < 0) {
        clean();
        throw std::runtime_error("Failed to allocate link cache!");
    }

    link = rtnl_link_get(link_cache, m_ifindex);
    if (link == NULL) {
        clean();
        throw std::runtime_error("Failed to get link from cache!");
    }

    addr = rtnl_addr_alloc();

    if (addr == NULL) {
        clean();
        throw std::runtime_error("Failed to allocate address!");
    }

    rtnl_addr_set_link(addr, link);
    rtnl_addr_set_family(addr, AF_INET6);
    rtnl_addr_set_prefixlen(addr, address.getPrefixLen());
    rtnl_addr_set_ifindex(addr, m_ifindex);

    int err = nl_addr_parse(address.getAddressAndPrefix().c_str(), AF_INET6, &lo_addr);

    if (err < 0) {

        clean();
        throw std::runtime_error("Failed to parse address! reason: " + std::string(nl_geterror(err)));
    }
    int locl_err = rtnl_addr_set_local(addr, lo_addr);

    if (locl_err < 0) {
        clean();
        throw std::runtime_error("Failed to set local address! reason: " + std::string(nl_geterror(locl_err)));
    }

    int add_err;
    std::string add_or_remove;

    if (remove) {
        add_err = rtnl_addr_delete(socket, addr, 0);
        add_or_remove = "remove";
    } else {
        add_err = rtnl_addr_add(socket, addr, 0);
        add_or_remove = "add";
    }

    if (add_err < 0) {

        clean();
        throw std::runtime_error("Failed to " + add_or_remove + " address! reason: " + std::string(nl_geterror(add_err)));
    }

    clean();
}

void IPV6::createOrModifyNeighbour(const Neighbour& neigh, int flags)
{
    nl_sock* socket = NULL;
    rtnl_neigh* neighbour = NULL;
    nl_addr* lo_addr = NULL;
    nl_addr* addr = NULL;

    auto clean = [&]() {
        if (socket != NULL)
            nl_socket_free(socket);
        if (neighbour != NULL)
            rtnl_neigh_put(neighbour);
        if (lo_addr != NULL)
            nl_addr_put(lo_addr);
        if (addr != NULL)
            nl_addr_put(addr);
    };

    socket = nl_socket_alloc();
    if (!socket) {
        clean();
        throw std::runtime_error("Failed to initialize socket!");
    }

    if (nl_connect(socket, NETLINK_ROUTE) < 0) {
        clean();
        throw std::runtime_error("Failed to connect to socket!");
    }

    neighbour = rtnl_neigh_alloc();
    if (neighbour == NULL) {
        clean();
        throw std::runtime_error("Failed to allocate address!");
    }

    int err = nl_addr_parse(neigh.getLinkLayer().c_str(), AF_LLC, &lo_addr);
    int addr_err = nl_addr_parse(neigh.getAddress().c_str(), AF_INET6, &addr);

    if (addr_err < 0) {
        clean();
        throw std::runtime_error("Failed to parse address! reason: " + std::string(nl_geterror(addr_err)));
    }

    if (err < 0) {
        clean();
        throw std::runtime_error("Failed to parse address! reason: " + std::string(nl_geterror(err)));
    }
    rtnl_neigh_set_lladdr(neighbour, lo_addr);
    rtnl_neigh_set_ifindex(neighbour, m_ifindex);
    rtnl_neigh_set_state(neighbour, NUD_PERMANENT);
    rtnl_neigh_set_family(neighbour, AF_INET6);
    int dst_err = rtnl_neigh_set_dst(neighbour, addr);

    if (dst_err < 0) {
        throw std::runtime_error("Failed to set destination! reason: " + std::string(nl_geterror(dst_err)));
    }

    int locl_err = rtnl_neigh_add(socket, neighbour, flags);
    if (locl_err < 0) {
        clean();
        throw std::runtime_error("Failed to add/modify neighbour! reason: " + std::string(nl_geterror(locl_err)));
    };

    clean();
}

void IPV6::removeNeighbor(const std::string& neigh)
{

    nl_sock* socket = NULL;
    rtnl_neigh* neighbour = NULL;
    nl_addr* lo_addr = NULL;
    nl_addr* addr = NULL;

    auto clean = [&]() {
        if (socket != NULL)
            nl_socket_free(socket);
        if (neighbour != NULL)
            rtnl_neigh_put(neighbour);
        if (lo_addr != NULL)
            nl_addr_put(lo_addr);
        if (addr != NULL)
            nl_addr_put(addr);
    };

    socket = nl_socket_alloc();
    if (!socket) {
        clean();
        throw std::runtime_error("Failed to initialize socket!");
    }

    if (nl_connect(socket, NETLINK_ROUTE) < 0) {
        clean();
        throw std::runtime_error("Failed to connect to socket!");
    }

    neighbour = rtnl_neigh_alloc();
    if (neighbour == NULL) {
        clean();
        throw std::runtime_error("Failed to allocate address!");
    }

    int addr_err = nl_addr_parse(neigh.c_str(), AF_INET6, &addr);

    if (addr_err < 0) {
        clean();
        throw std::runtime_error(nl_geterror(addr_err));
    }

    rtnl_neigh_set_ifindex(neighbour, m_ifindex);
    rtnl_neigh_set_dst(neighbour, addr);

    int locl_err = rtnl_neigh_delete(socket, neighbour, NLM_F_NONREC | NLM_F_MATCH);

    if (locl_err < 0) {
        clean();
        throw std::runtime_error(nl_geterror(addr_err));
    };

    clean();
}