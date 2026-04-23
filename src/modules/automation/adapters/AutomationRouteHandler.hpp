#pragma once

#include "crow.h"
#include "shared/adapters/JwtGuard.hpp"
#include "shared/adapters/SpaceGuard.hpp"
#include "shared/adapters/PermissionGuard.hpp"
#include "modules/automation/adapters/AutomationModule.hpp"

#include <memory>

class AutomationRouteHandler
{
    std::shared_ptr<AutomationModule> automation_module_;

public:
    explicit AutomationRouteHandler(std::shared_ptr<AutomationModule> automation_module)
        : automation_module_(std::move(automation_module)) {}

    void setup(crow::App<JwtMiddleware, SpaceMiddleware, PermissionMiddleware> &app);
};
