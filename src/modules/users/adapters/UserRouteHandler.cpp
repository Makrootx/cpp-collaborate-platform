#include "modules/users/ports/IUserService.hpp"
#include "shared/adapters/JwtGuard.hpp"

#include "crow.h"
#include <string>
#include <memory>
#include <spdlog/spdlog.h>

#include "modules/users/adapters/UserRouteHandler.hpp"
#include "shared/adapters/http/response/HttpResponses.hpp"

void UserRouteHandler::setup(crow::App<JwtMiddleware, SpaceMiddleware, PermissionMiddleware> &app)
{
    // GET all users
    CROW_ROUTE(app, "/api/users")
        .methods("GET"_method)
        .CROW_MIDDLEWARES(app, JwtMiddleware)([this, &app](const crow::request &req) -> crow::response
                                              {
            const auto &ctx = app.get_context<JwtMiddleware>(req);

            auto users = userService_->get_all_users();
            std::vector<crow::json::wvalue> userList;
            for (const auto &user : users)
            {
                userList.push_back(user.to_json());
            }

            crow::json::wvalue responseBody(userList);

            return HttpResponses::json_ok(responseBody.dump()); });

    // GET user by ID
    CROW_ROUTE(app, "/api/users/<int>")
        .methods("GET"_method)
        .CROW_MIDDLEWARES(app, JwtMiddleware)([this, &app](const crow::request &req, int userId) -> crow::response
                                              {
            const auto &ctx = app.get_context<JwtMiddleware>(req);

            auto userOpt = userService_->get_user_by_id(static_cast<unsigned long>(userId));
            if (!userOpt.has_value())
            {
                return HttpResponses::not_found("User not found");
            }
            return HttpResponses::json_ok(userOpt->to_json().dump()); });

    // POST login user
    CROW_ROUTE(app, "/api/users/auth").methods("POST"_method)([this](const crow::request &req)
                                                              {
            auto body = crow::json::load(req.body);
            if (!body)
            {
                return HttpResponses::invalid_json();
            }
            auto [isValid, errors] = UserAuthenticateDto::validate(body);
            if (!isValid)
            {
                return HttpResponses::invalid_payload(errors);
            }
            
            auto authDto = UserAuthenticateDto::from_json(body);
            auto authResult = userService_->authenticate_user(authDto.value());
            if (!authResult.has_value())
            {
                return HttpResponses::unauthorized("Invalid email or password");
            }
            
            return HttpResponses::ok(authResult.value()); });

    // POST register user
    CROW_ROUTE(app, "/api/users/register").methods("POST"_method)([this](const crow::request &req)
                                                                  {
            auto body = crow::json::load(req.body);
            if (!body)
            {
                return HttpResponses::invalid_json();
            }
            auto [isValid, errors] = UserCreateDto::validate(body);
            if (!isValid)
            {
                return HttpResponses::invalid_payload(errors);
            }
            auto createDto = UserCreateDto::from_json(body);
            userService_->create_user(createDto.value());
            return HttpResponses::created("User created successfully"); });

    CROW_ROUTE(app, "/api/users/")
        .methods("POST"_method)
        .CROW_MIDDLEWARES(app, JwtMiddleware)([this, &app](const crow::request &req) -> crow::response
                                              {
            const auto &ctx = app.get_context<JwtMiddleware>(req);
            auto body = crow::json::load(req.body);
            if (!body)
            {
                return HttpResponses::invalid_json();
            }
            crow::json::wvalue body_s(body);
            body_s["id"] = ctx.user_id.value();
            auto user_body = crow::json::load(body_s.dump());
            auto [isValid, errors] = UserUpdateDto::validate(user_body);
            if (!isValid)
            {
                return HttpResponses::invalid_payload(errors);
            }
            auto userUpdateDto = UserUpdateDto::from_json(user_body);
            userService_->update_user(userUpdateDto.value());
            return HttpResponses::ok("User updated successfully"); });
}
