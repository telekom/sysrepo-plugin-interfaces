#include "operational.h"
#include "libyang/tree_data.h"
#include "netlink/addr.h"
#include "netlink/cache.h"
#include "netlink/object.h"
#include "netlink/route/tc.h"
#include "netlink/socket.h"
#include "plugin/common.h"
#include "plugin/context.h"
#include "plugin/data/interfaces/interface/state_hash.h"
#include "plugin/ly_tree.h"
#include "plugin/types.h"
#include "srpc/common.h"
#include "sysrepo_types.h"

#include <assert.h>
#include <libyang/libyang.h>
#include <pthread.h>
#include <srpc.h>
#include <sysrepo.h>
#include <sysrepo/xpath.h>

#include <linux/netdevice.h>
#include <netlink/route/link.h>
#include <netlink/route/qdisc.h>
#include <sys/sysinfo.h>

static struct rtnl_link* interfaces_get_current_link(interfaces_ctx_t* ctx, sr_session_ctx_t* session, const char* xpath);
static int interfaces_extract_interface_name(sr_session_ctx_t* session, const char* xpath, char* buffer, size_t buffer_size);
static int interfaces_get_system_boot_time(char* buffer, size_t buffer_size);

int interfaces_subscription_operational_interfaces_interface_admin_status(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    void* error_ptr = NULL;
    const struct ly_ctx* ly_ctx = NULL;

    if (*parent == NULL) {
        ly_ctx = sr_acquire_context(sr_session_get_connection(session));
        if (ly_ctx == NULL) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "sr_acquire_context() failed");
            goto error_out;
        }
    }

    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}

int interfaces_subscription_operational_interfaces_interface_oper_status(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    void* error_ptr = NULL;

    // context
    const struct ly_ctx* ly_ctx = NULL;
    interfaces_ctx_t* ctx = private_data;

    // libnl
    struct rtnl_link* link = NULL;

    char xpath_buffer[PATH_MAX] = { 0 };

    const char* operstate_map[] = {
        [IF_OPER_UNKNOWN] = "unknown",
        [IF_OPER_NOTPRESENT] = "not-present",
        [IF_OPER_DOWN] = "down",
        [IF_OPER_LOWERLAYERDOWN] = "lower-layer-down",
        [IF_OPER_TESTING] = "testing",
        [IF_OPER_DORMANT] = "dormant",
        [IF_OPER_UP] = "up",
    };

    // there needs to be an allocated link cache in memory
    assert(*parent != NULL);
    assert(strcmp(LYD_NAME(*parent), "interface") == 0);

    // get node xpath
    SRPC_SAFE_CALL_PTR(error_ptr, lyd_path(*parent, LYD_PATH_STD, xpath_buffer, sizeof(xpath_buffer)), error_out);

    // get link
    SRPC_SAFE_CALL_PTR(link, interfaces_get_current_link(ctx, session, xpath_buffer), error_out);

    // get oper status
    const uint8_t oper_status = rtnl_link_get_operstate(link);
    const char* oper_status_str = operstate_map[oper_status];

    SRPLG_LOG_INF(PLUGIN_NAME, "oper-status(%s) = %s", rtnl_link_get_name(link), oper_status_str);

    // add oper-status node
    SRPC_SAFE_CALL_ERR(error, interfaces_ly_tree_create_interfaces_interface_oper_status(ly_ctx, *parent, oper_status_str), error_out);

    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}

int interfaces_subscription_operational_interfaces_interface_last_change(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    void* error_ptr = NULL;

    // context
    const struct ly_ctx* ly_ctx = NULL;
    interfaces_ctx_t* ctx = private_data;
    interfaces_state_changes_ctx_t* state_ctx = &ctx->state_ctx;
    interfaces_interface_state_hash_element_t* state_element = NULL;

    // libnl
    struct rtnl_link* link = NULL;

    // buffers
    char last_change_buffer[100] = { 0 };
    char xpath_buffer[PATH_MAX] = { 0 };

    // there needs to be an allocated link cache in memory
    assert(*parent != NULL);
    assert(strcmp(LYD_NAME(*parent), "interface") == 0);

    // get node xpath
    SRPC_SAFE_CALL_PTR(error_ptr, lyd_path(*parent, LYD_PATH_STD, xpath_buffer, sizeof(xpath_buffer)), error_out);

    // get link
    SRPC_SAFE_CALL_PTR(link, interfaces_get_current_link(ctx, session, xpath_buffer), error_out);

    // synchronization
    pthread_mutex_lock(&state_ctx->state_hash_mutex);

    // get last change
    SRPC_SAFE_CALL_PTR(state_element, interfaces_interface_state_hash_get(state_ctx->state_hash, rtnl_link_get_name(link)), error_out);

    const time_t last_change = state_element->state.last_change;
    struct tm* last_change_tm = localtime(&last_change);

    size_t written = strftime(last_change_buffer, sizeof(last_change_buffer), "%FT%TZ", last_change_tm);
    if (written == 0) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "strftime() failed");
        goto error_out;
    }

    SRPLG_LOG_INF(PLUGIN_NAME, "last-change(%s) = %s", rtnl_link_get_name(link), last_change_buffer);

    // add oper-status node
    SRPC_SAFE_CALL_ERR(error, interfaces_ly_tree_create_interfaces_interface_last_change(ly_ctx, *parent, last_change_buffer), error_out);

    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    pthread_mutex_unlock(&state_ctx->state_hash_mutex);

    return error;
}

int interfaces_subscription_operational_interfaces_interface_if_index(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    void* error_ptr = NULL;

    // context
    const struct ly_ctx* ly_ctx = NULL;
    interfaces_ctx_t* ctx = private_data;

    // buffers
    char ifindex_buffer[100] = { 0 };
    char xpath_buffer[PATH_MAX] = { 0 };

    // libnl
    struct rtnl_link* link = NULL;

    // there needs to be an allocated link cache in memory
    assert(*parent != NULL);
    assert(strcmp(LYD_NAME(*parent), "interface") == 0);

    // get node xpath
    SRPC_SAFE_CALL_PTR(error_ptr, lyd_path(*parent, LYD_PATH_STD, xpath_buffer, sizeof(xpath_buffer)), error_out);

    // get link
    SRPC_SAFE_CALL_PTR(link, interfaces_get_current_link(ctx, session, xpath_buffer), error_out);

    // get if-index
    const int ifindex = rtnl_link_get_ifindex(link);

    error = snprintf(ifindex_buffer, sizeof(ifindex_buffer), "%d", ifindex);
    if (error < 0) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() failed (%d)", error);
        goto error_out;
    }

    SRPLG_LOG_INF(PLUGIN_NAME, "if-index(%s) = %s", rtnl_link_get_name(link), ifindex_buffer);

    // add ifindex node
    SRPC_SAFE_CALL_ERR(error, interfaces_ly_tree_create_interfaces_interface_if_index(ly_ctx, *parent, ifindex_buffer), error_out);

    error = 0;
    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}

int interfaces_subscription_operational_interfaces_interface_phys_address(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    void* error_ptr = NULL;

    // context
    const struct ly_ctx* ly_ctx = NULL;
    interfaces_ctx_t* ctx = private_data;

    // buffers
    char phys_address_buffer[100] = { 0 };
    char xpath_buffer[PATH_MAX] = { 0 };

    // libnl
    struct rtnl_link* link = NULL;
    struct nl_addr* addr = NULL;

    // there needs to be an allocated link cache in memory
    assert(*parent != NULL);
    assert(strcmp(LYD_NAME(*parent), "interface") == 0);

    // get node xpath
    SRPC_SAFE_CALL_PTR(error_ptr, lyd_path(*parent, LYD_PATH_STD, xpath_buffer, sizeof(xpath_buffer)), error_out);

    // get link
    SRPC_SAFE_CALL_PTR(link, interfaces_get_current_link(ctx, session, xpath_buffer), error_out);

    // get phys-address
    SRPC_SAFE_CALL_PTR(addr, rtnl_link_get_addr(link), error_out);
    SRPC_SAFE_CALL_PTR(error_ptr, nl_addr2str(addr, phys_address_buffer, sizeof(phys_address_buffer)), error_out);

    SRPLG_LOG_INF(PLUGIN_NAME, "phys-address(%s) = %s", rtnl_link_get_name(link), phys_address_buffer);

    // add phys-address node
    SRPC_SAFE_CALL_ERR(error, interfaces_ly_tree_create_interfaces_interface_phys_address(ly_ctx, *parent, phys_address_buffer), error_out);

    error = 0;
    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}

int interfaces_subscription_operational_interfaces_interface_higher_layer_if(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    void* error_ptr = NULL;

    // context
    const struct ly_ctx* ly_ctx = NULL;
    interfaces_ctx_t* ctx = private_data;
    interfaces_nl_ctx_t* nl_ctx = &ctx->nl_ctx;

    char xpath_buffer[PATH_MAX] = { 0 };

    // libnl
    struct rtnl_link* link = NULL;
    struct rtnl_link* master_link = NULL;

    assert(*parent != NULL);
    assert(strcmp(LYD_NAME(*parent), "interface") == 0);

    // get node xpath
    SRPC_SAFE_CALL_PTR(error_ptr, lyd_path(*parent, LYD_PATH_STD, xpath_buffer, sizeof(xpath_buffer)), error_out);

    // get link
    SRPC_SAFE_CALL_PTR(link, interfaces_get_current_link(ctx, session, xpath_buffer), error_out);

    int master_if_index = rtnl_link_get_master(link);
    while (master_if_index) {
        master_link = rtnl_link_get(nl_ctx->link_cache, master_if_index);
        const char* master_name = rtnl_link_get_name(master_link);

        SRPLG_LOG_INF(PLUGIN_NAME, "higher-layer-if(%s) = %s", rtnl_link_get_name(link), master_name);

        // add higher-layer-if
        SRPC_SAFE_CALL_ERR(error, interfaces_ly_tree_create_interfaces_interface_higher_layer_if(ly_ctx, *parent, master_name), error_out);

        // go one layer higher
        master_if_index = rtnl_link_get_master(master_link);
    }

    error = 0;
    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}

int interfaces_subscription_operational_interfaces_interface_lower_layer_if(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    void* error_ptr = NULL;

    // context
    const struct ly_ctx* ly_ctx = NULL;
    interfaces_ctx_t* ctx = private_data;
    interfaces_nl_ctx_t* nl_ctx = &ctx->nl_ctx;

    char xpath_buffer[PATH_MAX] = { 0 };

    // libnl
    struct rtnl_link* link = NULL;
    struct rtnl_link* master_link = NULL;

    assert(*parent != NULL);
    assert(strcmp(LYD_NAME(*parent), "interface") == 0);

    // get node xpath
    SRPC_SAFE_CALL_PTR(error_ptr, lyd_path(*parent, LYD_PATH_STD, xpath_buffer, sizeof(xpath_buffer)), error_out);

    // get link
    SRPC_SAFE_CALL_PTR(link, interfaces_get_current_link(ctx, session, xpath_buffer), error_out);

    // iterate over all links and check for ones which have a master equal to the current link
    struct nl_cache* link_cache = nl_ctx->link_cache;
    struct rtnl_link* link_iter = (struct rtnl_link*)nl_cache_get_first(link_cache);

    while (link_iter) {
        int master_if_index = rtnl_link_get_master(link);
        while (master_if_index) {
            master_link = rtnl_link_get(nl_ctx->link_cache, master_if_index);
            const char* master_name = rtnl_link_get_name(master_link);

            if (!strcmp(master_name, rtnl_link_get_name(link))) {
                // current link is the higher layer interface of the iterated link
                SRPLG_LOG_INF(PLUGIN_NAME, "lower-layer-if(%s) = %s", rtnl_link_get_name(link), rtnl_link_get_name(link_iter));

                // add lower-layer-if
                SRPC_SAFE_CALL_ERR(error, interfaces_ly_tree_create_interfaces_interface_lower_layer_if(ly_ctx, *parent, rtnl_link_get_name(link_iter)), error_out);

                // found in the master list - continue checking other interfaces
                break;
            }

            // go one layer higher
            master_if_index = rtnl_link_get_master(master_link);
        }

        link_iter = (struct rtnl_link*)nl_cache_get_next((struct nl_object*)link_iter);
    }

    error = 0;
    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}

int interfaces_subscription_operational_interfaces_interface_speed(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    void* error_ptr = NULL;

    // context
    const struct ly_ctx* ly_ctx = NULL;
    interfaces_ctx_t* ctx = private_data;

    // buffers
    char speed_buffer[100] = { 0 };
    char xpath_buffer[PATH_MAX] = { 0 };

    // libnl
    struct rtnl_link* link = NULL;
    struct rtnl_qdisc* qdisc = NULL;
    struct rtnl_tc* tc = NULL;

    // there needs to be an allocated link cache in memory
    assert(*parent != NULL);
    assert(strcmp(LYD_NAME(*parent), "interface") == 0);

    // get node xpath
    SRPC_SAFE_CALL_PTR(error_ptr, lyd_path(*parent, LYD_PATH_STD, xpath_buffer, sizeof(xpath_buffer)), error_out);

    // get link
    SRPC_SAFE_CALL_PTR(link, interfaces_get_current_link(ctx, session, xpath_buffer), error_out);

    qdisc = rtnl_qdisc_alloc();

    // setup traffic control
    tc = TC_CAST(qdisc);
    rtnl_tc_set_link(tc, link);

    // get speed
    const uint64_t speed = rtnl_tc_get_stat(tc, RTNL_TC_RATE_BPS);
    error = snprintf(speed_buffer, sizeof(speed_buffer), "%lu", speed);
    if (error < 0) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() failed (%d)", error);
        goto error_out;
    }

    SRPLG_LOG_INF(PLUGIN_NAME, "speed(%s) = %s", rtnl_link_get_name(link), speed_buffer);

    // add phys-address node
    SRPC_SAFE_CALL_ERR(error, interfaces_ly_tree_create_interfaces_interface_speed(ly_ctx, *parent, speed_buffer), error_out);

    error = 0;
    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    rtnl_qdisc_put(qdisc);

    return error;
}

int interfaces_subscription_operational_interfaces_interface_statistics_discontinuity_time(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    void* error_ptr = NULL;

    // context
    const struct ly_ctx* ly_ctx = NULL;
    interfaces_ctx_t* ctx = private_data;

    // buffers
    char discontinuity_time_buffer[100] = { 0 };
    char xpath_buffer[PATH_MAX] = { 0 };

    // libnl
    struct rtnl_link* link = NULL;

    // there needs to be an allocated link cache in memory
    assert(*parent != NULL);
    assert(strcmp(LYD_NAME(*parent), "statistics") == 0);

    // get node xpath
    SRPC_SAFE_CALL_PTR(error_ptr, lyd_path(*parent, LYD_PATH_STD, xpath_buffer, sizeof(xpath_buffer)), error_out);

    // get link
    SRPC_SAFE_CALL_PTR(link, interfaces_get_current_link(ctx, session, xpath_buffer), error_out);

    // get boot time as discontinuity time
    SRPC_SAFE_CALL_ERR(error, interfaces_get_system_boot_time(discontinuity_time_buffer, sizeof(discontinuity_time_buffer)), error_out);

    SRPLG_LOG_INF(PLUGIN_NAME, "discontinuity-time(%s) = %s", rtnl_link_get_name(link), discontinuity_time_buffer);

    SRPC_SAFE_CALL_ERR(error, interfaces_ly_tree_create_interfaces_interface_statistics_discontinuity_time(ly_ctx, *parent, discontinuity_time_buffer), error_out);

    error = 0;
    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}

int interfaces_subscription_operational_interfaces_interface_statistics_in_octets(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    void* error_ptr = NULL;

    // context
    const struct ly_ctx* ly_ctx = NULL;
    interfaces_ctx_t* ctx = private_data;

    // buffers
    char in_octets_buffer[100] = { 0 };
    char xpath_buffer[PATH_MAX] = { 0 };

    // libnl
    struct rtnl_link* link = NULL;

    // there needs to be an allocated link cache in memory
    assert(*parent != NULL);
    assert(strcmp(LYD_NAME(*parent), "statistics") == 0);

    // get node xpath
    SRPC_SAFE_CALL_PTR(error_ptr, lyd_path(*parent, LYD_PATH_STD, xpath_buffer, sizeof(xpath_buffer)), error_out);

    // get link
    SRPC_SAFE_CALL_PTR(link, interfaces_get_current_link(ctx, session, xpath_buffer), error_out);

    const uint64_t in_octets = rtnl_link_get_stat(link, RTNL_LINK_RX_BYTES);

    error = snprintf(in_octets_buffer, sizeof(in_octets_buffer), "%lu", in_octets);
    if (error < 0) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() failed (%d)", error);
        goto error_out;
    }

    SRPLG_LOG_INF(PLUGIN_NAME, "in-octets(%s) = %s", rtnl_link_get_name(link), in_octets_buffer);

    SRPC_SAFE_CALL_ERR(error, interfaces_ly_tree_create_interfaces_interface_statistics_in_octets(ly_ctx, *parent, in_octets_buffer), error_out);

    error = 0;
    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}

int interfaces_subscription_operational_interfaces_interface_statistics_in_unicast_pkts(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    void* error_ptr = NULL;

    // context
    const struct ly_ctx* ly_ctx = NULL;
    interfaces_ctx_t* ctx = private_data;

    // buffers
    char in_unicast_pkts_buffer[100] = { 0 };
    char xpath_buffer[PATH_MAX] = { 0 };

    // libnl
    struct rtnl_link* link = NULL;

    // there needs to be an allocated link cache in memory
    assert(*parent != NULL);
    assert(strcmp(LYD_NAME(*parent), "statistics") == 0);

    // get node xpath
    SRPC_SAFE_CALL_PTR(error_ptr, lyd_path(*parent, LYD_PATH_STD, xpath_buffer, sizeof(xpath_buffer)), error_out);

    // get link
    SRPC_SAFE_CALL_PTR(link, interfaces_get_current_link(ctx, session, xpath_buffer), error_out);

    const uint64_t in_pkts = rtnl_link_get_stat(link, RTNL_LINK_RX_PACKETS);
    const uint64_t in_broadcast_pkts = rtnl_link_get_stat(link, RTNL_LINK_IP6_INBCASTPKTS);
    const uint64_t in_multicast_pkts = rtnl_link_get_stat(link, RTNL_LINK_IP6_INMCASTPKTS);
    const uint64_t in_unicast_pkts = in_pkts - in_broadcast_pkts - in_multicast_pkts;

    error = snprintf(in_unicast_pkts_buffer, sizeof(in_unicast_pkts_buffer), "%lu", in_unicast_pkts);
    if (error < 0) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() failed (%d)", error);
        goto error_out;
    }

    SRPLG_LOG_INF(PLUGIN_NAME, "in-unicast-pkts(%s) = %s", rtnl_link_get_name(link), in_unicast_pkts_buffer);

    SRPC_SAFE_CALL_ERR(error, interfaces_ly_tree_create_interfaces_interface_statistics_in_unicast_pkts(ly_ctx, *parent, in_unicast_pkts_buffer), error_out);

    error = 0;
    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}

int interfaces_subscription_operational_interfaces_interface_statistics_in_broadcast_pkts(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    void* error_ptr = NULL;

    // context
    const struct ly_ctx* ly_ctx = NULL;
    interfaces_ctx_t* ctx = private_data;

    // buffers
    char in_broadcast_pkts_buffer[100] = { 0 };
    char xpath_buffer[PATH_MAX] = { 0 };

    // libnl
    struct rtnl_link* link = NULL;

    // there needs to be an allocated link cache in memory
    assert(*parent != NULL);
    assert(strcmp(LYD_NAME(*parent), "statistics") == 0);

    // get node xpath
    SRPC_SAFE_CALL_PTR(error_ptr, lyd_path(*parent, LYD_PATH_STD, xpath_buffer, sizeof(xpath_buffer)), error_out);

    // get link
    SRPC_SAFE_CALL_PTR(link, interfaces_get_current_link(ctx, session, xpath_buffer), error_out);

    const uint64_t in_broadcast_pkts = rtnl_link_get_stat(link, RTNL_LINK_IP6_INBCASTPKTS);

    error = snprintf(in_broadcast_pkts_buffer, sizeof(in_broadcast_pkts_buffer), "%lu", in_broadcast_pkts);
    if (error < 0) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() failed (%d)", error);
        goto error_out;
    }

    SRPLG_LOG_INF(PLUGIN_NAME, "in-broadcast-pkts(%s) = %s", rtnl_link_get_name(link), in_broadcast_pkts_buffer);

    SRPC_SAFE_CALL_ERR(error, interfaces_ly_tree_create_interfaces_interface_statistics_in_broadcast_pkts(ly_ctx, *parent, in_broadcast_pkts_buffer), error_out);

    error = 0;
    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}

int interfaces_subscription_operational_interfaces_interface_statistics_in_multicast_pkts(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    void* error_ptr = NULL;

    // context
    const struct ly_ctx* ly_ctx = NULL;
    interfaces_ctx_t* ctx = private_data;

    // buffers
    char in_multicast_pkts_buffer[100] = { 0 };
    char xpath_buffer[PATH_MAX] = { 0 };

    // libnl
    struct rtnl_link* link = NULL;

    // there needs to be an allocated link cache in memory
    assert(*parent != NULL);
    assert(strcmp(LYD_NAME(*parent), "statistics") == 0);

    // get node xpath
    SRPC_SAFE_CALL_PTR(error_ptr, lyd_path(*parent, LYD_PATH_STD, xpath_buffer, sizeof(xpath_buffer)), error_out);

    // get link
    SRPC_SAFE_CALL_PTR(link, interfaces_get_current_link(ctx, session, xpath_buffer), error_out);

    const uint64_t in_multicast_pkts = rtnl_link_get_stat(link, RTNL_LINK_IP6_INMCASTPKTS);

    error = snprintf(in_multicast_pkts_buffer, sizeof(in_multicast_pkts_buffer), "%lu", in_multicast_pkts);
    if (error < 0) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() failed (%d)", error);
        goto error_out;
    }

    SRPLG_LOG_INF(PLUGIN_NAME, "in-multicast-pkts(%s) = %s", rtnl_link_get_name(link), in_multicast_pkts_buffer);

    SRPC_SAFE_CALL_ERR(error, interfaces_ly_tree_create_interfaces_interface_statistics_in_multicast_pkts(ly_ctx, *parent, in_multicast_pkts_buffer), error_out);

    error = 0;
    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}

int interfaces_subscription_operational_interfaces_interface_statistics_in_discards(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    void* error_ptr = NULL;

    // context
    const struct ly_ctx* ly_ctx = NULL;
    interfaces_ctx_t* ctx = private_data;

    // buffers
    char in_discards_buffer[100] = { 0 };
    char xpath_buffer[PATH_MAX] = { 0 };

    // libnl
    struct rtnl_link* link = NULL;

    // there needs to be an allocated link cache in memory
    assert(*parent != NULL);
    assert(strcmp(LYD_NAME(*parent), "statistics") == 0);

    // get node xpath
    SRPC_SAFE_CALL_PTR(error_ptr, lyd_path(*parent, LYD_PATH_STD, xpath_buffer, sizeof(xpath_buffer)), error_out);

    // get link
    SRPC_SAFE_CALL_PTR(link, interfaces_get_current_link(ctx, session, xpath_buffer), error_out);

    const uint32_t in_discards = (uint32_t)rtnl_link_get_stat(link, RTNL_LINK_RX_DROPPED);

    error = snprintf(in_discards_buffer, sizeof(in_discards_buffer), "%u", in_discards);
    if (error < 0) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() failed (%d)", error);
        goto error_out;
    }

    SRPLG_LOG_INF(PLUGIN_NAME, "in-discards(%s) = %s", rtnl_link_get_name(link), in_discards_buffer);

    SRPC_SAFE_CALL_ERR(error, interfaces_ly_tree_create_interfaces_interface_statistics_in_discards(ly_ctx, *parent, in_discards_buffer), error_out);

    error = 0;
    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}

int interfaces_subscription_operational_interfaces_interface_statistics_in_errors(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    void* error_ptr = NULL;

    // context
    const struct ly_ctx* ly_ctx = NULL;
    interfaces_ctx_t* ctx = private_data;

    // buffers
    char in_errors_buffer[100] = { 0 };
    char xpath_buffer[PATH_MAX] = { 0 };

    // libnl
    struct rtnl_link* link = NULL;

    // there needs to be an allocated link cache in memory
    assert(*parent != NULL);
    assert(strcmp(LYD_NAME(*parent), "statistics") == 0);

    // get node xpath
    SRPC_SAFE_CALL_PTR(error_ptr, lyd_path(*parent, LYD_PATH_STD, xpath_buffer, sizeof(xpath_buffer)), error_out);

    // get link
    SRPC_SAFE_CALL_PTR(link, interfaces_get_current_link(ctx, session, xpath_buffer), error_out);

    const uint32_t in_errors = (uint32_t)rtnl_link_get_stat(link, RTNL_LINK_RX_ERRORS);

    error = snprintf(in_errors_buffer, sizeof(in_errors_buffer), "%u", in_errors);
    if (error < 0) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() failed (%d)", error);
        goto error_out;
    }

    SRPLG_LOG_INF(PLUGIN_NAME, "in-errors(%s) = %s", rtnl_link_get_name(link), in_errors_buffer);

    SRPC_SAFE_CALL_ERR(error, interfaces_ly_tree_create_interfaces_interface_statistics_in_errors(ly_ctx, *parent, in_errors_buffer), error_out);

    error = 0;
    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}

int interfaces_subscription_operational_interfaces_interface_statistics_in_unknown_protos(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    void* error_ptr = NULL;

    // context
    const struct ly_ctx* ly_ctx = NULL;
    interfaces_ctx_t* ctx = private_data;

    // buffers
    char in_unknown_protos_buffer[100] = { 0 };
    char xpath_buffer[PATH_MAX] = { 0 };

    // libnl
    struct rtnl_link* link = NULL;

    // there needs to be an allocated link cache in memory
    assert(*parent != NULL);
    assert(strcmp(LYD_NAME(*parent), "statistics") == 0);

    // get node xpath
    SRPC_SAFE_CALL_PTR(error_ptr, lyd_path(*parent, LYD_PATH_STD, xpath_buffer, sizeof(xpath_buffer)), error_out);

    // get link
    SRPC_SAFE_CALL_PTR(link, interfaces_get_current_link(ctx, session, xpath_buffer), error_out);

    const uint32_t in_unknown_protos = (uint32_t)rtnl_link_get_stat(link, RTNL_LINK_IP6_INUNKNOWNPROTOS);

    error = snprintf(in_unknown_protos_buffer, sizeof(in_unknown_protos_buffer), "%u", in_unknown_protos);
    if (error < 0) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() failed (%d)", error);
        goto error_out;
    }

    SRPLG_LOG_INF(PLUGIN_NAME, "in-unknown-protos(%s) = %s", rtnl_link_get_name(link), in_unknown_protos_buffer);

    SRPC_SAFE_CALL_ERR(error, interfaces_ly_tree_create_interfaces_interface_statistics_in_unknown_protos(ly_ctx, *parent, in_unknown_protos_buffer), error_out);

    error = 0;
    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}

int interfaces_subscription_operational_interfaces_interface_statistics_out_octets(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    void* error_ptr = NULL;

    // context
    const struct ly_ctx* ly_ctx = NULL;
    interfaces_ctx_t* ctx = private_data;

    // buffers
    char out_octets_buffer[100] = { 0 };
    char xpath_buffer[PATH_MAX] = { 0 };

    // libnl
    struct rtnl_link* link = NULL;

    // there needs to be an allocated link cache in memory
    assert(*parent != NULL);
    assert(strcmp(LYD_NAME(*parent), "statistics") == 0);

    // get node xpath
    SRPC_SAFE_CALL_PTR(error_ptr, lyd_path(*parent, LYD_PATH_STD, xpath_buffer, sizeof(xpath_buffer)), error_out);

    // get link
    SRPC_SAFE_CALL_PTR(link, interfaces_get_current_link(ctx, session, xpath_buffer), error_out);

    const uint64_t out_octets = rtnl_link_get_stat(link, RTNL_LINK_TX_BYTES);

    error = snprintf(out_octets_buffer, sizeof(out_octets_buffer), "%lu", out_octets);
    if (error < 0) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() failed (%d)", error);
        goto error_out;
    }

    SRPLG_LOG_INF(PLUGIN_NAME, "out-octets(%s) = %s", rtnl_link_get_name(link), out_octets_buffer);

    SRPC_SAFE_CALL_ERR(error, interfaces_ly_tree_create_interfaces_interface_statistics_out_octets(ly_ctx, *parent, out_octets_buffer), error_out);

    error = 0;
    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}

int interfaces_subscription_operational_interfaces_interface_statistics_out_unicast_pkts(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    void* error_ptr = NULL;

    // context
    const struct ly_ctx* ly_ctx = NULL;
    interfaces_ctx_t* ctx = private_data;

    // buffers
    char out_unicast_pkts_buffer[100] = { 0 };
    char xpath_buffer[PATH_MAX] = { 0 };

    // libnl
    struct rtnl_link* link = NULL;

    // there needs to be an allocated link cache in memory
    assert(*parent != NULL);
    assert(strcmp(LYD_NAME(*parent), "statistics") == 0);

    // get node xpath
    SRPC_SAFE_CALL_PTR(error_ptr, lyd_path(*parent, LYD_PATH_STD, xpath_buffer, sizeof(xpath_buffer)), error_out);

    // get link
    SRPC_SAFE_CALL_PTR(link, interfaces_get_current_link(ctx, session, xpath_buffer), error_out);

    const uint64_t out_pkts = rtnl_link_get_stat(link, RTNL_LINK_TX_PACKETS);
    const uint64_t out_broadcast_pkts = rtnl_link_get_stat(link, RTNL_LINK_IP6_OUTBCASTPKTS);
    const uint64_t out_multicast_pkts = rtnl_link_get_stat(link, RTNL_LINK_IP6_OUTMCASTPKTS);
    const uint64_t out_unicast_pkts = out_pkts - out_broadcast_pkts - out_multicast_pkts;

    error = snprintf(out_unicast_pkts_buffer, sizeof(out_unicast_pkts_buffer), "%lu", out_unicast_pkts);
    if (error < 0) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() failed (%d)", error);
        goto error_out;
    }

    SRPLG_LOG_INF(PLUGIN_NAME, "out-unicast-pkts(%s) = %s", rtnl_link_get_name(link), out_unicast_pkts_buffer);

    SRPC_SAFE_CALL_ERR(error, interfaces_ly_tree_create_interfaces_interface_statistics_out_unicast_pkts(ly_ctx, *parent, out_unicast_pkts_buffer), error_out);

    error = 0;
    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}

int interfaces_subscription_operational_interfaces_interface_statistics_out_broadcast_pkts(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    void* error_ptr = NULL;

    // context
    const struct ly_ctx* ly_ctx = NULL;
    interfaces_ctx_t* ctx = private_data;

    // buffers
    char out_broadcast_pkts_buffer[100] = { 0 };
    char xpath_buffer[PATH_MAX] = { 0 };

    // libnl
    struct rtnl_link* link = NULL;

    // there needs to be an allocated link cache in memory
    assert(*parent != NULL);
    assert(strcmp(LYD_NAME(*parent), "statistics") == 0);

    // get node xpath
    SRPC_SAFE_CALL_PTR(error_ptr, lyd_path(*parent, LYD_PATH_STD, xpath_buffer, sizeof(xpath_buffer)), error_out);

    // get link
    SRPC_SAFE_CALL_PTR(link, interfaces_get_current_link(ctx, session, xpath_buffer), error_out);

    const uint64_t out_broadcast_pkts = rtnl_link_get_stat(link, RTNL_LINK_IP6_OUTBCASTPKTS);

    error = snprintf(out_broadcast_pkts_buffer, sizeof(out_broadcast_pkts_buffer), "%lu", out_broadcast_pkts);
    if (error < 0) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() failed (%d)", error);
        goto error_out;
    }

    SRPLG_LOG_INF(PLUGIN_NAME, "out-broadcast-pkts(%s) = %s", rtnl_link_get_name(link), out_broadcast_pkts_buffer);

    SRPC_SAFE_CALL_ERR(error, interfaces_ly_tree_create_interfaces_interface_statistics_out_broadcast_pkts(ly_ctx, *parent, out_broadcast_pkts_buffer), error_out);

    error = 0;
    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}

int interfaces_subscription_operational_interfaces_interface_statistics_out_multicast_pkts(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    void* error_ptr = NULL;

    // context
    const struct ly_ctx* ly_ctx = NULL;
    interfaces_ctx_t* ctx = private_data;

    // buffers
    char out_multicast_pkts_buffer[100] = { 0 };
    char xpath_buffer[PATH_MAX] = { 0 };

    // libnl
    struct rtnl_link* link = NULL;

    // there needs to be an allocated link cache in memory
    assert(*parent != NULL);
    assert(strcmp(LYD_NAME(*parent), "statistics") == 0);

    // get node xpath
    SRPC_SAFE_CALL_PTR(error_ptr, lyd_path(*parent, LYD_PATH_STD, xpath_buffer, sizeof(xpath_buffer)), error_out);

    // get link
    SRPC_SAFE_CALL_PTR(link, interfaces_get_current_link(ctx, session, xpath_buffer), error_out);

    const uint64_t out_multicast_pkts = rtnl_link_get_stat(link, RTNL_LINK_IP6_OUTMCASTPKTS);

    error = snprintf(out_multicast_pkts_buffer, sizeof(out_multicast_pkts_buffer), "%lu", out_multicast_pkts);
    if (error < 0) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() failed (%d)", error);
        goto error_out;
    }

    SRPLG_LOG_INF(PLUGIN_NAME, "out-multicast-pkts(%s) = %s", rtnl_link_get_name(link), out_multicast_pkts_buffer);

    SRPC_SAFE_CALL_ERR(error, interfaces_ly_tree_create_interfaces_interface_statistics_out_multicast_pkts(ly_ctx, *parent, out_multicast_pkts_buffer), error_out);

    error = 0;
    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}

int interfaces_subscription_operational_interfaces_interface_statistics_out_discards(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    void* error_ptr = NULL;

    // context
    const struct ly_ctx* ly_ctx = NULL;
    interfaces_ctx_t* ctx = private_data;

    // buffers
    char out_discards_buffer[100] = { 0 };
    char xpath_buffer[PATH_MAX] = { 0 };

    // libnl
    struct rtnl_link* link = NULL;

    // there needs to be an allocated link cache in memory
    assert(*parent != NULL);
    assert(strcmp(LYD_NAME(*parent), "statistics") == 0);

    // get node xpath
    SRPC_SAFE_CALL_PTR(error_ptr, lyd_path(*parent, LYD_PATH_STD, xpath_buffer, sizeof(xpath_buffer)), error_out);

    // get link
    SRPC_SAFE_CALL_PTR(link, interfaces_get_current_link(ctx, session, xpath_buffer), error_out);

    const uint64_t out_discards = rtnl_link_get_stat(link, RTNL_LINK_TX_DROPPED);

    error = snprintf(out_discards_buffer, sizeof(out_discards_buffer), "%lu", out_discards);
    if (error < 0) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() failed (%d)", error);
        goto error_out;
    }

    SRPLG_LOG_INF(PLUGIN_NAME, "out-discards(%s) = %s", rtnl_link_get_name(link), out_discards_buffer);

    SRPC_SAFE_CALL_ERR(error, interfaces_ly_tree_create_interfaces_interface_statistics_out_discards(ly_ctx, *parent, out_discards_buffer), error_out);

    error = 0;
    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}

int interfaces_subscription_operational_interfaces_interface_statistics_out_errors(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    void* error_ptr = NULL;

    // context
    const struct ly_ctx* ly_ctx = NULL;
    interfaces_ctx_t* ctx = private_data;

    // buffers
    char out_errors_buffer[100] = { 0 };
    char xpath_buffer[PATH_MAX] = { 0 };

    // libnl
    struct rtnl_link* link = NULL;

    // there needs to be an allocated link cache in memory
    assert(*parent != NULL);
    assert(strcmp(LYD_NAME(*parent), "statistics") == 0);

    // get node xpath
    SRPC_SAFE_CALL_PTR(error_ptr, lyd_path(*parent, LYD_PATH_STD, xpath_buffer, sizeof(xpath_buffer)), error_out);

    // get link
    SRPC_SAFE_CALL_PTR(link, interfaces_get_current_link(ctx, session, xpath_buffer), error_out);

    const uint64_t out_errors = rtnl_link_get_stat(link, RTNL_LINK_TX_DROPPED);

    error = snprintf(out_errors_buffer, sizeof(out_errors_buffer), "%lu", out_errors);
    if (error < 0) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() failed (%d)", error);
        goto error_out;
    }

    SRPLG_LOG_INF(PLUGIN_NAME, "out-errors(%s) = %s", rtnl_link_get_name(link), out_errors_buffer);

    SRPC_SAFE_CALL_ERR(error, interfaces_ly_tree_create_interfaces_interface_statistics_out_errors(ly_ctx, *parent, out_errors_buffer), error_out);

    error = 0;
    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}

int interfaces_subscription_operational_interfaces_interface_statistics_in_discard_unknown_encaps(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    void* error_ptr = NULL;
    const struct ly_ctx* ly_ctx = NULL;

    if (*parent == NULL) {
        ly_ctx = sr_acquire_context(sr_session_get_connection(session));
        if (ly_ctx == NULL) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "sr_acquire_context() failed");
            goto error_out;
        }
    }

    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}

int interfaces_subscription_operational_interfaces_interface_carrier_delay_carrier_transitions(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    void* error_ptr = NULL;
    const struct ly_ctx* ly_ctx = NULL;

    if (*parent == NULL) {
        ly_ctx = sr_acquire_context(sr_session_get_connection(session));
        if (ly_ctx == NULL) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "sr_acquire_context() failed");
            goto error_out;
        }
    }

    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}

int interfaces_subscription_operational_interfaces_interface_carrier_delay_timer_running(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    void* error_ptr = NULL;
    const struct ly_ctx* ly_ctx = NULL;

    if (*parent == NULL) {
        ly_ctx = sr_acquire_context(sr_session_get_connection(session));
        if (ly_ctx == NULL) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "sr_acquire_context() failed");
            goto error_out;
        }
    }

    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}

int interfaces_subscription_operational_interfaces_interface_dampening_penalty(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    void* error_ptr = NULL;
    const struct ly_ctx* ly_ctx = NULL;

    if (*parent == NULL) {
        ly_ctx = sr_acquire_context(sr_session_get_connection(session));
        if (ly_ctx == NULL) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "sr_acquire_context() failed");
            goto error_out;
        }
    }

    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}

int interfaces_subscription_operational_interfaces_interface_dampening_suppressed(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    void* error_ptr = NULL;
    const struct ly_ctx* ly_ctx = NULL;

    if (*parent == NULL) {
        ly_ctx = sr_acquire_context(sr_session_get_connection(session));
        if (ly_ctx == NULL) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "sr_acquire_context() failed");
            goto error_out;
        }
    }

    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}

int interfaces_subscription_operational_interfaces_interface_dampening_time_remaining(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    void* error_ptr = NULL;
    const struct ly_ctx* ly_ctx = NULL;

    if (*parent == NULL) {
        ly_ctx = sr_acquire_context(sr_session_get_connection(session));
        if (ly_ctx == NULL) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "sr_acquire_context() failed");
            goto error_out;
        }
    }

    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}

int interfaces_subscription_operational_interfaces_interface_forwarding_mode(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    void* error_ptr = NULL;
    const struct ly_ctx* ly_ctx = NULL;

    if (*parent == NULL) {
        ly_ctx = sr_acquire_context(sr_session_get_connection(session));
        if (ly_ctx == NULL) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "sr_acquire_context() failed");
            goto error_out;
        }
    }

    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:
    return error;
}

int interfaces_subscription_operational_interfaces_interface(sr_session_ctx_t* session, uint32_t sub_id, const char* module_name, const char* path, const char* request_xpath, uint32_t request_id, struct lyd_node** parent, void* private_data)
{
    int error = SR_ERR_OK;
    void* error_ptr = NULL;
    const struct ly_ctx* ly_ctx = NULL;
    interfaces_ctx_t* ctx = private_data;
    interfaces_nl_ctx_t* nl_ctx = &ctx->nl_ctx;
    struct rtnl_link* link_iter = NULL;

    // libyang
    struct lyd_node* interface_list_node = NULL;

    // setup nl socket
    if (!nl_ctx->socket) {
        // netlink
        SRPC_SAFE_CALL_PTR(nl_ctx->socket, nl_socket_alloc(), error_out);

        // connect
        SRPC_SAFE_CALL_ERR(error, nl_connect(nl_ctx->socket, NETLINK_ROUTE), error_out);
    }

    // cache was already allocated - free existing cache
    if (nl_ctx->link_cache) {
        nl_cache_refill(nl_ctx->socket, nl_ctx->link_cache);
    } else {
        // allocate new link cache
        SRPC_SAFE_CALL_ERR(error, rtnl_link_alloc_cache(nl_ctx->socket, AF_UNSPEC, &nl_ctx->link_cache), error_out);
    }

    if (*parent == NULL) {
        ly_ctx = sr_acquire_context(sr_session_get_connection(session));
        if (ly_ctx == NULL) {
            SRPLG_LOG_ERR(PLUGIN_NAME, "sr_acquire_context() failed");
            goto error_out;
        }

        // set parent to the interfaces container
        SRPC_SAFE_CALL_ERR(error, interfaces_ly_tree_create_interfaces(ly_ctx, parent), error_out);
    }

    // iterate links and add them to the operational DS
    link_iter = (struct rtnl_link*)nl_cache_get_first(nl_ctx->link_cache);
    while (link_iter) {
        SRPLG_LOG_INF(PLUGIN_NAME, "Interface %s", rtnl_link_get_name(link_iter));

        // add interface
        SRPC_SAFE_CALL_ERR(error, interfaces_ly_tree_create_interfaces_interface(ly_ctx, *parent, &interface_list_node, rtnl_link_get_name(link_iter)), error_out);

        link_iter = (struct rtnl_link*)nl_cache_get_next((struct nl_object*)link_iter);
    }

    goto out;

error_out:
    error = SR_ERR_CALLBACK_FAILED;

out:

    return error;
}

static struct rtnl_link* interfaces_get_current_link(interfaces_ctx_t* ctx, sr_session_ctx_t* session, const char* xpath)
{
    int error = 0;

    const interfaces_nl_ctx_t* nl_ctx = &ctx->nl_ctx;

    // buffers
    char interface_name_buffer[100] = { 0 };

    // libnl
    struct rtnl_link* link = NULL;

    // there needs to be an allocated link cache in memory
    assert(nl_ctx->link_cache != NULL);

    // extract interface name
    SRPC_SAFE_CALL_ERR(error, interfaces_extract_interface_name(session, xpath, interface_name_buffer, sizeof(interface_name_buffer)), error_out);

    // get link by name
    SRPC_SAFE_CALL_PTR(link, rtnl_link_get_by_name(nl_ctx->link_cache, interface_name_buffer), error_out);

    return link;

error_out:
    return NULL;
}

static int interfaces_extract_interface_name(sr_session_ctx_t* session, const char* xpath, char* buffer, size_t buffer_size)
{
    int error = 0;

    const char* name = NULL;

    sr_xpath_ctx_t xpath_ctx = { 0 };

    // extract key
    SRPC_SAFE_CALL_PTR(name, sr_xpath_key_value((char*)xpath, "interface", "name", &xpath_ctx), error_out);

    // store to buffer
    error = snprintf(buffer, buffer_size, "%s", name);
    if (error < 0) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() failed");
        goto error_out;
    }

    error = 0;
    goto out;

error_out:
    error = -1;

out:
    return error;
}

static int interfaces_get_system_boot_time(char* buffer, size_t buffer_size)
{
    time_t now = 0;
    struct tm* ts = { 0 };
    struct sysinfo s_info = { 0 };
    time_t uptime_seconds = 0;

    now = time(NULL);

    ts = localtime(&now);
    if (ts == NULL)
        return -1;

    if (sysinfo(&s_info) != 0)
        return -1;

    uptime_seconds = s_info.uptime;

    time_t diff = now - uptime_seconds;

    ts = localtime(&diff);
    if (ts == NULL)
        return -1;

    strftime(buffer, buffer_size, "%FT%TZ", ts);

    return 0;
}