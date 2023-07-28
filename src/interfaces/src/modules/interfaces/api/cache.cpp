#include "cache.hpp"
#include "modules/interfaces/api/nl.hpp"
#include "netlink/cache.h"

/**
 * @brief Private constructor accessible only to friend classes. Stores a reference to nl_cache for later access of cache data.
 */
template <typename T, typename NetlinkT>
Cache<T, NetlinkT>::Cache(struct nl_cache* cache)
    : m_cache(cache, NlEmptyDeleter<NlCache>)
{
}

/**
 * @brief Get first element in the cache.
 */
template <typename T, typename NetlinkT> T Cache<T, NetlinkT>::getFirst()
{
    m_lastRef = nl_cache_get_first(m_cache.get());
    return T(m_lastRef);
}

/**
 * @brief Get next element in the cache.
 */
template <typename T, typename NetlinkT> std::optional<T> Cache<T, NetlinkT>::getNext()
{
    m_lastRef = nl_cache_get_next(m_lastRef);
    return T(m_lastRef);
}
