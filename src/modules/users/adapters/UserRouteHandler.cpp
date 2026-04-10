#include "modules/users/ports/IUserService.hpp"
#include "shared/adapters/JwtGuard.hpp"

#include "crow.h"
#include <string>
#include <memory>
#include <spdlog/spdlog.h>

#include "modules/users/adapters/UserRouteHandler.hpp"

crow::response prepare_invalid_payload_response(const std::vector<std::string> &errors)
{
    std::string errorMsg = "Invalid request payload. Errors: ";
    for (const auto &error : errors)
    {
        errorMsg += error + "; ";
    }
    return crow::response(400, errorMsg);
};

void UserRouteHandler::setup(crow::App<JwtMiddleware, SpaceMiddleware> &app)
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

            return crow::response(200, responseBody.dump()); });

    // GET user by ID
    CROW_ROUTE(app, "/api/users/<int>")
        .methods("GET"_method)
        .CROW_MIDDLEWARES(app, JwtMiddleware)([this, &app](const crow::request &req, int userId) -> crow::response
                                              {
            const auto &ctx = app.get_context<JwtMiddleware>(req);

            auto userOpt = userService_->get_user_by_id(static_cast<unsigned long>(userId));
            if (!userOpt.has_value())
            {
                return crow::response(404, "User not found");
            }
            return crow::response(200, userOpt->to_json().dump()); });

    // POST login user
    CROW_ROUTE(app, "/api/users/auth").methods("POST"_method)([this](const crow::request &req)
                                                              {
            auto body = crow::json::load(req.body);
            if (!body)
            {
                return crow::response(400, "Invalid JSON");
            }
            auto [isValid, errors] = UserAuthenticateDto::validate(body);
            if (!isValid)
            {
                return prepare_invalid_payload_response(errors);
            }
            
            auto authDto = UserAuthenticateDto::from_json(body);
            auto authResult = userService_->authenticate_user(authDto.value());
            if (!authResult.has_value())
            {
                return crow::response(401, "Invalid email or password");
            }
            
            return crow::response(200, authResult.value()); });

    // POST register user
    CROW_ROUTE(app, "/api/users/register").methods("POST"_method)([this](const crow::request &req)
                                                                  {
            auto body = crow::json::load(req.body);
            if (!body)
            {
                return crow::response(400, "Invalid JSON");
            }
            auto [isValid, errors] = UserCreateDto::validate(body);
            if (!isValid)
            {
                return prepare_invalid_payload_response(errors);
            }
            auto createDto = UserCreateDto::from_json(body);
            userService_->create_user(createDto.value());
            return crow::response(201, "User created successfully"); });

    CROW_ROUTE(app, "/api/users/")
        .methods("POST"_method)
        .CROW_MIDDLEWARES(app, JwtMiddleware)([this, &app](const crow::request &req) -> crow::response
                                              {
            const auto &ctx = app.get_context<JwtMiddleware>(req);
            auto body = crow::json::load(req.body);
            if (!body)
            {
                return crow::response(400, "Invalid JSON");
            }
            crow::json::wvalue body_s(body);
            body_s["id"] = ctx.user_id.value();
            auto user_body = crow::json::load(body_s.dump());
            auto [isValid, errors] = UserUpdateDto::validate(user_body);
            if (!isValid)
            {
                return prepare_invalid_payload_response(errors);
            }
            auto userUpdateDto = UserUpdateDto::from_json(user_body);
            userService_->update_user(userUpdateDto.value());
            return crow::response(200, "User updated successfully"); });
}