#include "oper.hpp"

namespace ietf::rt {
namespace sub::oper {
    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin operational context.
     *
     */
    RoutingOperGetCb::RoutingOperGetCb(std::shared_ptr<RoutingOperationalContext> ctx) { m_ctx = ctx; }

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
    sr::ErrorCode RoutingOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
        std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId,
        std::optional<ly::DataNode>& output)
    {
        sr::ErrorCode error = sr::ErrorCode::Ok;
        return error;
    }

}
}
