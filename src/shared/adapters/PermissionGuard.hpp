#pragma once

// #include "shared/adapters/JwtAuthenticatorService.hpp"
#include "modules/groups/ports/IGroupRepo.hpp"
#include "modules/groups/adapters/GroupModule.hpp"
#include "shared/adapters/SpaceGuard.hpp"
#include "shared/adapters/permission_core/PermissionUtils.hpp"

#include "crow.h"
#include <utility>
#include <memory>
#include <numeric>

struct PermissionMiddleware : public crow::ILocalMiddleware
{
protected:
    std::shared_ptr<GroupModule> group_module_;

public:
    struct context
    {
        GroupPermission permissions;
    };

    PermissionMiddleware() = default;
    PermissionMiddleware(std::shared_ptr<GroupModule> group_module) : group_module_(std::move(group_module)) {}

    template <typename AllContext>
    void before_handle(crow::request &req,
                       crow::response &res,
                       context &ctx, AllContext &all_ctx);

    void after_handle(crow::request &, crow::response &, context &) {}
};

#include "PermissionGuard.tpp"
