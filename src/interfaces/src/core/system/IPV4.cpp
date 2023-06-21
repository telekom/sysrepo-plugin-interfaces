#include "IPV4.hpp"
#include <iostream>

IPV4::IPV4(int ifindex)
    : ifindex { ifindex } {};

bool IPV4::getEnabled()
{
    struct nl_sock* socket;
    struct nl_cache* addr_cache;

    struct Arguments {
        bool enabled;
        int ifindex;
    } args;

    args.enabled = false;
    args.ifindex = this->ifindex;

    socket = nl_socket_alloc();
    if (!socket) {
        nl_socket_free(socket);
        throw std::runtime_error("Failed to initialize socket!");
    }

    if (nl_connect(socket, NETLINK_ROUTE) < 0) {
        nl_socket_free(socket);
        throw std::runtime_error("Failed to connect to socket!");
    }

    if (rtnl_addr_alloc_cache(socket, &addr_cache) < 0) {
        nl_socket_free(socket);
        nl_cache_put(addr_cache);
        throw std::runtime_error("Failed to allocate address cache!");
    }

    nl_cache_foreach(
        addr_cache,
        [](nl_object* obj, void* arg) {
            Arguments* arguments = static_cast<Arguments*>(arg);

            nl_addr* local_addr = rtnl_addr_get_local((rtnl_addr*)obj);
            if (rtnl_addr_get_ifindex((rtnl_addr*)obj) == arguments->ifindex && rtnl_addr_get_family((rtnl_addr*)obj) == AF_INET) {
                arguments->enabled = true;
            }
        },
        &args);

    nl_socket_free(socket);
    nl_cache_put(addr_cache);
    return args.enabled;
}

int IPV4::getMtu()
{
    nl_sock* socket = NULL;
    nl_cache* cache = NULL;

    int mtu = 0;

    socket = nl_socket_alloc();
    if (!socket) {
        nl_socket_free(socket);
        throw std::runtime_error("Failed to initialize socket!");
    }

    if (nl_connect(socket, NETLINK_ROUTE) < 0) {
        nl_socket_free(socket);
        throw std::runtime_error("Failed to connect to socket!");
    }

    if (rtnl_link_alloc_cache(socket, AF_UNSPEC, &cache) < 0) {
        nl_socket_free(socket);
        throw std::runtime_error("Failed to allocate link cache!");
    }

    rtnl_link* lnk = rtnl_link_get(cache, this->ifindex);

    if (lnk != NULL) {
        mtu = rtnl_link_get_mtu(lnk);
    };

    nl_socket_free(socket);
    nl_cache_put(cache);
    rtnl_link_put(lnk);

    return mtu;
}

void IPV4::setMtu(unsigned int mtu)
{
    nl_sock* socket = NULL;
    nl_cache* cache = NULL;
    rtnl_link* lnk = NULL;
    rtnl_link* req_link = NULL;

    auto clean = [&]() {
        if (socket != NULL)
            nl_socket_free(socket);
        if (cache != NULL)
            nl_cache_put(cache);
        if (lnk != NULL)
            rtnl_link_put(lnk);
        if (req_link != NULL)
            rtnl_link_put(req_link);
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

    if (rtnl_link_alloc_cache(socket, AF_UNSPEC, &cache) < 0) {
        clean();
        throw std::runtime_error("Failed to allocate link cache!");
    }

    lnk = rtnl_link_get(cache, this->ifindex);

    // req link
    req_link = rtnl_link_alloc();

    if (lnk != NULL && req_link != NULL) {
        clean();

        int err = rtnl_link_change(socket, lnk, req_link, 0);

        if (err < 0) {
            clean();
            throw std::runtime_error(std::string("Link failed to change, reason: ") + std::string(nl_geterror(err)));
        }
    } else {
        clean();
        throw std::runtime_error("Failed to obtain link!");
    }

    clean();
}

std::string IPV4::getIPWithPrefix()
{
    nl_sock* socket = NULL;
    nl_cache* cache = NULL;

    struct Arguments {
        std::string ip_address;
        int ifindex;
    } args;

    args.ifindex = this->ifindex;
    args.ip_address.clear();

    socket = nl_socket_alloc();
    if (!socket) {
        nl_socket_free(socket);
        throw std::runtime_error("Failed to initialize socket!");
    }

    if (nl_connect(socket, NETLINK_ROUTE) < 0) {
        nl_socket_free(socket);
        throw std::runtime_error("Failed to connect to socket!");
    }

    if (rtnl_addr_alloc_cache(socket, &cache) < 0) {
        nl_socket_free(socket);
        throw std::runtime_error("Failed to allocate address cache!");
    }

    nl_cache_foreach(
        cache,
        [](nl_object* obj, void* arg) {
            Arguments* arguments = static_cast<Arguments*>(arg);

            if ((rtnl_addr_get_ifindex((rtnl_addr*)obj) == arguments->ifindex) && (rtnl_addr_get_family((rtnl_addr*)obj) == AF_INET)) {
                nl_addr* local_addr = rtnl_addr_get_local((rtnl_addr*)obj);
                char buffer[50];
                nl_addr2str(local_addr, buffer, 50);
                arguments->ip_address = buffer;
            }
        },
        &args);

    nl_socket_free(socket);
    nl_cache_put(cache);
    return args.ip_address;
}

std::string IPV4::getIPAddress()
{
    std::string full_ip = this->getIPWithPrefix();
    return std::string(full_ip.substr(0, full_ip.find('/')));
}

int IPV4::getPrefixLen()
{
    std::string full_ip = this->getIPWithPrefix();

    if (full_ip.empty())
        return -1;

    return std::stoi(std::string(full_ip.substr(full_ip.find('/') + 1, full_ip.size())).c_str());
}

std::vector<Neighbour> IPV4::getNeighbours()
{
    std::vector<Neighbour> list;

    nl_sock* socket = NULL;
    nl_cache* cache = NULL;

    struct Arguments {
        int ifindex;
        std::vector<Neighbour>* lst;
    } args;

    args.ifindex = this->ifindex;
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

            if ((rtnl_neigh_get_ifindex((rtnl_neigh*)obj) == arguments->ifindex) && (rtnl_neigh_get_family((rtnl_neigh*)obj) == AF_INET)) {

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

void IPV4::addAddress(const Address& address) { removeOrAddAddress(address, false); };

std::vector<Address> IPV4::getAdressList()
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

    args.ifindex = this->ifindex;
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

            if ((rtnl_addr_get_ifindex((rtnl_addr*)obj) == arguments->ifindex) && (rtnl_addr_get_family((rtnl_addr*)obj) == AF_INET)) {
                nl_addr* local_addr = rtnl_addr_get_local((rtnl_addr*)obj);
                char buffer[50];
                nl_addr2str(local_addr, buffer, 50);
                arguments->lst->push_back(Address(buffer));
            }
        },
        &args);

    clean();
    return list;
};

void IPV4::removeAddress(const Address& address) const { removeOrAddAddress(address, true); };

void IPV4::removeOrAddAddress(const Address& address, bool remove) const
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

    link = rtnl_link_get(link_cache, ifindex);
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
    rtnl_addr_set_family(addr, AF_INET);
    rtnl_addr_set_prefixlen(addr, address.getPrefixLen());
    rtnl_addr_set_ifindex(addr, ifindex);

    int err = nl_addr_parse(address.getAddressAndPrefix().c_str(), AF_INET, &lo_addr);

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

void IPV4::createOrModifyNeighbour(const Neighbour& neigh, int flags) const
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
    int addr_err = nl_addr_parse(neigh.getAddress().c_str(), AF_INET, &addr);

    if (addr_err < 0) {
        clean();
        throw std::runtime_error("Failed to parse address! reason: " + std::string(nl_geterror(addr_err)));
    }

    if (err < 0) {
        clean();
        throw std::runtime_error("Failed to parse address! reason: " + std::string(nl_geterror(err)));
    }
    rtnl_neigh_set_lladdr(neighbour, lo_addr);
    rtnl_neigh_set_ifindex(neighbour, ifindex);
    rtnl_neigh_set_state(neighbour, NUD_PERMANENT);
    rtnl_neigh_set_family(neighbour, AF_INET);
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
};
void IPV4::addNeighbour(const Neighbour& neigh) { createOrModifyNeighbour(neigh, NLM_F_CREATE ); };

void IPV4::modifyNeighbourLinkLayer(const Neighbour& neigh) { createOrModifyNeighbour(neigh, NLM_F_REPLACE ); };

void IPV4::removeNeighbor(const std::string& neigh)
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

    int addr_err = nl_addr_parse(neigh.c_str(), AF_INET, &addr);

    if (addr_err < 0) {
        clean();
        throw std::runtime_error(nl_geterror(addr_err));
    }

    rtnl_neigh_set_ifindex(neighbour, ifindex);
    rtnl_neigh_set_dst(neighbour, addr);

    int locl_err = rtnl_neigh_delete(socket, neighbour, NLM_F_NONREC | NLM_F_MATCH);

    if (locl_err < 0) {
        clean();
        throw std::runtime_error(nl_geterror(locl_err));
    };

    clean();
}
