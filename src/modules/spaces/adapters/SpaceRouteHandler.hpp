#pragma once

#include "crow.h"
#include "modules/spaces/ports/ISpaceService.hpp"
#include "shared/adapters/JwtGuard.hpp"
#include "shared/adapters/SpaceGuard.hpp"

#include <memory>

/** @brief Registers HTTP endpoints for space workflows. */
class SpaceRouteHandler
{
private:
    std::shared_ptr<ISpaceService> spaceService_;

public:
    explicit SpaceRouteHandler(std::shared_ptr<ISpaceService> spaceService)
        : spaceService_(std::move(spaceService))
    {
    }

    /** @brief Binds all space routes and middleware-aware handlers to app. */
    void setup(crow::App<JwtMiddleware, SpaceMiddleware> &app);
};
