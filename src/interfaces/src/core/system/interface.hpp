#include <string>
#include <stdexcept>
#include <netlink/route/link.h>
#include "IPV4_Address.hpp"


//all previous headers in case something is missing
/*
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <arpa/inet.h>
#include <netlink/netlink.h>
#include <netlink/route/link.h>
#include <netlink/route/addr.h>
#include <netlink/route/neighbour.h>
#include <linux/if.h>
*/

class Interface {
private:
    int ifindex;
    IPV4_Address ipv4_address;

public:
    // disable copy and move constructors
    Interface();

    Interface(const Interface&);

    // Disable move constructor
    Interface(Interface&&);

    Interface(int ifindex);

    std::string getName();

    bool getEnabled();

    std::string getType();

    IPV4_Address getIPV4();

    void setEnabled(bool enabled);

    void setName(std::string name);

    void remove();
};
