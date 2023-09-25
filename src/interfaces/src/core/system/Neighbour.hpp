#pragma once
#include <string>
#include <netlink/route/neighbour.h>

class Neighbour {
private:
    std::string address;
    std::string link_layer_address;

public:
    Neighbour();

    Neighbour(std::string address, std::string link_layer_address);

    std::string getAddress() const;
    std::string getLinkLayer() const;
};