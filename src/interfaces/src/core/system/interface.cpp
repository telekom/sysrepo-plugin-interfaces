#include "interface.hpp"

Interface::Interface() = delete;

Interface::Interface(const Interface&) = delete;

Interface::Interface(Interface&&) = delete;

Interface::Interface(int ifindex)
    : ifindex { ifindex }
    , ipv4_address { ifindex } {};

IPV4 Interface::getIPV4() { return this->ipv4_address; };

void Interface::create(std::string name, std::string type, bool enabled)
{
    // limit the name size
    if (name.size() > 15)
        throw std::runtime_error("Name to long!");

    std::string iana_type;
    if (type == "iana-if-type:ethernetCsmacd")
        iana_type = "veth";
    else if (type == "iana-if-type:softwareLoopback")
        iana_type = "vcan";
    else if (type == "iana-if-type:l2vlan")
        iana_type = "vlan";
    else if (type == "iana-if-type:other")
        iana_type = "dummy";
    else if (type == "iana-if-type:bridge")
        iana_type = "bridge";
    else
        throw std::runtime_error("Unsuported type!");

    rtnl_link* link = NULL;
    nl_sock* socket = NULL;

    auto clean = [&]() {
        if (link != NULL)
            rtnl_link_put(link);
        if (socket != NULL)
            nl_socket_free(socket);
    };

    socket = nl_socket_alloc();
    if (socket == NULL) {
        throw std::runtime_error("Failed to allocate socket!");
    }

    link = rtnl_link_alloc();
    if (link == NULL) {
        clean();
        throw std::runtime_error("Failed to allocate Link!");
    }

    if (nl_connect(socket, NETLINK_ROUTE) < 0) {
        clean();
        throw std::runtime_error("Socket failed to connect!");
    }

    rtnl_link_set_name(link, name.c_str());

    enabled ? rtnl_link_set_flags(link, rtnl_link_str2flags("up")) : rtnl_link_unset_flags(link, rtnl_link_str2flags("up"));

    int type_err = rtnl_link_set_type(link, iana_type.c_str());

    if (type_err < 0) {
        clean();
        throw std::runtime_error(nl_geterror(type_err));
    };

    int link_err = rtnl_link_add(socket, link, NLM_F_CREATE);

    if (link_err < 0) {
        clean();
        throw std::runtime_error(nl_geterror(link_err));
    };
    clean();
}

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

    bool enabled = false;

    int operstate = this->getOperStatus();
    if (operstate == 6 || operstate == 0) {
        enabled = true;
    } else
        enabled = false;

    return enabled;
}

int Interface::getOperStatus()
{

    int operstate = -1;

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

    operstate = rtnl_link_get_operstate(lnk);

    nl_socket_free(socket);
    nl_cache_put(cache);
    rtnl_link_put(lnk);

    return operstate;
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
    if (name.size() > 15) {
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

void Interface::setType(std::string type)
{
    std::string iana_type;

    if (type == "iana-if-type:ethernetCsmacd")
        iana_type = "veth";
    else if (type == "iana-if-type:softwareLoopback")
        iana_type = "vcan";
    else if (type == "iana-if-type:l2vlan")
        iana_type = "vlan";
    else if (type == "iana-if-type:other")
        iana_type = "dummy";
    else if (type == "iana-if-type:bridge")
        iana_type = "bridge";
    else
        throw std::runtime_error("Unsuported type!");

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
        int set_err = rtnl_link_set_type(req_link, iana_type.c_str());
        if (set_err < 0) {

            nl_socket_free(socket);
            nl_cache_put(cache);
            rtnl_link_put(lnk);
            rtnl_link_put(req_link);
            throw std::runtime_error(std::string("Failed to set type, reason: ") + std::string(nl_geterror(set_err)));
        }

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

// returns 0 if no ifindex is found
int getIfindexFromName(std::string name)
{
    nl_sock* socket = NULL;
    nl_cache* cache = NULL;

    int ifindex;

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

    ifindex = rtnl_link_name2i(cache, name.c_str());

    nl_socket_free(socket);
    nl_cache_put(cache);

    return ifindex;
}
