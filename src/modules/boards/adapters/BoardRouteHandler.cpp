#include "modules/boards/adapters/BoardRouteHandler.hpp"
#include "crow.h"
#include "shared/adapters/SpaceGuard.hpp"
#include "modules/boards/adapters/dto/BoardDto.hpp"
#include "shared/adapters/http/response/HttpResponses.hpp"
#include "shared/adapters/http/request/HttpRequestParams.hpp"
#include <spdlog/spdlog.h>
#include "modules/automation/adapters/AutomationLoader.hpp"

void BoardRouteHandler::setup(crow::App<JwtMiddleware, SpaceMiddleware, PermissionMiddleware> &app)
{
    CROW_ROUTE(app, "/api/boards")
        .methods("GET"_method)
        .CROW_MIDDLEWARES(app, JwtMiddleware, SpaceMiddleware, PermissionMiddleware)([this, &app](const crow::request &req) -> crow::response
                                                                                     {
            const auto &spaceCtx = app.get_context<SpaceMiddleware>(req);
            const auto &permCtx = app.get_context<PermissionMiddleware>(req);
            AutomationLoader automationLoader(spaceCtx.space_id);
            auto relations = HttpRequestParams::parse_csv_param(req, "relations");
            try
            {
                auto boards = board_module_->get_contexted_service(spaceCtx.space_id, permCtx.permissions).get_all_boards_query_secured(relations);
                std::vector<crow::json::wvalue> list;
                list.reserve(boards.size());
                for (const auto &b : boards)
                    list.push_back(b.to_json());
                crow::json::wvalue response(list);
                return HttpResponses::json_ok(response.dump());
            }
            catch (const std::exception &e)
            {
                spdlog::error("Exception fetching boards: {}", e.what());
                return HttpResponses::internal_server_error(std::string("Internal server error: ") + e.what());
            }
            catch (...)
            {
                return HttpResponses::internal_server_error();
            } });

    CROW_ROUTE(app, "/api/boards/<int>")
        .methods("GET"_method)
        .CROW_MIDDLEWARES(app, JwtMiddleware, SpaceMiddleware, PermissionMiddleware)([this, &app](const crow::request &req, long int board_id) -> crow::response
                                                                                     {
            const auto &spaceCtx = app.get_context<SpaceMiddleware>(req);
            const auto &permCtx = app.get_context<PermissionMiddleware>(req);
            AutomationLoader automationLoader(spaceCtx.space_id);
            auto relations = HttpRequestParams::parse_csv_param(req, "relations");
            try
            {
                auto board = board_module_->get_contexted_service(spaceCtx.space_id, permCtx.permissions).get_board_secured_by_id_query(board_id, relations);
                return HttpResponses::json_ok(board.to_json().dump());
            }
            catch (const std::exception &e)
            {
                spdlog::error("Exception fetching board {}: {}", board_id, e.what());
                return HttpResponses::not_found(std::string("Board not found: ") + e.what());
            }
            catch (...)
            {
                return HttpResponses::internal_server_error();
            } });

    CROW_ROUTE(app, "/api/boards")
        .methods("POST"_method)
        .CROW_MIDDLEWARES(app, JwtMiddleware, SpaceMiddleware)([this, &app](const crow::request &req) -> crow::response
                                                               {
            const auto &spaceCtx = app.get_context<SpaceMiddleware>(req);
            AutomationLoader automationLoader(spaceCtx.space_id);
            auto body = crow::json::load(req.body);
            if (!body)
                return HttpResponses::invalid_json();
            auto [isValid, errors] = BoardCreateDto::validate(body);
            if (!isValid)
                return HttpResponses::invalid_payload(errors);
            auto dto = BoardCreateDto::from_json(body);
            try
            {
                auto result = board_module_->get_contexted_service(spaceCtx.space_id).create_board(dto.value());
                return HttpResponses::json_ok(result.to_json().dump());
            }
            catch (const std::exception &e)
            {
                spdlog::error("Exception creating board: {}", e.what());
                return HttpResponses::internal_server_error(std::string("Internal server error: ") + e.what());
            }
            catch (...)
            {
                return HttpResponses::internal_server_error();
            } });

    CROW_ROUTE(app, "/api/boards/<int>")
        .methods("PUT"_method)
        .CROW_MIDDLEWARES(app, JwtMiddleware, SpaceMiddleware, PermissionMiddleware)([this, &app](const crow::request &req, long int board_id) -> crow::response
                                                                                     {
            const auto &spaceCtx = app.get_context<SpaceMiddleware>(req);
            const auto &permCtx = app.get_context<PermissionMiddleware>(req);
            AutomationLoader automationLoader(spaceCtx.space_id);
            auto body = crow::json::load(req.body);
            if (!body)
                return HttpResponses::invalid_json();
            auto [isValid, errors] = BoardUpdateDto::validate(body);
            if (!isValid)
                return HttpResponses::invalid_payload(errors);
            auto dto = BoardUpdateDto::from_json(body);
            dto.value().id = board_id;
            try
            {
                auto result = board_module_->get_contexted_service(spaceCtx.space_id, permCtx.permissions).update_board_secured(dto.value());
                return HttpResponses::json_ok(result.to_json().dump());
            }
            catch (const std::exception &e)
            {
                spdlog::error("Exception updating board {}: {}", board_id, e.what());
                return HttpResponses::internal_server_error(std::string("Internal server error: ") + e.what());
            }
            catch (...)
            {
                return HttpResponses::internal_server_error();
            } });

    CROW_ROUTE(app, "/api/boards/<int>")
        .methods("DELETE"_method)
        .CROW_MIDDLEWARES(app, JwtMiddleware, SpaceMiddleware)([this, &app](const crow::request &req, long int board_id) -> crow::response
                                                               {
            const auto &spaceCtx = app.get_context<SpaceMiddleware>(req);
            AutomationLoader automationLoader(spaceCtx.space_id);
            if (spaceCtx.user_id != spaceCtx.admin_id)
                return HttpResponses::forbidden();
            try
            {
                board_module_->get_contexted_service(spaceCtx.space_id).delete_board_by_id(board_id);
                return HttpResponses::ok("Board deleted successfully");
            }
            catch (const std::exception &e)
            {
                spdlog::error("Exception deleting board {}: {}", board_id, e.what());
                return HttpResponses::internal_server_error(std::string("Internal server error: ") + e.what());
            }
            catch (...)
            {
                return HttpResponses::internal_server_error();
            } });
}
