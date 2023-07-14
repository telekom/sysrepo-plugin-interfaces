#pragma once

#include <string>
#include <stdexcept>

class Address {
private:
    std::string m_address;
    int m_prefix_len;

public:
    Address();
    Address(std::string address, int prefix_len);
    Address(std::string addr_with_prefix);
    void setPrefixLength(int prefix_len);

    void setAddressString(std::string address);

    int getPrefixLen() const;

    std::string getAddressString() const;

    std::string getAddressAndPrefix() const ;
};