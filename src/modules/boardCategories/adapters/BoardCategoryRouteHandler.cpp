#include "modules/boardCategories/adapters/BoardCategoryRouteHandler.hpp"
#include "crow.h"
#include "shared/adapters/SpaceGuard.hpp"
#include "modules/boardCategories/adapters/dto/BoardCategoryDto.hpp"
#include "shared/adapters/http/response/HttpResponses.hpp"
#include "shared/adapters/http/request/HttpRequestParams.hpp"
#include "modules/automation/adapters/AutomationLoader.hpp"
#include <spdlog/spdlog.h>

void BoardCategoryRouteHandler::setup(crow::App<JwtMiddleware, SpaceMiddleware, PermissionMiddleware> &app)
{
    CROW_ROUTE(app, "/api/board-categories")
        .methods("GET"_method)
        .CROW_MIDDLEWARES(app, JwtMiddleware, SpaceMiddleware, PermissionMiddleware)([this, &app](const crow::request &req) -> crow::response
                                                                                     {
            const auto &spaceCtx = app.get_context<SpaceMiddleware>(req);
            const auto &permCtx = app.get_context<PermissionMiddleware>(req);
            AutomationLoader automationLoader(spaceCtx.space_id);
            auto relations = HttpRequestParams::parse_csv_param(req, "relations");
            try
            {
                auto cats = board_category_module_->get_contexted_service(spaceCtx.space_id, permCtx.permissions).get_all_board_categories_query_secured(relations);
                std::vector<crow::json::wvalue> list;
                list.reserve(cats.size());
                for (const auto &c : cats)
                    list.push_back(c.to_json());
                crow::json::wvalue response(list);
                return HttpResponses::json_ok(response.dump());
            }
            catch (const std::exception &e)
            {
                spdlog::error("Exception fetching board categories: {}", e.what());
                return HttpResponses::internal_server_error(std::string("Internal server error: ") + e.what());
            }
            catch (...)
            {
                return HttpResponses::internal_server_error();
            } });

    CROW_ROUTE(app, "/api/board-categories/<int>")
        .methods("GET"_method)
        .CROW_MIDDLEWARES(app, JwtMiddleware, SpaceMiddleware, PermissionMiddleware)([this, &app](const crow::request &req, long int cat_id) -> crow::response
                                                                                     {
            const auto &spaceCtx = app.get_context<SpaceMiddleware>(req);
            const auto &permCtx = app.get_context<PermissionMiddleware>(req);
            AutomationLoader automationLoader(spaceCtx.space_id);
            auto relations = HttpRequestParams::parse_csv_param(req, "relations");
            try
            {
                auto cat = board_category_module_->get_contexted_service(spaceCtx.space_id, permCtx.permissions).get_board_category_secured_by_id_query(cat_id, relations);
                return HttpResponses::json_ok(cat.to_json().dump());
            }
            catch (const std::exception &e)
            {
                spdlog::error("Exception fetching board category {}: {}", cat_id, e.what());
                return HttpResponses::not_found(std::string("BoardCategory not found: ") + e.what());
            }
            catch (...)
            {
                return HttpResponses::internal_server_error();
            } });

    CROW_ROUTE(app, "/api/board-categories")
        .methods("POST"_method)
        .CROW_MIDDLEWARES(app, JwtMiddleware, SpaceMiddleware)([this, &app](const crow::request &req) -> crow::response
                                                               {
            const auto &spaceCtx = app.get_context<SpaceMiddleware>(req);
            AutomationLoader automationLoader(spaceCtx.space_id);
            auto body = crow::json::load(req.body);
            if (!body)
                return HttpResponses::invalid_json();
            auto [isValid, errors] = BoardCategoryCreateDto::validate(body);
            if (!isValid)
                return HttpResponses::invalid_payload(errors);
            auto dto = BoardCategoryCreateDto::from_json(body);
            try
            {
                auto result = board_category_module_->get_contexted_service(spaceCtx.space_id).create_board_category(dto.value());
                return HttpResponses::json_ok(result.to_json().dump());
            }
            catch (const std::exception &e)
            {
                spdlog::error("Exception creating board category: {}", e.what());
                return HttpResponses::internal_server_error(std::string("Internal server error: ") + e.what());
            }
            catch (...)
            {
                return HttpResponses::internal_server_error();
            } });

    CROW_ROUTE(app, "/api/board-categories/<int>")
        .methods("PUT"_method)
        .CROW_MIDDLEWARES(app, JwtMiddleware, SpaceMiddleware, PermissionMiddleware)([this, &app](const crow::request &req, long int cat_id) -> crow::response
                                                                                     {
            const auto &spaceCtx = app.get_context<SpaceMiddleware>(req);
            const auto &permCtx = app.get_context<PermissionMiddleware>(req);
            AutomationLoader automationLoader(spaceCtx.space_id);
            auto body = crow::json::load(req.body);
            if (!body)
                return HttpResponses::invalid_json();
            auto [isValid, errors] = BoardCategoryUpdateDto::validate(body);
            if (!isValid)
                return HttpResponses::invalid_payload(errors);
            auto dto = BoardCategoryUpdateDto::from_json(body);
            dto.value().id = cat_id;
            try
            {
                auto result = board_category_module_->get_contexted_service(spaceCtx.space_id, permCtx.permissions).update_board_category_secured(dto.value());
                return HttpResponses::json_ok(result.to_json().dump());
            }
            catch (const std::exception &e)
            {
                spdlog::error("Exception updating board category {}: {}", cat_id, e.what());
                return HttpResponses::internal_server_error(std::string("Internal server error: ") + e.what());
            }
            catch (...)
            {
                return HttpResponses::internal_server_error();
            } });

    CROW_ROUTE(app, "/api/board-categories/<int>")
        .methods("DELETE"_method)
        .CROW_MIDDLEWARES(app, JwtMiddleware, SpaceMiddleware)([this, &app](const crow::request &req, long int cat_id) -> crow::response
                                                               {
            const auto &spaceCtx = app.get_context<SpaceMiddleware>(req);
            AutomationLoader automationLoader(spaceCtx.space_id);
            if (spaceCtx.user_id != spaceCtx.admin_id)
                return HttpResponses::forbidden();
            try
            {
                board_category_module_->get_contexted_service(spaceCtx.space_id).delete_board_category_by_id(cat_id);
                return HttpResponses::ok("BoardCategory deleted successfully");
            }
            catch (const std::exception &e)
            {
                spdlog::error("Exception deleting board category {}: {}", cat_id, e.what());
                return HttpResponses::internal_server_error(std::string("Internal server error: ") + e.what());
            }
            catch (...)
            {
                return HttpResponses::internal_server_error();
            } });
}
