#pragma once

#include <string>
#include <netlink/route/addr.h>
#include <stdexcept>
#include <vector>
#include "Neighbour.hpp"
#include "Address.hpp"

class IPV6 {
private:
    int m_ifindex;
    // bool m_enabled;
    // bool m_forwarding;
    // int m_dup_addr_detect_transmits;

    void removeOrAddAddress(const Address& address, bool remove);

    void createOrModifyNeighbour(const Neighbour& neigh, int flags);

public:
    IPV6(int ifindex);

    void addAddress(const Address& address);

    void addNeighbor(const Neighbour& neigh);

    void modifyNetworkLinkLayer(const Neighbour& neigh);

    void removeNeighbor(const std::string& neigh);

    void removeAddress(const Address& address);

    int getMTU();

    void setMTU();
};