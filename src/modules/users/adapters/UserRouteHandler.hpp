#pragma once
#include "modules/users/ports/IUserService.hpp"
#include "shared/adapters/JwtGuard.hpp"
#include "shared/adapters/PermissionGuard.hpp"
#include "shared/adapters/SpaceGuard.hpp"
#include "crow.h"

/** @brief Registers HTTP endpoints for user workflows. */
class UserRouteHandler
{
private:
    std::shared_ptr<IUserService> userService_;

public:
    explicit UserRouteHandler(std::shared_ptr<IUserService> userService)
        : userService_(std::move(userService))
    {
    }

    /** @brief Binds all user routes and middleware-aware handlers to app. */
    void setup(crow::App<JwtMiddleware, SpaceMiddleware, PermissionMiddleware> &app);
};