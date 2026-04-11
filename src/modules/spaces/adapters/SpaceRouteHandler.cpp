#include "modules/spaces/adapters/SpaceRouteHandler.hpp"

#include "crow.h"
#include "shared/adapters/SpaceGuard.hpp"

#include <exception>
#include <string>
#include <vector>
#include <spdlog/spdlog.h>
#include "modules/spaces/adapters/dto/SpaceDto.hpp"
#include <ranges>

void SpaceRouteHandler::setup(crow::App<JwtMiddleware, SpaceMiddleware> &app)
{
    CROW_ROUTE(app, "/api/spaces/<int>")
        .methods("GET"_method)
        .CROW_MIDDLEWARES(app, JwtMiddleware, SpaceMiddleware)([this, &app](const crow::request &req, int spaceId) -> crow::response
                                                               {
            const auto &ctx = app.get_context<JwtMiddleware>(req);
            const auto &spaceCtx = app.get_context<SpaceMiddleware>(req);

            spdlog::info("Handling get space by id request: method=GET path={} user_id={} space_id={}", req.url, spaceCtx.user_id.has_value() ? spaceCtx.user_id.value_or("null") : "null", spaceId);

            auto spaceOpt = spaceService_->get_space_by_id(static_cast<long int>(spaceId));
            if (!spaceOpt.has_value())
            {
                return crow::response(404, "Space not found");
            }

            crow::response response(200);
            response.body = spaceOpt->to_json().dump();
            return response; });

    CROW_ROUTE(app, "/api/spaces/<int>/query")
        .methods("GET"_method)
        .CROW_MIDDLEWARES(app, JwtMiddleware)([this, &app](const crow::request &req, int spaceId) -> crow::response
                                              {
            const auto &ctx = app.get_context<JwtMiddleware>(req);

            const char *columnsRaw = req.url_params.get("columns");
            if (columnsRaw == nullptr)
            {
                return crow::response(400, "Missing query parameter: columns");
            }

            std::vector<std::string> columns;
            std::string columnsValue(columnsRaw);
            std::string current;
            for (char ch : columnsValue)
            {
                if (ch == ',')
                {
                    if (!current.empty())
                    {
                        columns.push_back(current);
                        current.clear();
                    }
                    continue;
                }

                if (ch != ' ')
                {
                    current.push_back(ch);
                }
            }
            if (!current.empty())
            {
                columns.push_back(current);
            }

            if (columns.empty())
            {
                return crow::response(400, "Invalid query parameter: columns");
            }

            auto spaceOpt = spaceService_->get_space_by_id_query(static_cast<long int>(spaceId), columns);
            if (!spaceOpt.has_value())
            {
                return crow::response(404, "Space not found");
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

            crow::response response(200);
            response.body = crow::json::wvalue(std::move(result)).dump();
            return response; });

    CROW_ROUTE(app, "/api/spaces").methods("POST"_method).CROW_MIDDLEWARES(app, JwtMiddleware)([this, &app](const crow::request &req)

                                                                                               {
        const auto &ctx = app.get_context<JwtMiddleware>(req);
        if (!ctx.user_id.has_value())
        {
            return crow::response(401, "Unauthorized");
        }

        auto body = crow::json::load(req.body);
        if (!body)
        {
            return crow::response(400, "Invalid JSON");
        }
        crow::json::wvalue body_s(body);
        try
        {
            body_s["owner_id"] = std::stol(*ctx.user_id);
        }
        catch (const std::exception &)
        {
            return crow::response(401, "Unauthorized: invalid user id in token");
        }

        auto createDto = SpaceCreateDto::from_json(crow::json::load(body_s.dump()));
        if (!createDto.has_value())
        {
            return crow::response(400, "Missing or invalid fields: title, owner_id");
        }

        const auto id = spaceService_->create_space(createDto.value());
        return crow::response(201, std::to_string(id)); });

    CROW_ROUTE(app, "/api/spaces").methods("PUT"_method)([this](const crow::request &req)
                                                         {
        spdlog::info("Handling spaces update request: method=PUT path={} body_size={}", req.url, req.body.size());
        auto body = crow::json::load(req.body);
        if (!body)
        {
            return crow::response(400, "Invalid JSON");
        }

        auto updateDto = SpaceUpdateDto::from_json(body);
        if (!updateDto.has_value())
        {
            return crow::response(400, "Missing or invalid fields: id");
        }

        try
        {
            spdlog::info("Calling update_space for spaces PUT");
            spaceService_->update_space(updateDto.value());
            return crow::response(200, "Space updated");
        }
        catch (const std::exception &ex)
        {
            spdlog::error("spaces PUT handler exception: {}", ex.what());
            return crow::response(400, ex.what());
        } });

    CROW_ROUTE(app, "/api/spaces/<int>/members")
        .methods("POST"_method)
        .CROW_MIDDLEWARES(app, JwtMiddleware)([this, &app](const crow::request &req, int spaceId) -> crow::response
                                              {
            const auto &ctx = app.get_context<JwtMiddleware>(req);

            auto body = crow::json::load(req.body);
            if (!body)
            {
                return crow::response(400, "Invalid JSON");
            }

            const auto userIds = JsonTypeHelper<std::vector<long int>>::read(body);

            try
            {
                spdlog::info("Calling add_space_members for spaces POST");
                spaceService_->add_space_members(static_cast<long int>(spaceId), userIds);
                return crow::response(200, "Members added");
            }
            catch (const std::exception &ex)
            {
                return crow::response(400, ex.what());
            } });

    CROW_ROUTE(app, "/api/spaces/<int>/members")
        .methods("DELETE"_method)
        .CROW_MIDDLEWARES(app, JwtMiddleware)([this](const crow::request &req, int spaceId) -> crow::response
                                              {
            auto body = crow::json::load(req.body);
            if (!body)
            {
                return crow::response(400, "Invalid JSON");
            }

            const auto userIds = JsonTypeHelper<std::vector<long int>>::read(body);
            try
            {
                spaceService_->remove_space_members(static_cast<long int>(spaceId), userIds);
                return crow::response(200, "Members removed");
            }
            catch (const std::exception &ex)
            {
                return crow::response(400, ex.what());
            } });

    CROW_ROUTE(app, "/api/spaces/<int>").methods("DELETE"_method)([this](const crow::request &, int spaceId)
                                                                  {
        spaceService_->delete_space_by_id(static_cast<long int>(spaceId));
        return crow::response(204); });
}
