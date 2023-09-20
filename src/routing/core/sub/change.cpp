#include "change.hpp"

namespace ietf::rt {
namespace sub::change {
/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
RouterIdModuleChangeCb::RouterIdModuleChangeCb(std::shared_ptr<ietf::rt::ModuleChangeContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-routing:routing/router-id.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param event Type of the event that has occured.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * SR_EV_DONE, for example) have the same request ID.
 *
 * @return Error code.
 *
 */
sr::ErrorCode RouterIdModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
TypeModuleChangeCb::TypeModuleChangeCb(std::shared_ptr<ietf::rt::ModuleChangeContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/type.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param event Type of the event that has occured.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * SR_EV_DONE, for example) have the same request ID.
 *
 * @return Error code.
 *
 */
sr::ErrorCode TypeModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
NameModuleChangeCb::NameModuleChangeCb(std::shared_ptr<ietf::rt::ModuleChangeContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/name.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param event Type of the event that has occured.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * SR_EV_DONE, for example) have the same request ID.
 *
 * @return Error code.
 *
 */
sr::ErrorCode NameModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
DescriptionModuleChangeCb::DescriptionModuleChangeCb(std::shared_ptr<ietf::rt::ModuleChangeContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/description.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param event Type of the event that has occured.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * SR_EV_DONE, for example) have the same request ID.
 *
 * @return Error code.
 *
 */
sr::ErrorCode DescriptionModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
V4RouteDestinationPrefixModuleChangeCb::V4RouteDestinationPrefixModuleChangeCb(std::shared_ptr<ietf::rt::ModuleChangeContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/destination-prefix.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param event Type of the event that has occured.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * SR_EV_DONE, for example) have the same request ID.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V4RouteDestinationPrefixModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
V4RouteDescriptionModuleChangeCb::V4RouteDescriptionModuleChangeCb(std::shared_ptr<ietf::rt::ModuleChangeContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/description.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param event Type of the event that has occured.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * SR_EV_DONE, for example) have the same request ID.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V4RouteDescriptionModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
V4RouteNextHopOutgoingInterfaceModuleChangeCb::V4RouteNextHopOutgoingInterfaceModuleChangeCb(std::shared_ptr<ietf::rt::ModuleChangeContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop/outgoing-interface.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param event Type of the event that has occured.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * SR_EV_DONE, for example) have the same request ID.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V4RouteNextHopOutgoingInterfaceModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
V4RouteNextHopNextHopAddressModuleChangeCb::V4RouteNextHopNextHopAddressModuleChangeCb(std::shared_ptr<ietf::rt::ModuleChangeContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop/next-hop-address.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param event Type of the event that has occured.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * SR_EV_DONE, for example) have the same request ID.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V4RouteNextHopNextHopAddressModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
V4RouteNextHopSpecialNextHopModuleChangeCb::V4RouteNextHopSpecialNextHopModuleChangeCb(std::shared_ptr<ietf::rt::ModuleChangeContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop/special-next-hop.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param event Type of the event that has occured.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * SR_EV_DONE, for example) have the same request ID.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V4RouteNextHopSpecialNextHopModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
V4RouteNextHopNextHopListNextHopIndexModuleChangeCb::V4RouteNextHopNextHopListNextHopIndexModuleChangeCb(std::shared_ptr<ietf::rt::ModuleChangeContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop/next-hop-list/next-hop[index='%s']/index.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param event Type of the event that has occured.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * SR_EV_DONE, for example) have the same request ID.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V4RouteNextHopNextHopListNextHopIndexModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
V4RouteNextHopNextHopListNextHopOutgoingInterfaceModuleChangeCb::V4RouteNextHopNextHopListNextHopOutgoingInterfaceModuleChangeCb(std::shared_ptr<ietf::rt::ModuleChangeContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop/next-hop-list/next-hop[index='%s']/outgoing-interface.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param event Type of the event that has occured.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * SR_EV_DONE, for example) have the same request ID.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V4RouteNextHopNextHopListNextHopOutgoingInterfaceModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
V4RouteNextHopNextHopListNextHopNextHopAddressModuleChangeCb::V4RouteNextHopNextHopListNextHopNextHopAddressModuleChangeCb(std::shared_ptr<ietf::rt::ModuleChangeContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop/next-hop-list/next-hop[index='%s']/next-hop-address.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param event Type of the event that has occured.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * SR_EV_DONE, for example) have the same request ID.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V4RouteNextHopNextHopListNextHopNextHopAddressModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
V4RouteNextHopNextHopListNextHopModuleChangeCb::V4RouteNextHopNextHopListNextHopModuleChangeCb(std::shared_ptr<ietf::rt::ModuleChangeContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop/next-hop-list/next-hop[index='%s'].
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param event Type of the event that has occured.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * SR_EV_DONE, for example) have the same request ID.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V4RouteNextHopNextHopListNextHopModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
V4RouteNextHopNextHopListModuleChangeCb::V4RouteNextHopNextHopListModuleChangeCb(std::shared_ptr<ietf::rt::ModuleChangeContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop/next-hop-list.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param event Type of the event that has occured.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * SR_EV_DONE, for example) have the same request ID.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V4RouteNextHopNextHopListModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
V4RouteNextHopModuleChangeCb::V4RouteNextHopModuleChangeCb(std::shared_ptr<ietf::rt::ModuleChangeContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s']/next-hop.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param event Type of the event that has occured.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * SR_EV_DONE, for example) have the same request ID.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V4RouteNextHopModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
V4RouteModuleChangeCb::V4RouteModuleChangeCb(std::shared_ptr<ietf::rt::ModuleChangeContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4/route[destination-prefix='%s'].
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param event Type of the event that has occured.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * SR_EV_DONE, for example) have the same request ID.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V4RouteModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
V4ModuleChangeCb::V4ModuleChangeCb(std::shared_ptr<ietf::rt::ModuleChangeContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv4-unicast-routing:ipv4.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param event Type of the event that has occured.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * SR_EV_DONE, for example) have the same request ID.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V4ModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
V6RouteDestinationPrefixModuleChangeCb::V6RouteDestinationPrefixModuleChangeCb(std::shared_ptr<ietf::rt::ModuleChangeContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/destination-prefix.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param event Type of the event that has occured.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * SR_EV_DONE, for example) have the same request ID.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V6RouteDestinationPrefixModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
V6RouteDescriptionModuleChangeCb::V6RouteDescriptionModuleChangeCb(std::shared_ptr<ietf::rt::ModuleChangeContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/description.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param event Type of the event that has occured.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * SR_EV_DONE, for example) have the same request ID.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V6RouteDescriptionModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
V6RouteNextHopOutgoingInterfaceModuleChangeCb::V6RouteNextHopOutgoingInterfaceModuleChangeCb(std::shared_ptr<ietf::rt::ModuleChangeContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop/outgoing-interface.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param event Type of the event that has occured.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * SR_EV_DONE, for example) have the same request ID.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V6RouteNextHopOutgoingInterfaceModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
V6RouteNextHopNextHopAddressModuleChangeCb::V6RouteNextHopNextHopAddressModuleChangeCb(std::shared_ptr<ietf::rt::ModuleChangeContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop/next-hop-address.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param event Type of the event that has occured.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * SR_EV_DONE, for example) have the same request ID.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V6RouteNextHopNextHopAddressModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
V6RouteNextHopSpecialNextHopModuleChangeCb::V6RouteNextHopSpecialNextHopModuleChangeCb(std::shared_ptr<ietf::rt::ModuleChangeContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop/special-next-hop.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param event Type of the event that has occured.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * SR_EV_DONE, for example) have the same request ID.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V6RouteNextHopSpecialNextHopModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
V6RouteNextHopNextHopListNextHopIndexModuleChangeCb::V6RouteNextHopNextHopListNextHopIndexModuleChangeCb(std::shared_ptr<ietf::rt::ModuleChangeContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop/next-hop-list/next-hop[index='%s']/index.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param event Type of the event that has occured.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * SR_EV_DONE, for example) have the same request ID.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V6RouteNextHopNextHopListNextHopIndexModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
V6RouteNextHopNextHopListNextHopOutgoingInterfaceModuleChangeCb::V6RouteNextHopNextHopListNextHopOutgoingInterfaceModuleChangeCb(std::shared_ptr<ietf::rt::ModuleChangeContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop/next-hop-list/next-hop[index='%s']/outgoing-interface.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param event Type of the event that has occured.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * SR_EV_DONE, for example) have the same request ID.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V6RouteNextHopNextHopListNextHopOutgoingInterfaceModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
V6RouteNextHopNextHopListNextHopNextHopAddressModuleChangeCb::V6RouteNextHopNextHopListNextHopNextHopAddressModuleChangeCb(std::shared_ptr<ietf::rt::ModuleChangeContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop/next-hop-list/next-hop[index='%s']/next-hop-address.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param event Type of the event that has occured.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * SR_EV_DONE, for example) have the same request ID.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V6RouteNextHopNextHopListNextHopNextHopAddressModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
V6RouteNextHopNextHopListNextHopModuleChangeCb::V6RouteNextHopNextHopListNextHopModuleChangeCb(std::shared_ptr<ietf::rt::ModuleChangeContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop/next-hop-list/next-hop[index='%s'].
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param event Type of the event that has occured.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * SR_EV_DONE, for example) have the same request ID.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V6RouteNextHopNextHopListNextHopModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
V6RouteNextHopNextHopListModuleChangeCb::V6RouteNextHopNextHopListModuleChangeCb(std::shared_ptr<ietf::rt::ModuleChangeContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop/next-hop-list.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param event Type of the event that has occured.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * SR_EV_DONE, for example) have the same request ID.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V6RouteNextHopNextHopListModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
V6RouteNextHopModuleChangeCb::V6RouteNextHopModuleChangeCb(std::shared_ptr<ietf::rt::ModuleChangeContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s']/next-hop.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param event Type of the event that has occured.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * SR_EV_DONE, for example) have the same request ID.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V6RouteNextHopModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
V6RouteModuleChangeCb::V6RouteModuleChangeCb(std::shared_ptr<ietf::rt::ModuleChangeContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6/route[destination-prefix='%s'].
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param event Type of the event that has occured.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * SR_EV_DONE, for example) have the same request ID.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V6RouteModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
V6ModuleChangeCb::V6ModuleChangeCb(std::shared_ptr<ietf::rt::ModuleChangeContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes/ietf-ipv6-unicast-routing:ipv6.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param event Type of the event that has occured.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * SR_EV_DONE, for example) have the same request ID.
 *
 * @return Error code.
 *
 */
sr::ErrorCode V6ModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
StaticRoutesModuleChangeCb::StaticRoutesModuleChangeCb(std::shared_ptr<ietf::rt::ModuleChangeContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s']/static-routes.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param event Type of the event that has occured.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * SR_EV_DONE, for example) have the same request ID.
 *
 * @return Error code.
 *
 */
sr::ErrorCode StaticRoutesModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
ControlPlaneProtocolModuleChangeCb::ControlPlaneProtocolModuleChangeCb(std::shared_ptr<ietf::rt::ModuleChangeContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-routing:routing/control-plane-protocols/control-plane-protocol[name='%s'][type='%s'].
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param event Type of the event that has occured.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * SR_EV_DONE, for example) have the same request ID.
 *
 * @return Error code.
 *
 */
sr::ErrorCode ControlPlaneProtocolModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
ControlPlaneProtocolsModuleChangeCb::ControlPlaneProtocolsModuleChangeCb(std::shared_ptr<ietf::rt::ModuleChangeContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-routing:routing/control-plane-protocols.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param event Type of the event that has occured.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * SR_EV_DONE, for example) have the same request ID.
 *
 * @return Error code.
 *
 */
sr::ErrorCode ControlPlaneProtocolsModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
NameModuleChangeCb::NameModuleChangeCb(std::shared_ptr<ietf::rt::ModuleChangeContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-routing:routing/ribs/rib[name='%s']/name.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param event Type of the event that has occured.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * SR_EV_DONE, for example) have the same request ID.
 *
 * @return Error code.
 *
 */
sr::ErrorCode NameModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
AddressFamilyModuleChangeCb::AddressFamilyModuleChangeCb(std::shared_ptr<ietf::rt::ModuleChangeContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-routing:routing/ribs/rib[name='%s']/address-family.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param event Type of the event that has occured.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * SR_EV_DONE, for example) have the same request ID.
 *
 * @return Error code.
 *
 */
sr::ErrorCode AddressFamilyModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
DescriptionModuleChangeCb::DescriptionModuleChangeCb(std::shared_ptr<ietf::rt::ModuleChangeContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-routing:routing/ribs/rib[name='%s']/description.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param event Type of the event that has occured.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * SR_EV_DONE, for example) have the same request ID.
 *
 * @return Error code.
 *
 */
sr::ErrorCode DescriptionModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
RibModuleChangeCb::RibModuleChangeCb(std::shared_ptr<ietf::rt::ModuleChangeContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-routing:routing/ribs/rib[name='%s'].
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param event Type of the event that has occured.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * SR_EV_DONE, for example) have the same request ID.
 *
 * @return Error code.
 *
 */
sr::ErrorCode RibModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
RibsModuleChangeCb::RibsModuleChangeCb(std::shared_ptr<ietf::rt::ModuleChangeContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-routing:routing/ribs.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param event Type of the event that has occured.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * SR_EV_DONE, for example) have the same request ID.
 *
 * @return Error code.
 *
 */
sr::ErrorCode RibsModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
RoutingModuleChangeCb::RoutingModuleChangeCb(std::shared_ptr<ietf::rt::ModuleChangeContext> ctx)
{
    m_ctx = ctx;
}

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-routing:routing.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param event Type of the event that has occured.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * SR_EV_DONE, for example) have the same request ID.
 *
 * @return Error code.
 *
 */
sr::ErrorCode RoutingModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

}
}