#include "Address.hpp"

Address::Address()
{
    this->m_address.clear();
    this->m_prefix_len = 0;
}

Address::Address(std::string address, int prefix_len)
    : m_address { address }
    , m_prefix_len { prefix_len } {};
Address::Address(std::string address_with_prefix)
{
    size_t delimiter_pos = address_with_prefix.find("/");
    if (delimiter_pos == std::string::npos) {
        throw std::runtime_error("Failed to parse address, no delimiter!");
    }

    this->m_address = address_with_prefix.substr(0, delimiter_pos);
    this->m_prefix_len = std::stoi(address_with_prefix.substr(delimiter_pos + 1));
}

void Address::setPrefixLength(int prefix_len) { this->m_prefix_len = prefix_len; };
void Address::setAddressString(std::string address) { this->m_address = address; };
int Address::getPrefixLen() const { return this->m_prefix_len; };
std::string Address::getAddressString() const { return this->m_address; };
std::string Address::getAddressAndPrefix() const { return std::string(m_address + "/" + std::to_string(m_prefix_len)); } ;
