#include "IPV4_Address.hpp"

IPV4_Address::IPV4_Address(int ifindex)
    : ifindex { ifindex } {};

bool IPV4_Address::getEnabled()
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

int IPV4_Address::getMtu()
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

std::string IPV4_Address::getIPWithPrefix()
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

std::string IPV4_Address::getIPAddress()
{
    std::string full_ip = this->getIPWithPrefix();
    return std::string(full_ip.substr(0, full_ip.find('/')));
}

int IPV4_Address::getPrefixLen()
{
    std::string full_ip = this->getIPWithPrefix();

    if (full_ip.empty())
        return -1;

    return std::stoi(std::string(full_ip.substr(full_ip.find('/') + 1, full_ip.size())).c_str());
}

std::vector<Neighbour> IPV4_Address::getNeighbours()
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
