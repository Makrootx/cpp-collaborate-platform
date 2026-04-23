#include "modules/tasks/adapters/TaskRouteHandler.hpp"

#include "crow.h"
#include "shared/adapters/SpaceGuard.hpp"

#include <exception>
#include <string>
#include <vector>
#include <ranges>
#include <spdlog/spdlog.h>

#include "modules/tasks/adapters/dto/TaskDto.hpp"
#include "shared/adapters/http/response/HttpResponses.hpp"
#include "shared/adapters/http/request/HttpRequestParams.hpp"

void TaskRouteHandler::setup(crow::App<JwtMiddleware, SpaceMiddleware, PermissionMiddleware> &app)
{
    CROW_ROUTE(app, "/api/tasks")
        .methods("POST"_method)
        .CROW_MIDDLEWARES(app, JwtMiddleware, SpaceMiddleware)([this, &app](const crow::request &req) -> crow::response
                                                               {
            const auto &ctx = app.get_context<JwtMiddleware>(req);
            const auto &spaceCtx = app.get_context<SpaceMiddleware>(req);

            auto body = crow::json::load(req.body);
            if (!body)
            {
                return HttpResponses::invalid_json();
            }
            auto [isValid, errors] = TaskCreateDto::validate(body);
            if (!isValid)
            {
                return HttpResponses::invalid_payload(errors);
            }
            auto taskCreateDto = TaskCreateDto::from_json(body);
            task_module_->get_contexted_service(spaceCtx.space_id).create_task(taskCreateDto.value());
            return HttpResponses::created("Task created successfully"); });

    CROW_ROUTE(app, "/api/tasks/<int>")
        .methods("GET"_method)
        .CROW_MIDDLEWARES(app, JwtMiddleware, SpaceMiddleware, PermissionMiddleware)([this, &app](const crow::request &req, long int task_id) -> crow::response
                                                                                     {
        const auto &ctx = app.get_context<JwtMiddleware>(req);
        const auto &spaceCtx = app.get_context<SpaceMiddleware>(req);
        const auto &permCtx = app.get_context<PermissionMiddleware>(req);

        auto automationLoader = AutomationLoader(spaceCtx.space_id);

        auto relations = HttpRequestParams::parse_csv_param(req, "relations");

        try
        {
            auto task_dto = task_module_->get_contexted_service(spaceCtx.space_id, permCtx.permissions).get_task_secured_by_id_query(task_id, relations);
            if (!task_dto.has_value())
            {
                return HttpResponses::not_found("Task not found");
            }
            return HttpResponses::json_ok(task_dto.value().to_json().dump());
        }
        catch (const std::exception &e)
        {
            spdlog::error("Exception fetching task {}: {}", task_id, e.what());
            return HttpResponses::internal_server_error(std::string("Internal server error: ") + e.what());
        }
        catch (...)
        {
            spdlog::error("Unknown error fetching task {}", task_id);
            return HttpResponses::internal_server_error();
        } });

    CROW_ROUTE(app, "/api/tasks")
        .methods("GET"_method)
        .CROW_MIDDLEWARES(app, JwtMiddleware, SpaceMiddleware, PermissionMiddleware)([this, &app](const crow::request &req) -> crow::response
                                                                                     {
        const auto &ctx = app.get_context<JwtMiddleware>(req);
        const auto &spaceCtx = app.get_context<SpaceMiddleware>(req);
        const auto &permCtx = app.get_context<PermissionMiddleware>(req);

        auto automationLoader = AutomationLoader(spaceCtx.space_id);

        auto relations = HttpRequestParams::parse_csv_param(req, "relations");

        try
        {
            auto tasks = task_module_->get_contexted_service(spaceCtx.space_id, permCtx.permissions).get_all_tasks_query_secured(relations);
            std::vector<crow::json::wvalue> taskList;
            taskList.reserve(tasks.size());
            for (const auto &task : tasks)
            {
                taskList.push_back(task.to_json());
            }
            crow::json::wvalue responseBody(taskList);
            return HttpResponses::json_ok(responseBody.dump());
        }
        catch (const std::exception &e)
        {
            spdlog::error("Exception fetching tasks: {}", e.what());
            return HttpResponses::internal_server_error(std::string("Internal server error: ") + e.what());
        }
        catch (...)
        {
            spdlog::error("Unknown error fetching tasks");
            return HttpResponses::internal_server_error();
        } });

    CROW_ROUTE(app, "/api/tasks/<int>")
        .methods("PUT"_method)
        .CROW_MIDDLEWARES(app, JwtMiddleware, SpaceMiddleware, PermissionMiddleware)([this, &app](const crow::request &req, long int task_id) -> crow::response
                                                                                     {
        const auto &ctx = app.get_context<JwtMiddleware>(req);
        const auto &spaceCtx = app.get_context<SpaceMiddleware>(req);
        const auto &permCtx = app.get_context<PermissionMiddleware>(req);

        auto automationLoader = AutomationLoader(spaceCtx.space_id);

        auto body = crow::json::load(req.body);
        if (!body)
        {
            return HttpResponses::invalid_json();
        }
        crow::json::wvalue body_w(body);
        body_w["id"] = task_id;
        auto body_with_id = crow::json::load(body_w.dump());

        auto [isValid, errors] = TaskUpdateDto::validate(body_with_id);
        if (!isValid)
        {
            return HttpResponses::invalid_payload(errors);
        }

        try
        {
            auto taskUpdateDto = TaskUpdateDto::from_json(body_with_id);
            auto result = task_module_->get_contexted_service(spaceCtx.space_id, permCtx.permissions).update_task_secured(taskUpdateDto.value());
            return HttpResponses::json_ok(result.to_json().dump());
        }
        catch (const std::exception &e)
        {
            spdlog::error("Exception updating task {}: {}", task_id, e.what());
            return HttpResponses::internal_server_error(std::string("Internal server error: ") + e.what());
        }
        catch (...)
        {
            spdlog::error("Unknown error updating task {}", task_id);
            return HttpResponses::internal_server_error();
        } });

    CROW_ROUTE(app, "/api/tasks/<int>")
        .methods("DELETE"_method)
        .CROW_MIDDLEWARES(app, JwtMiddleware, SpaceMiddleware, PermissionMiddleware)([this, &app](const crow::request &req, long int task_id) -> crow::response
                                                                                     {
        const auto &ctx = app.get_context<JwtMiddleware>(req);
        const auto &spaceCtx = app.get_context<SpaceMiddleware>(req);
        const auto &permCtx = app.get_context<PermissionMiddleware>(req);

        auto automationLoader = AutomationLoader(spaceCtx.space_id);

        try
        {
            task_module_->get_contexted_service(spaceCtx.space_id, permCtx.permissions).delete_task_by_id(task_id);
            return HttpResponses::ok("Task deleted successfully");
        }
        catch (const std::exception &e)
        {
            spdlog::error("Exception deleting task {}: {}", task_id, e.what());
            return HttpResponses::internal_server_error(std::string("Internal server error: ") + e.what());
        }
        catch (...)
        {
            spdlog::error("Unknown error deleting task {}", task_id);
            return HttpResponses::internal_server_error();
        } });
}
