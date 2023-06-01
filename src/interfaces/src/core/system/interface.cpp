#include "interface.hpp"

Interface::Interface() = delete;

Interface::Interface(const Interface&) = delete;

Interface::Interface(Interface&&) = delete;

Interface::Interface(int ifindex)
    : ifindex { ifindex }
    , ipv4_address { ifindex } {};

IPV4_Address Interface::getIPV4() { return this->ipv4_address; };

std::string Interface::getName()
{
    nl_sock* socket = NULL;
    nl_cache* cache = NULL;

    std::string name;

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

    rtnl_link* lnk = rtnl_link_get(cache, ifindex);

    if (lnk != NULL) {
        name = rtnl_link_get_name(lnk);
    }

    nl_socket_free(socket);
    nl_cache_put(cache);
    rtnl_link_put(lnk);

    return name;
}

bool Interface::getEnabled()
{
    nl_sock* socket = NULL;
    nl_cache* cache = NULL;

    bool enabled = false;

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

    rtnl_link* lnk = rtnl_link_get(cache, ifindex);

    if (lnk != NULL) {
        int operstate = rtnl_link_get_operstate(lnk);
        if (operstate == 6 || operstate == 0) {
            enabled = true;
        } else
            enabled = false;
    }

    nl_socket_free(socket);
    nl_cache_put(cache);
    rtnl_link_put(lnk);

    return enabled;
}

std::string Interface::getType()
{
    nl_sock* socket = NULL;
    nl_cache* cache = NULL;

    std::string type;

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

    rtnl_link* lnk = rtnl_link_get(cache, ifindex);

    if (lnk != NULL) {
        const char* nl_type = rtnl_link_get_type(lnk);

        if (nl_type == NULL) {
            type = "iana-if-type:other";
        } else if (strcmp(nl_type, "veth") == 0) {
            type = "iana-if-type:ethernetCsmacd";
        } else if (strcmp(nl_type, "vcan") == 0) {
            type = "iana-if-type:softwareLoopback";
        } else if (strcmp(nl_type, "vlan") == 0) {
            type = "iana-if-type:l2vlan";
        } else if (strcmp(nl_type, "dummy") == 0) {
            type = "iana-if-type:other";
        } else if (strcmp(nl_type, "bridge") == 0) {
            type = "iana-if-type:bridge";
        } else {
            type = "Unknown";
        };
    }

    rtnl_link_put(lnk);
    nl_socket_free(socket);
    nl_cache_put(cache);
    return type;
}

void Interface::setEnabled(bool enabled)
{
    nl_sock* socket = NULL;
    nl_cache* cache = NULL;

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
    // current link
    rtnl_link* lnk = rtnl_link_get(cache, ifindex);

    // req link
    rtnl_link* req_link = rtnl_link_alloc();

    if (lnk != NULL && req_link != NULL) {
        enabled ? rtnl_link_set_flags(req_link, rtnl_link_str2flags("up")) : rtnl_link_unset_flags(req_link, rtnl_link_str2flags("up"));

        int err = rtnl_link_change(socket, lnk, req_link, 0);
        if (err < 0) {

            nl_socket_free(socket);
            nl_cache_put(cache);
            rtnl_link_put(lnk);
            rtnl_link_put(req_link);
            throw std::runtime_error(std::string("Link failed to change, reason: ") + std::string(nl_geterror(err)));
        }
    } else {
        nl_socket_free(socket);
        nl_cache_put(cache);
        rtnl_link_put(lnk);
        rtnl_link_put(req_link);
        throw std::runtime_error("Failed to obtain link!");
    }

    nl_socket_free(socket);
    nl_cache_put(cache);
    rtnl_link_put(lnk);
    rtnl_link_put(req_link);
}

void Interface::setName(std::string name)
{
    if (name.size() > 20) {
        throw std::runtime_error("Name cannot be more than 20 characters!");
    }

    nl_sock* socket = NULL;
    nl_cache* cache = NULL;

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
    // current link
    rtnl_link* lnk = rtnl_link_get(cache, ifindex);

    // req link
    rtnl_link* req_link = rtnl_link_alloc();

    if (lnk != NULL && req_link != NULL) {
        rtnl_link_set_name(req_link, name.c_str());

        int err = rtnl_link_change(socket, lnk, req_link, 0);
        if (err < 0) {

            nl_socket_free(socket);
            nl_cache_put(cache);
            rtnl_link_put(lnk);
            rtnl_link_put(req_link);
            throw std::runtime_error(std::string("Link failed to change, reason: ") + std::string(nl_geterror(err)));
        }
    } else {
        nl_socket_free(socket);
        nl_cache_put(cache);
        rtnl_link_put(lnk);
        rtnl_link_put(req_link);
        throw std::runtime_error("Failed to obtain link!");
    }

    nl_socket_free(socket);
    nl_cache_put(cache);
    rtnl_link_put(lnk);
    rtnl_link_put(req_link);
}

void Interface::remove()
{
    nl_sock* socket = NULL;
    nl_cache* cache = NULL;

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
    // current link
    rtnl_link* lnk = rtnl_link_get(cache, ifindex);

    if (lnk != NULL) {
        int err = rtnl_link_delete(socket, lnk);
        if (err < 0) {

            nl_socket_free(socket);
            nl_cache_put(cache);
            rtnl_link_put(lnk);
            throw std::runtime_error(std::string("Failed to delete link! reason: ") + std::string(nl_geterror(err)));
        }
    } else {
        nl_socket_free(socket);
        nl_cache_put(cache);
        rtnl_link_put(lnk);

        throw std::runtime_error("Failed to obtain link!");
    }

    nl_socket_free(socket);
    nl_cache_put(cache);
    rtnl_link_put(lnk);
}
