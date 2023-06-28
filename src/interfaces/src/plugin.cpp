#include "plugin.hpp"
#include "core/context.hpp"

// subscription API
#include "core/sub/change.hpp"
#include "core/sub/oper.hpp"
#include "core/sub/rpc.hpp"
#include "sysrepo-cpp/Enum.hpp"
#include "sysrepo-cpp/Subscription.hpp"
#include "sysrepo.h"

#include <sysrepo-cpp/Session.hpp>
#include <sysrepo-cpp/utils/utils.hpp>

#include "core/system/Address.hpp"
#include "core/system/interface.hpp"
#include "core/system/IPV4.hpp"
#include "core/system/IPV6.hpp"
#include "core/system/Neighbour.hpp"

namespace sr = sysrepo;

/**
 * @brief Operational callback struct.
 */
struct OperationalCallback {
    std::string xpath; ///< XPath of the data.
    sysrepo::OperGetCb callback; ///< Callback function.
};

/**
 * @brief Module change callback struct.
 */
struct ModuleChangeCallback {
    std::string xpath; ///< XPath of the data.
    sysrepo::ModuleChangeCb callback; ///< Callback function.
};

/**
 * @brief RPC callback struct.
 */
struct RpcCallback {
    std::string xpath; ///< XPath of the data.
    sysrepo::RpcActionCb callback; ///< Callback function.
};

/**
 * Register all operational plugin subscriptions.
 *
 * @param sess Session to use for creating subscriptions.
 * @param ctx Plugin context.
 *
 */
// void registerOperationalSubscriptions(sr::Session& sess, ietf::ifc::PluginContext& ctx);

/**
 * Register all module change plugin subscriptions.
 *
 * @param sess Session to use for creating subscriptions.
 * @param ctx Plugin context.
 *
 */
void registerModuleChangeSubscriptions(sr::Session& sess, ietf::ifc::PluginContext& ctx);
void loadRunningDatastore(sysrepo::Session& session);

/**
 * Register all RPC plugin subscriptions.
 *
 * @param sess Session to use for creating subscriptions.
 * @param ctx Plugin context.
 *
 */
// void registerRpcSubscriptions(sr::Session& sess, ietf::ifc::PluginContext& ctx);

/**
 * @brief Plugin init callback.
 *
 * @param session Plugin session.
 * @param priv Private data.
 *
 * @return Error code (SR_ERR_OK on success).
 */

int sr_plugin_init_cb(sr_session_ctx_t* session, void** priv)
{
    sr::ErrorCode error = sysrepo::ErrorCode::Ok;
    sysrepo::Session sess = sysrepo::wrapUnmanagedSession(session);
    auto plugin_ctx = new ietf::ifc::PluginContext(sess);

    *priv = static_cast<void*>(plugin_ctx);

    // create session subscriptions
    SRPLG_LOG_INF("ietf-interfaces-plugin", "Creating plugin subscriptions");
    loadRunningDatastore(sess);
    // registerOperationalSubscriptions(sess, *plugin_ctx);
    registerModuleChangeSubscriptions(sess, *plugin_ctx);
    // registerRpcSubscriptions(sess, *plugin_ctx);

    SRPLG_LOG_INF("ietf-interfaces-plugin", "Created plugin subscriptions");

    return static_cast<int>(error);
}

void sr_plugin_cleanup_cb(sr_session_ctx_t* session, void* priv)
{

    SRPLG_LOG_INF("ietf-interfaces-plugin", "Plugin cleanup called");
    auto plugin_ctx = static_cast<ietf::ifc::PluginContext*>(priv);
    delete plugin_ctx;
    SRPLG_LOG_INF("ietf-interfaces-plugin", "Plugin cleanup finished");
}

void registerModuleChangeSubscriptions(sr::Session& sess, ietf::ifc::PluginContext& ctx)
{
    const auto change_callbacks = { ModuleChangeCallback { "/ietf-interfaces:interfaces/interface",
                                        ietf::ifc::sub::change::InterfaceModuleChangeCb(ctx.getModuleChangeContext()) },
        ModuleChangeCallback {
            "/ietf-interfaces:interfaces/interface/enabled", ietf::ifc::sub::change::InterfaceModuleEnabledChangeCb(ctx.getModuleChangeContext()) },
        ModuleChangeCallback {
            "/ietf-interfaces:interfaces/interface/type", ietf::ifc::sub::change::InterfaceModuleTypeChangeCb(ctx.getModuleChangeContext()) },
        ModuleChangeCallback { "/ietf-interfaces:interfaces/interface/ipv4/address/ip",
            ietf::ifc::sub::change::InterfaceModuleIPV4AddressChangeCb(ctx.getModuleChangeContext()) },
        ModuleChangeCallback { "/ietf-interfaces:interfaces/interface/ipv4/address/prefix-length",
            ietf::ifc::sub::change::InterfaceModuleIPV4PrefixChangeCb(ctx.getModuleChangeContext()) },
        ModuleChangeCallback { "/ietf-interfaces:interfaces/interface/ipv4/enabled",
            ietf::ifc::sub::change::InterfaceModuleIPV4EnableChangeCb(ctx.getModuleChangeContext()) },
        ModuleChangeCallback {
            "/ietf-interfaces:interfaces/interface/ipv4/mtu", ietf::ifc::sub::change::InterfaceModuleIPV4MtuChangeCb(ctx.getModuleChangeContext()) },
        ModuleChangeCallback { "/ietf-interfaces:interfaces/interface/ipv4/neighbor/ip",
            ietf::ifc::sub::change::InterfaceModuleIPV4NeighbourIpChangeCb(ctx.getModuleChangeContext()) },
        ModuleChangeCallback { "/ietf-interfaces:interfaces/interface/ipv4/neighbor/link-layer-address",
            ietf::ifc::sub::change::InterfaceModuleIPV4NeighbourLLAddressChangeCb(ctx.getModuleChangeContext()) },
        ModuleChangeCallback {
            "/ietf-interfaces:interfaces/interface/ipv6//*", ietf::ifc::sub::change::InterfaceModuleIPV6ChangeCb(ctx.getModuleChangeContext()) } };

    auto& sub_handle = ctx.getSubscriptionHandle();

    for (auto& cb : change_callbacks) {
        if (sub_handle.has_value()) {
            sub_handle->onModuleChange("ietf-interfaces", cb.callback, cb.xpath);
        } else {
            sub_handle = sess.onModuleChange("ietf-interfaces", cb.callback, cb.xpath);
        }
    }
}

void loadRunningDatastore(sysrepo::Session& session)
{
    // first switch to running datastore
    session.switchDatastore(sysrepo::Datastore::Running);

    // check if datastore is empty
    std::optional<libyang::DataNode> starting_node = session.getData("/ietf-interfaces:interfaces/interface");

    if (starting_node == std::nullopt) {

        for (auto&& i : getInterfaces()) {
            Interface interface(i);
            std::string name = interface.getName();
            std::string type = interface.getType();
            bool enabled = interface.getEnabled();

            session.setItem("/ietf-interfaces:interfaces/interface[name='" + name + "']/type", type);
            session.setItem("/ietf-interfaces:interfaces/interface[name='" + name + "']/enabled", enabled ? "true" : "false");

            IPV4 ipv4_address(i);

            for (Address& addr : ipv4_address.getAdressList()) {
                std::string ip_address = addr.getAddressString();
                std::string prefix_length = std::to_string(addr.getPrefixLen());

                session.setItem(
                    "/ietf-interfaces:interfaces/interface[name='" + name + "']/ietf-ip:ipv4/address[ip='" + ip_address + "']/prefix-length",
                    prefix_length);
            }

            for (Neighbour& neigh : ipv4_address.getNeighbours()) {
                std::string neigh_ip = neigh.getAddress();
                std::string ll_addr = neigh.getLinkLayer();

                session.setItem(
                    "/ietf-interfaces:interfaces/interface[name='" + name + "']/ietf-ip:ipv4/neighbor[ip='" + neigh_ip + "']/link-layer-address",
                    ll_addr);
            }

            IPV6 ipv6_address(i);

            for (Address& addr : ipv6_address.getAddressList()) {

                std::string ip_address = addr.getAddressString();
                std::string prefix_len = std::to_string(addr.getPrefixLen());
                 session.setItem(
                    "/ietf-interfaces:interfaces/interface[name='" + name + "']/ietf-ip:ipv6/address[ip='" + ip_address + "']/prefix-length",
                    prefix_len);
            }

            for (Neighbour& neigh : ipv6_address.getNeighborList()) {

                std::string ip_address = neigh.getAddress();
                std::string ll_addr = neigh.getLinkLayer();
                 session.setItem(
                    "/ietf-interfaces:interfaces/interface[name='" + name + "']/ietf-ip:ipv6/neighbor[ip='" + ip_address + "']/link-layer-address",
                 ll_addr);
            }
        }

        session.applyChanges();

        // //sess.setItem("/ietf-interfaces:interfaces/interface/name","test_if");

        // session.setItem("/ietf-interfaces:interfaces/interface[name='test_if']/enabled", "true");
        // session.applyChanges();
    }
};