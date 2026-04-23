#pragma once

#include "crow.h"
#include "shared/adapters/JwtGuard.hpp"
#include "shared/adapters/SpaceGuard.hpp"
#include "shared/adapters/PermissionGuard.hpp"
#include "modules/boards/adapters/BoardModule.hpp"
#include <memory>

class BoardRouteHandler
{
private:
    std::shared_ptr<BoardModule> board_module_;

public:
    explicit BoardRouteHandler(std::shared_ptr<BoardModule> board_module)
        : board_module_(std::move(board_module)) {}

    void setup(crow::App<JwtMiddleware, SpaceMiddleware, PermissionMiddleware> &app);
};
