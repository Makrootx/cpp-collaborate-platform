#pragma once

#include "modules/spaces/ports/ISpaceRepo.hpp"
#include "shared/adapters/JwtGuard.hpp"
#include "shared/adapters/http/response/HttpResponses.hpp"

#include "crow.h"
#include <utility>
#include <memory>

/**
 * @brief Middleware that authorizes access to a space resource.
 *
 * Requires valid JWT context and verifies requested user is allowed to access
 * the target space.
 */
struct SpaceMiddleware : public crow::ILocalMiddleware
{
    /** @brief Request-scoped authorization context for space access checks. */
    struct context
    {
        long int admin_id;
        long int user_id;
        long int space_id;
    };

    SpaceMiddleware() = default;
    SpaceMiddleware(std::shared_ptr<ISpaceRepo> space_repo) : space_repo_(std::move(space_repo)) {}

    /** @brief Validates space_id and enforces membership/ownership access rules. */
    template <typename AllContext>
    void before_handle(crow::request &req,
                       crow::response &res,
                       context &ctx, AllContext &all_ctx);

    void after_handle(crow::request &, crow::response &, context &) {}

protected:
    std::shared_ptr<ISpaceRepo> space_repo_;
};

#include "SpaceGuard.tpp"