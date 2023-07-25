#include "address.hpp"
#include "netlink/addr.h"
#include <stdexcept>

/**
 * @brief Private constructor accessible only to friend classes. Stores a reference to nl_addr for later access of address members.
 */
Address::Address(struct nl_addr* addr)
    : m_addr(addr)
{
}

/**
 * @breif Convert the address to string.
 */
std::string Address::toString() const
{
    char buffer[100] = { 0 };
    void* error = 0;

    error = nl_addr2str(m_addr.get(), buffer, sizeof(buffer));
    if (error == nullptr) {
        throw std::runtime_error("Unable to convert address to text format");
    }

    return std::string(buffer);
}
