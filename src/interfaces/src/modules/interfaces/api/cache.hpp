#pragma once

#include "netlink/cache.h"
#include "nl.hpp"

#include <optional>

template <typename T> class CacheRef {
public:
    friend class NlContext; ///< Allow NlContext to use the private constructor.

    /**
     * @brief Return begin pointer.
     */
    auto begin() { return m_objects.begin(); }

    /**
     * @brief Return begin pointer.
     */
    auto end() { return m_objects.end(); }

private:
    using NlObject = struct nl_object;
    using NlCache = struct nl_cache; ///< NL cache type alias;
    using NlCacheDeleter = NlDeleter<NlCache>; ///< Deleter type alias.
    using NlCachePtr = std::unique_ptr<NlCache, NlCacheDeleter>; ///< Unique pointer type alias.
    using NlSocketPtr = std::unique_ptr<struct nl_sock, NlDeleter<struct nl_sock>>; //Socket type alias.

    /**
     * @brief Private constructor accessible only to friend classes. Stores a reference to nl_cache for later access of cache data.
     */
    CacheRef(struct nl_cache* cache, struct nl_sock* socket)
        : m_cache(cache, NlEmptyDeleter<NlCache>)
        , m_socket(socket,NlEmptyDeleter<struct nl_sock>)
    {
        auto iter = nl_cache_get_first(cache);

        while (iter) {
            m_objects.push_back(T(iter,m_socket.get()));
            iter = nl_cache_get_next(iter);
        }
    }

    NlCachePtr m_cache; ///< Cache reference.
    std::vector<T> m_objects; ///< Cached object references.
    NlSocketPtr m_socket; ///socket reference.
};
