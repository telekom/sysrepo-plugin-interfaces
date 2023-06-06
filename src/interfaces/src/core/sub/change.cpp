#include "change.hpp"
#include <iostream>
#include <optional>
#include <libyang-cpp/DataNode.hpp>
#include <interfaces/src/core/system/interface.hpp>
#include <sysrepo.h>

using std::cout;
using std::endl;

namespace ietf::ifc {
namespace sub::change {
    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin module change context.
     *
     */
    InterfaceModuleEnabledChangeCb::InterfaceModuleEnabledChangeCb(std::shared_ptr<ietf::ifc::ModuleChangeContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated module change operator() for path /ietf-interfaces:interfaces/interface[name='%s'].
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
    sr::ErrorCode InterfaceModuleEnabledChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
        std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
    {

        sr::ErrorCode error = sr::ErrorCode::Ok;

        switch (event) {
        case sysrepo::Event::Change:
            for (sysrepo::Change change : session.getChanges(subXPath->data())) {
                switch (change.operation) {
                case sysrepo::ChangeOperation::Created:
                case sysrepo::ChangeOperation::Modified: {

                    libyang::DataNode tmp(change.node.firstSibling());
                    std::string name = tmp.asTerm().valueStr().data();

                    libyang::Value value = change.node.asTerm().value();
                    bool enabled = std::get<bool>(value);
                    int ifindex = getIfindexFromName(name);

                    if (ifindex == 0) {
                        SRPLG_LOG_ERR("ietf_interfaces", "non-existing ifindex at interface %s", name);
                        return sr::ErrorCode::OperationFailed;
                    } else {
                        try{
                            Interface(ifindex).setEnabled(enabled);
                        }catch(std::runtime_error &e){
                            SRPLG_LOG_ERR("ietf_interfaces", "Error changing interface state, reason:  %s", e.what());
                            return sr::ErrorCode::OperationFailed;
                        }
                        
                    }
                }
                case sysrepo::ChangeOperation::Deleted:
                    break;
                }
            }
            break;
        default:
            break;
        }

        return error;
    }

}
}