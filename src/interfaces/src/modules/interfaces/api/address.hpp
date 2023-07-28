#pragma once

#include "interface.hpp"
#include <string>

class Address {
public:
    friend class Interface; ///< Allow Interface class to use the private constructor.

    /**
     * @breif Convert the address to string.
     */
    std::string toString() const;

private:
    using NlAddr = struct nl_addr; ///< NL address type alias;
    using NlAddrDeleter = NlDeleter<NlAddr>; ///< Deleter type alias.
    using NlAddrPtr = std::unique_ptr<NlAddr, NlAddrDeleter>; ///< Unique pointer type alias.

    /**
     * @brief Private constructor accessible only to friend classes. Stores a reference to nl_addr for later access of address members.
     */
    Address(struct nl_addr* addr);

    NlAddrPtr m_addr; ///< Addr reference.
};
