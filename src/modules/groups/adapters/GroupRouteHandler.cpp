#include "modules/groups/adapters/GroupRouteHandler.hpp"

#include "crow.h"
#include "shared/adapters/SpaceGuard.hpp"

#include <exception>
#include <string>
#include <vector>
#include <spdlog/spdlog.h>

#include "modules/groups/adapters/dto/GroupDto.hpp"
#include "shared/adapters/http/response/HttpResponses.hpp"
#include "shared/adapters/http/request/HttpRequestParams.hpp"

void GroupRouteHandler::setup(crow::App<JwtMiddleware, SpaceMiddleware, PermissionMiddleware> &app)
{
    CROW_ROUTE(app, "/api/groups")
        .methods("GET"_method)
        .CROW_MIDDLEWARES(app, JwtMiddleware, SpaceMiddleware)([this, &app](const crow::request &req) -> crow::response
                                                               {
            const auto &spaceCtx = app.get_context<SpaceMiddleware>(req);

            auto relations = HttpRequestParams::parse_csv_param(req, "relations");

            try
            {
                auto groups = group_module_->get_contexted_service(spaceCtx.space_id).get_all_groups_query(relations);
                std::vector<crow::json::wvalue> groupList;
                groupList.reserve(groups.size());
                for (const auto &group : groups)
                {
                    groupList.push_back(group.to_json());
                }
                crow::json::wvalue responseBody(groupList);
                return HttpResponses::json_ok(responseBody.dump());
            }
            catch (const std::exception &e)
            {
                spdlog::error("Exception fetching groups: {}", e.what());
                return HttpResponses::internal_server_error(std::string("Internal server error: ") + e.what());
            }
            catch (...)
            {
                spdlog::error("Unknown error fetching groups");
                return HttpResponses::internal_server_error();
            } });

    CROW_ROUTE(app, "/api/groups/<int>")
        .methods("GET"_method)
        .CROW_MIDDLEWARES(app, JwtMiddleware, SpaceMiddleware)([this, &app](const crow::request &req, long int group_id) -> crow::response
                                                               {
            const auto &spaceCtx = app.get_context<SpaceMiddleware>(req);

            auto relations = HttpRequestParams::parse_csv_param(req, "relations");

            try
            {
                auto group = group_module_->get_contexted_service(spaceCtx.space_id).get_group_by_id_query(group_id, relations);
                if (!group.has_value())
                {
                    return HttpResponses::not_found("Group not found");
                }
                return HttpResponses::json_ok(group.value().to_json().dump());
            }
            catch (const std::exception &e)
            {
                spdlog::error("Exception fetching group {}: {}", group_id, e.what());
                return HttpResponses::internal_server_error(std::string("Internal server error: ") + e.what());
            }
            catch (...)
            {
                spdlog::error("Unknown error fetching group {}", group_id);
                return HttpResponses::internal_server_error();
            } });

    CROW_ROUTE(app, "/api/groups")
        .methods("POST"_method)
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
            auto [isValid, errors] = GroupCreateDto::validate(body);
            if (!isValid)
            {
                return HttpResponses::invalid_payload(errors);
            }

            try
            {
                auto dto = GroupCreateDto::from_json(body);
                group_module_->get_contexted_service(spaceCtx.space_id).create_group(dto.value());
                return HttpResponses::created("Group created successfully");
            }
            catch (const std::exception &e)
            {
                spdlog::error("Exception creating group: {}", e.what());
                return HttpResponses::internal_server_error(std::string("Internal server error: ") + e.what());
            }
            catch (...)
            {
                spdlog::error("Unknown error creating group");
                return HttpResponses::internal_server_error();
            } });

    CROW_ROUTE(app, "/api/groups/<int>")
        .methods("PUT"_method)
        .CROW_MIDDLEWARES(app, JwtMiddleware, SpaceMiddleware)([this, &app](const crow::request &req, long int group_id) -> crow::response
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
            crow::json::wvalue body_w(body);
            body_w["id"] = group_id;
            auto body_with_id = crow::json::load(body_w.dump());

            auto [isValid, errors] = GroupUpdateDto::validate(body_with_id);
            if (!isValid)
            {
                return HttpResponses::invalid_payload(errors);
            }

            try
            {
                auto dto = GroupUpdateDto::from_json(body_with_id);
                group_module_->get_contexted_service(spaceCtx.space_id).update_group(dto.value());
                return HttpResponses::ok("Group updated successfully");
            }
            catch (const std::exception &e)
            {
                spdlog::error("Exception updating group {}: {}", group_id, e.what());
                return HttpResponses::internal_server_error(std::string("Internal server error: ") + e.what());
            }
            catch (...)
            {
                spdlog::error("Unknown error updating group {}", group_id);
                return HttpResponses::internal_server_error();
            } });

    CROW_ROUTE(app, "/api/groups/<int>")
        .methods("DELETE"_method)
        .CROW_MIDDLEWARES(app, JwtMiddleware, SpaceMiddleware)([this, &app](const crow::request &req, long int group_id) -> crow::response
                                                               {
            const auto &spaceCtx = app.get_context<SpaceMiddleware>(req);

            if (spaceCtx.user_id != spaceCtx.admin_id)
            {
                return HttpResponses::forbidden();
            }

            try
            {
                group_module_->get_contexted_service(spaceCtx.space_id).delete_group_by_id(group_id);
                return HttpResponses::ok("Group deleted successfully");
            }
            catch (const std::exception &e)
            {
                spdlog::error("Exception deleting group {}: {}", group_id, e.what());
                return HttpResponses::internal_server_error(std::string("Internal server error: ") + e.what());
            }
            catch (...)
            {
                spdlog::error("Unknown error deleting group {}", group_id);
                return HttpResponses::internal_server_error();
            } });

    CROW_ROUTE(app, "/api/groups/<int>/members")
        .methods("POST"_method)
        .CROW_MIDDLEWARES(app, JwtMiddleware, SpaceMiddleware)([this, &app](const crow::request &req, long int group_id) -> crow::response
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
            auto [isValid, errors] = GroupMembersDto::validate(body);
            if (!isValid)
            {
                return HttpResponses::invalid_payload(errors);
            }

            try
            {
                auto dto = GroupMembersDto::from_json(body);
                group_module_->get_contexted_service(spaceCtx.space_id).add_group_members(group_id, dto.value().member_ids);
                return HttpResponses::ok("Members added successfully");
            }
            catch (const std::exception &e)
            {
                spdlog::error("Exception adding members to group {}: {}", group_id, e.what());
                return HttpResponses::internal_server_error(std::string("Internal server error: ") + e.what());
            }
            catch (...)
            {
                spdlog::error("Unknown error adding members to group {}", group_id);
                return HttpResponses::internal_server_error();
            } });

    CROW_ROUTE(app, "/api/groups/<int>/members")
        .methods("DELETE"_method)
        .CROW_MIDDLEWARES(app, JwtMiddleware, SpaceMiddleware)([this, &app](const crow::request &req, long int group_id) -> crow::response
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
            auto [isValid, errors] = GroupMembersDto::validate(body);
            if (!isValid)
            {
                return HttpResponses::invalid_payload(errors);
            }

            try
            {
                auto dto = GroupMembersDto::from_json(body);
                group_module_->get_contexted_service(spaceCtx.space_id).remove_group_members(group_id, dto.value().member_ids);
                return HttpResponses::ok("Members removed successfully");
            }
            catch (const std::exception &e)
            {
                spdlog::error("Exception removing members from group {}: {}", group_id, e.what());
                return HttpResponses::internal_server_error(std::string("Internal server error: ") + e.what());
            }
            catch (...)
            {
                spdlog::error("Unknown error removing members from group {}", group_id);
                return HttpResponses::internal_server_error();
            } });

    CROW_ROUTE(app, "/api/groups/<int>/permissions")
        .methods("PUT"_method)
        .CROW_MIDDLEWARES(app, JwtMiddleware, SpaceMiddleware)([this, &app](const crow::request &req, long int group_id) -> crow::response
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
            crow::json::wvalue body_w(body);
            body_w["group_id"] = group_id;
            auto body_with_id = crow::json::load(body_w.dump());

            auto [isValid, errors] = GroupSetPermissionsForEntityDto::validate(body_with_id);
            if (!isValid)
            {
                return HttpResponses::invalid_payload(errors);
            }

            try
            {
                auto dto = GroupSetPermissionsForEntityDto::from_json(body_with_id);
                group_module_->get_contexted_service(spaceCtx.space_id).set_group_entity_permissions(group_id, dto.value().entity, dto.value().permissions);
                return HttpResponses::ok("Permissions updated successfully");
            }
            catch (const std::exception &e)
            {
                spdlog::error("Exception setting permissions for group {}: {}", group_id, e.what());
                return HttpResponses::internal_server_error(std::string("Internal server error: ") + e.what());
            }
            catch (...)
            {
                spdlog::error("Unknown error setting permissions for group {}", group_id);
                return HttpResponses::internal_server_error();
            } });
}
