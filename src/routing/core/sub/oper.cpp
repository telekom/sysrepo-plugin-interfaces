#include "oper.hpp"

namespace ietf::rt {
namespace sub::oper {
/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
RouterIdOperGetCb::RouterIdOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/router-id.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode RouterIdOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
InterfaceOperGetCb::InterfaceOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/interfaces/interface.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode InterfaceOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
InterfacesOperGetCb::InterfacesOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/interfaces.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode InterfacesOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
TypeOperGetCb::TypeOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/type.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode TypeOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
NameOperGetCb::NameOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/name.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode NameOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
DescriptionOperGetCb::DescriptionOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/description.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode DescriptionOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
V4RouteDestinationPrefixOperGetCb::V4RouteDestinationPrefixOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/destination-prefix.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V4RouteDestinationPrefixOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
V4RouteDescriptionOperGetCb::V4RouteDescriptionOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/description.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V4RouteDescriptionOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
V4RouteNextHopOutgoingInterfaceOperGetCb::V4RouteNextHopOutgoingInterfaceOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop/outgoing-interface.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V4RouteNextHopOutgoingInterfaceOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
V4RouteNextHopNextHopAddressOperGetCb::V4RouteNextHopNextHopAddressOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop/next-hop-address.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V4RouteNextHopNextHopAddressOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
V4RouteNextHopSpecialNextHopOperGetCb::V4RouteNextHopSpecialNextHopOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop/special-next-hop.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V4RouteNextHopSpecialNextHopOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
V4RouteNextHopNextHopListNextHopIndexOperGetCb::V4RouteNextHopNextHopListNextHopIndexOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop/next-hop-list/next-hop[index='%s']/index.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V4RouteNextHopNextHopListNextHopIndexOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
V4RouteNextHopNextHopListNextHopOutgoingInterfaceOperGetCb::V4RouteNextHopNextHopListNextHopOutgoingInterfaceOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop/next-hop-list/next-hop[index='%s']/outgoing-interface.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V4RouteNextHopNextHopListNextHopOutgoingInterfaceOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
V4RouteNextHopNextHopListNextHopNextHopAddressOperGetCb::V4RouteNextHopNextHopListNextHopNextHopAddressOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop/next-hop-list/next-hop[index='%s']/next-hop-address.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V4RouteNextHopNextHopListNextHopNextHopAddressOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
V4RouteNextHopNextHopListNextHopOperGetCb::V4RouteNextHopNextHopListNextHopOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop/next-hop-list/next-hop[index='%s'].
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V4RouteNextHopNextHopListNextHopOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
V4RouteNextHopNextHopListOperGetCb::V4RouteNextHopNextHopListOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop/next-hop-list.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V4RouteNextHopNextHopListOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
V4RouteNextHopOperGetCb::V4RouteNextHopOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V4RouteNextHopOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
V4RouteOperGetCb::V4RouteOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s'].
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V4RouteOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
V4OperGetCb::V4OperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V4OperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
V6RouteDestinationPrefixOperGetCb::V6RouteDestinationPrefixOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/destination-prefix.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V6RouteDestinationPrefixOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
V6RouteDescriptionOperGetCb::V6RouteDescriptionOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/description.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V6RouteDescriptionOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
V6RouteNextHopOutgoingInterfaceOperGetCb::V6RouteNextHopOutgoingInterfaceOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop/outgoing-interface.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V6RouteNextHopOutgoingInterfaceOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
V6RouteNextHopNextHopAddressOperGetCb::V6RouteNextHopNextHopAddressOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop/next-hop-address.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V6RouteNextHopNextHopAddressOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
V6RouteNextHopSpecialNextHopOperGetCb::V6RouteNextHopSpecialNextHopOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop/special-next-hop.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V6RouteNextHopSpecialNextHopOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
V6RouteNextHopNextHopListNextHopIndexOperGetCb::V6RouteNextHopNextHopListNextHopIndexOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop/next-hop-list/next-hop[index='%s']/index.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V6RouteNextHopNextHopListNextHopIndexOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
V6RouteNextHopNextHopListNextHopOutgoingInterfaceOperGetCb::V6RouteNextHopNextHopListNextHopOutgoingInterfaceOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop/next-hop-list/next-hop[index='%s']/outgoing-interface.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V6RouteNextHopNextHopListNextHopOutgoingInterfaceOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
V6RouteNextHopNextHopListNextHopNextHopAddressOperGetCb::V6RouteNextHopNextHopListNextHopNextHopAddressOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop/next-hop-list/next-hop[index='%s']/next-hop-address.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V6RouteNextHopNextHopListNextHopNextHopAddressOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
V6RouteNextHopNextHopListNextHopOperGetCb::V6RouteNextHopNextHopListNextHopOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop/next-hop-list/next-hop[index='%s'].
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V6RouteNextHopNextHopListNextHopOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
V6RouteNextHopNextHopListOperGetCb::V6RouteNextHopNextHopListOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop/next-hop-list.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V6RouteNextHopNextHopListOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
V6RouteNextHopOperGetCb::V6RouteNextHopOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V6RouteNextHopOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
V6RouteOperGetCb::V6RouteOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s'].
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V6RouteOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
V6OperGetCb::V6OperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V6OperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
StaticRoutesOperGetCb::StaticRoutesOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode StaticRoutesOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
ControlPlaneProtocolOperGetCb::ControlPlaneProtocolOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s'].
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode ControlPlaneProtocolOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
ControlPlaneProtocolsOperGetCb::ControlPlaneProtocolsOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/control-plane-protocols.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode ControlPlaneProtocolsOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
NameOperGetCb::NameOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/ribs/rib[name='%s']/name.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode NameOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
AddressFamilyOperGetCb::AddressFamilyOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/ribs/rib[name='%s']/address-family.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode AddressFamilyOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
DefaultRibOperGetCb::DefaultRibOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/ribs/rib[name='%s']/default-rib.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode DefaultRibOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
RoutePreferenceOperGetCb::RoutePreferenceOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/ribs/rib[name='%s']/routes/route/route-preference.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode RoutePreferenceOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
OutgoingInterfaceOperGetCb::OutgoingInterfaceOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/ribs/rib[name='%s']/routes/route/next-hop/outgoing-interface.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode OutgoingInterfaceOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
NextHopAddressOperGetCb::NextHopAddressOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/ribs/rib[name='%s']/routes/route/next-hop/ietf-ipv4-unicast-routing:next-hop-address.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode NextHopAddressOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
NextHopAddressOperGetCb::NextHopAddressOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/ribs/rib[name='%s']/routes/route/next-hop/ietf-ipv6-unicast-routing:next-hop-address.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode NextHopAddressOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
SpecialNextHopOperGetCb::SpecialNextHopOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/ribs/rib[name='%s']/routes/route/next-hop/special-next-hop.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode SpecialNextHopOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
OutgoingInterfaceOperGetCb::OutgoingInterfaceOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/ribs/rib[name='%s']/routes/route/next-hop/next-hop-list/next-hop/outgoing-interface.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode OutgoingInterfaceOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
AddressOperGetCb::AddressOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/ribs/rib[name='%s']/routes/route/next-hop/next-hop-list/next-hop/ietf-ipv4-unicast-routing:address.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode AddressOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
AddressOperGetCb::AddressOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/ribs/rib[name='%s']/routes/route/next-hop/next-hop-list/next-hop/ietf-ipv6-unicast-routing:address.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode AddressOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
NextHopOperGetCb::NextHopOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/ribs/rib[name='%s']/routes/route/next-hop/next-hop-list/next-hop.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode NextHopOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
NextHopListOperGetCb::NextHopListOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/ribs/rib[name='%s']/routes/route/next-hop/next-hop-list.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode NextHopListOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
NextHopOperGetCb::NextHopOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/ribs/rib[name='%s']/routes/route/next-hop.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode NextHopOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
SourceProtocolOperGetCb::SourceProtocolOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/ribs/rib[name='%s']/routes/route/source-protocol.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode SourceProtocolOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
ActiveOperGetCb::ActiveOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/ribs/rib[name='%s']/routes/route/active.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode ActiveOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
LastUpdatedOperGetCb::LastUpdatedOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/ribs/rib[name='%s']/routes/route/last-updated.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode LastUpdatedOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
DestinationPrefixOperGetCb::DestinationPrefixOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/ribs/rib[name='%s']/routes/route/ietf-ipv4-unicast-routing:destination-prefix.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode DestinationPrefixOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
DestinationPrefixOperGetCb::DestinationPrefixOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/ribs/rib[name='%s']/routes/route/ietf-ipv6-unicast-routing:destination-prefix.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode DestinationPrefixOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
RouteOperGetCb::RouteOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/ribs/rib[name='%s']/routes/route.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode RouteOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
RoutesOperGetCb::RoutesOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/ribs/rib[name='%s']/routes.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode RoutesOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
DescriptionOperGetCb::DescriptionOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/ribs/rib[name='%s']/description.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode DescriptionOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
RibOperGetCb::RibOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/ribs/rib[name='%s'].
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode RibOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
RibsOperGetCb::RibsOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing/ribs.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode RibsOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
RoutingOperGetCb::RoutingOperGetCb(std::shared_ptr<ietf::rt::OperContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-routing:routing.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode RoutingOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

}
}