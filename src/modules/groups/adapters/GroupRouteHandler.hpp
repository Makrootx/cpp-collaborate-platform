#pragma once

#include "crow.h"
#include "shared/adapters/JwtGuard.hpp"
#include "shared/adapters/SpaceGuard.hpp"
#include "shared/adapters/PermissionGuard.hpp"
#include "modules/groups/adapters/GroupModule.hpp"

#include <memory>

class GroupRouteHandler
{
private:
    std::shared_ptr<GroupModule> group_module_;

public:
    explicit GroupRouteHandler(std::shared_ptr<GroupModule> group_module)
        : group_module_(std::move(group_module))
    {
    }

    void setup(crow::App<JwtMiddleware, SpaceMiddleware, PermissionMiddleware> &app);
};
