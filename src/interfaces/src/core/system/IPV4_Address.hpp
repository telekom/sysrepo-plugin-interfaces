#include <string>
#include <netlink/route/addr.h>
#include <stdexcept>
#include <vector>
#include "Neighbour.hpp"

class IPV4_Address {
private:
    int ifindex;

public:
    IPV4_Address(int ifindex);
    IPV4_Address() = delete;

    bool getEnabled();

    int getMtu();

    void setMtu(unsigned int mtu);

    std::string getIPWithPrefix();

    std::string getIPAddress();

    int getPrefixLen();

    std::vector<Neighbour> getNeighbours();
};
