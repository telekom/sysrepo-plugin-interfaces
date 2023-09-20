#pragma once

#include <string>
#include <netlink/route/addr.h>
#include <stdexcept>
#include <vector>
#include "Neighbour.hpp"
#include "Address.hpp"

class IPV4 {
private:
    int ifindex;
    void removeOrAddAddress(const Address& address, bool remove) const;
    void createOrModifyNeighbour(const Neighbour& neigh, int flags = 0) const;

public:
    IPV4(int ifindex);
    IPV4() = delete;

    bool getEnabled();

    int getMtu();

    void setMtu(unsigned int mtu);

    std::string getIPWithPrefix();

    std::string getIPAddress();

    int getPrefixLen();

    std::vector<Neighbour> getNeighbours();
    std::vector<Address> getAdressList();

    void addAddress(const Address& address);

    void removeAddress(const Address& address) const;

    void addNeighbour(const Neighbour& neigh);

    void modifyNeighbourLinkLayer(const Neighbour& neigh);
    void removeNeighbor(const std::string& neigh);
};
