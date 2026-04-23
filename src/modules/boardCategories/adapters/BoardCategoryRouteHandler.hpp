#pragma once

#include "crow.h"
#include "shared/adapters/JwtGuard.hpp"
#include "shared/adapters/SpaceGuard.hpp"
#include "shared/adapters/PermissionGuard.hpp"
#include "modules/boardCategories/adapters/BoardCategoryModule.hpp"
#include <memory>

class BoardCategoryRouteHandler
{
private:
    std::shared_ptr<BoardCategoryModule> board_category_module_;

public:
    explicit BoardCategoryRouteHandler(std::shared_ptr<BoardCategoryModule> board_category_module)
        : board_category_module_(std::move(board_category_module)) {}

    void setup(crow::App<JwtMiddleware, SpaceMiddleware, PermissionMiddleware> &app);
};
