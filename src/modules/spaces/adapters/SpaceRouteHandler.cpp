#include "modules/spaces/adapters/SpaceRouteHandler.hpp"

#include "crow.h"
#include "shared/adapters/SpaceGuard.hpp"

#include <exception>
#include <string>
#include <vector>
#include <spdlog/spdlog.h>
#include "modules/spaces/adapters/dto/SpaceDto.hpp"
#include "shared/adapters/http/response/HttpResponses.hpp"
#include "shared/adapters/http/request/HttpRequestParams.hpp"
#include <ranges>

void SpaceRouteHandler::setup(crow::App<JwtMiddleware, SpaceMiddleware, PermissionMiddleware> &app)
{

    CROW_ROUTE(app, "/api/spaces/<int>")
        .methods("GET"_method)
        .CROW_MIDDLEWARES(app, JwtMiddleware, SpaceMiddleware)([this, &app](const crow::request &req, int spaceId) -> crow::response
                                                               {
            const auto &ctx = app.get_context<JwtMiddleware>(req);
            
            auto relations = HttpRequestParams::parse_csv_param(req, "relations");

            auto spaceOpt = spaceService_->get_space_by_id_query(static_cast<long int>(spaceId), relations);
            if (!spaceOpt.has_value())
            {
                return HttpResponses::not_found("Space not found");
            }

            crow::response response(200);
            response.body = spaceOpt->to_json().dump();
            return response; });

    CROW_ROUTE(app, "/api/spaces/user/<int>")
        .methods("GET"_method)
        .CROW_MIDDLEWARES(app, JwtMiddleware)([this, &app](const crow::request &req, int userId) -> crow::response
                                              {
            const auto &ctx = app.get_context<JwtMiddleware>(req);

            const auto spaces = spaceService_->get_user_spaces(static_cast<long int>(userId));
            crow::json::wvalue::list result;
            result.reserve(spaces.size());
            for (const auto &space : spaces)
            {
                result.emplace_back(space.to_json());
            }

            return HttpResponses::json_ok(crow::json::wvalue(std::move(result)).dump()); });

    CROW_ROUTE(app, "/api/spaces")
        .methods("POST"_method)
        .CROW_MIDDLEWARES(app, JwtMiddleware)([this, &app](const crow::request &req)

                                              {
        const auto &ctx = app.get_context<JwtMiddleware>(req);
        auto body = crow::json::load(req.body);
        if (!body)
        {
            return HttpResponses::invalid_json();
        }
        crow::json::wvalue body_s(body);
        body_s["owner_id"] = std::stol(ctx.user_id.value());
        auto user_body = crow::json::load(body_s.dump());
        auto [isValid, errors] = SpaceCreateDto::validate(user_body);
        if (!isValid)
        {
            return HttpResponses::invalid_payload(errors);
        }
        auto createDto = SpaceCreateDto::from_json(user_body);
        const auto id = spaceService_->create_space(createDto.value());
            return HttpResponses::created(std::to_string(id)); });

    CROW_ROUTE(app, "/api/spaces")
        .methods("PUT"_method)
        .CROW_MIDDLEWARES(app, JwtMiddleware, SpaceMiddleware)([this](const crow::request &req)
                                                               {
        auto body = crow::json::load(req.body);
        if (!body)
        {
            return HttpResponses::invalid_json();
        }

        auto [isValid, errors] = UserUpdateDto::validate(body);
        if (!isValid)
        {
            return HttpResponses::invalid_payload(errors);
        }
        auto updateDto = SpaceUpdateDto::from_json(body);

            spaceService_->update_space(updateDto.value());
            return HttpResponses::ok("Space updated"); });

    CROW_ROUTE(app, "/api/spaces/<int>/members")
        .methods("POST"_method)
        .CROW_MIDDLEWARES(app, JwtMiddleware, SpaceMiddleware)([this, &app](const crow::request &req, int spaceId) -> crow::response
                                                               {
            const auto &ctx = app.get_context<JwtMiddleware>(req);

            auto body = crow::json::load(req.body);
            if (!body)
            {
                return HttpResponses::invalid_json();
            }

            const auto userIds = JsonTypeHelper<std::vector<long int>>::read(body);

                spaceService_->add_space_members(static_cast<long int>(spaceId), userIds);
                return HttpResponses::ok("Members added"); });

    CROW_ROUTE(app, "/api/spaces/<int>/members")
        .methods("DELETE"_method)
        .CROW_MIDDLEWARES(app, JwtMiddleware)([this](const crow::request &req, int spaceId) -> crow::response
                                              {
            auto body = crow::json::load(req.body);
            if (!body)
            {
                return HttpResponses::invalid_json();
            }

            const auto userIds = JsonTypeHelper<std::vector<long int>>::read(body);
                spaceService_->remove_space_members(static_cast<long int>(spaceId), userIds);
                return HttpResponses::ok("Members removed"); });

    CROW_ROUTE(app, "/api/spaces/<int>").methods("DELETE"_method)([this](const crow::request &, int spaceId)
                                                                  {
        spaceService_->delete_space_by_id(static_cast<long int>(spaceId));
        return HttpResponses::no_content(); });
}
