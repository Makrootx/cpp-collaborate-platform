#include "shared/adapters/SpaceGuard.hpp"

template <typename AllContext>
void SpaceMiddleware::before_handle(crow::request &req,
                                    crow::response &res,
                                    context &ctx,
                                    AllContext &all_ctx)
{
    const JwtMiddleware::context &jwtCtx = all_ctx.template get<JwtMiddleware>();

    const std::string &spaceIdHeader = req.get_header_value("X-Space-ID");
    if (spaceIdHeader.empty())
    {
        res = HttpResponses::bad_request("Missing X-Space-ID header");
        res.end();
        return;
    }

    long int spaceId = std::stol(spaceIdHeader);
    const auto space = space_repo_->find_by_id_query(spaceId, {"members"});
    if (!space.has_value())
    {
        res = HttpResponses::not_found("Space not found");
        res.end();
        return;
    }

    if (space->user_has_access(jwtCtx.user_id.has_value() ? std::stol(jwtCtx.user_id.value()) : -1))
    {
        ctx.user_id = std::stol(jwtCtx.user_id.value());
        ctx.admin_id = space->get_owner().get_id();
        ctx.space_id = spaceId;
    }
    else
    {
        res = HttpResponses::forbidden("Access denied");
        res.end();
        return;
    }
}