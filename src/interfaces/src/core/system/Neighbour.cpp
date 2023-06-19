#include "Neighbour.hpp"

Neighbour::Neighbour() = delete;

Neighbour::Neighbour(std::string address, std::string link_layer_address)
    : address { address }
    , link_layer_address { link_layer_address }
{
}

std::string Neighbour::getAddress() const { return this->address; };
std::string Neighbour::getLinkLayer() const { return this->link_layer_address; };