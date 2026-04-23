#include "modules/automation/adapters/AutomationRouteHandler.hpp"

#include "crow.h"
#include "shared/adapters/SpaceGuard.hpp"

#include <exception>
#include <string>
#include <spdlog/spdlog.h>

#include "modules/automation/adapters/dto/AutomationDto.hpp"
#include "shared/adapters/http/response/HttpResponses.hpp"

void AutomationRouteHandler::setup(crow::App<JwtMiddleware, SpaceMiddleware, PermissionMiddleware> &app)
{
    // GET /api/automations
    CROW_ROUTE(app, "/api/automations")
        .methods("GET"_method)
        .CROW_MIDDLEWARES(app, JwtMiddleware, SpaceMiddleware)([this, &app](const crow::request &req) -> crow::response
                                                               {
            const auto &spaceCtx = app.get_context<SpaceMiddleware>(req);

            try
            {
                auto result = automation_module_->get_contexted_service(spaceCtx.space_id).get_automation();
                return HttpResponses::json_ok(result.to_json().dump());
            }
            catch (const std::exception &e)
            {
                spdlog::error("Exception fetching automation for space {}: {}", spaceCtx.space_id, e.what());
                return HttpResponses::internal_server_error(std::string("Internal server error: ") + e.what());
            }
            catch (...)
            {
                spdlog::error("Unknown error fetching automation for space {}", spaceCtx.space_id);
                return HttpResponses::internal_server_error();
            } });

    // PUT /api/automations
    CROW_ROUTE(app, "/api/automations")
        .methods("PUT"_method)
        .CROW_MIDDLEWARES(app, JwtMiddleware, SpaceMiddleware)([this, &app](const crow::request &req) -> crow::response
                                                               {
            const auto &spaceCtx = app.get_context<SpaceMiddleware>(req);

            if (spaceCtx.user_id != spaceCtx.admin_id)
            {
                return HttpResponses::forbidden();
            }

            auto body = crow::json::load(req.body);
            if (!body)
            {
                return HttpResponses::invalid_json();
            }
            auto [isValid, errors] = AutomationUpdateDto::validate(body);
            if (!isValid)
            {
                return HttpResponses::invalid_payload(errors);
            }

            try
            {
                auto dto = AutomationUpdateDto::from_json(body);
                automation_module_->get_contexted_service(spaceCtx.space_id).update_automation(dto.value());
                return HttpResponses::ok("Automation config updated successfully");
            }
            catch (const std::exception &e)
            {
                spdlog::error("Exception updating automation for space {}: {}", spaceCtx.space_id, e.what());
                return HttpResponses::internal_server_error(std::string("Internal server error: ") + e.what());
            }
            catch (...)
            {
                spdlog::error("Unknown error updating automation for space {}", spaceCtx.space_id);
                return HttpResponses::internal_server_error();
            } });
}
