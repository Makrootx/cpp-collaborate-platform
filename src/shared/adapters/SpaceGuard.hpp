#pragma once

// #include "shared/adapters/JwtAuthenticatorService.hpp"
#include "modules/spaces/ports/ISpaceRepo.hpp"
#include "shared/adapters/JwtGuard.hpp"

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
        std::optional<std::string> user_id;
    };

    SpaceMiddleware() = default;
    SpaceMiddleware(std::shared_ptr<ISpaceRepo> space_repo) : space_repo_(std::move(space_repo)) {}

    /** @brief Validates space_id and enforces membership/ownership access rules. */
    template <typename AllContext>
    void before_handle(crow::request &req,
                       crow::response &res,
                       context &ctx, AllContext &all_ctx)
    {
        const JwtMiddleware::context &jwtCtx = all_ctx.template get<JwtMiddleware>();

        const char *spaceIdRaw = req.url_params.get("space_id");
        if (spaceIdRaw == nullptr)
        {
            res.code = 400;
            res.end();
            return;
        }

        std::string spaceIdValue(spaceIdRaw);

        long int spaceId = std::stol(spaceIdValue);
        const auto space = space_repo_->find_by_id_query(spaceId, {"members"});
        if (!space.has_value())
        {
            res.code = 404;
            res.end();
            return;
        }
        if (space->user_has_access(jwtCtx.user_id.has_value() ? std::stol(jwtCtx.user_id.value()) : -1))
        {
            ctx.user_id = jwtCtx.user_id;
        }
        else
        {
            res.code = 403;
            res.end();
            return;
        }
    }

    void after_handle(crow::request &, crow::response &, context &) {}

protected:
    std::shared_ptr<ISpaceRepo> space_repo_;
};