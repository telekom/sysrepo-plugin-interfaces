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

    void addAddress(const Address& address) ;

    void removeAddress(const Address& address) const;

    void modifyPrefixLength(std::string address, int prefixlen);
};
