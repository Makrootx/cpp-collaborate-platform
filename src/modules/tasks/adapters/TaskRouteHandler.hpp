#pragma once

#include "crow.h"
#include "shared/adapters/JwtGuard.hpp"
#include "shared/adapters/SpaceGuard.hpp"
#include "shared/adapters/PermissionGuard.hpp"
#include "modules/tasks/adapters/TaskModule.hpp"
#include "modules/automation/adapters/AutomationLoader.hpp"

#include <memory>

class TaskRouteHandler
{
private:
    std::shared_ptr<TaskModule> task_module_;

public:
    explicit TaskRouteHandler(std::shared_ptr<TaskModule> task_module)
        : task_module_(std::move(task_module))
    {
    }

    void setup(crow::App<JwtMiddleware, SpaceMiddleware, PermissionMiddleware> &app);
};
