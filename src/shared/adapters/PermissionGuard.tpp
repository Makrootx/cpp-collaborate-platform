#include "shared/adapters/PermissionGuard.hpp"

template <typename AllContext>
void PermissionMiddleware::before_handle(crow::request &req,
                                         crow::response &res,
                                         context &ctx,
                                         AllContext &all_ctx)
{
    const SpaceMiddleware::context &spaceCtx = all_ctx.template get<SpaceMiddleware>();

    if (spaceCtx.user_id == spaceCtx.admin_id)
    {
        spdlog::debug("User {} is admin of space {}, granting all permissions", spaceCtx.user_id, spaceCtx.space_id);
        auto perms = PermissionCore::create_default_permissions(true);
        PermissionCore::populate_missing(perms);
        ctx.permissions = perms;
        spdlog::debug("After aggregation permissions for user {}: {}", spaceCtx.user_id, ctx.permissions.to_json().dump());
        return;
    }
    std::vector<GroupPermission> permissions_to_aggregate;
    auto groupRepo = group_module_->get_contexted_repo("space_" + std::to_string(spaceCtx.space_id));
    auto groups = groupRepo.get_all_query({"members"});
    for (const auto &group : groups)
    {
        if (std::ranges::any_of(group.get_members(), [&](const auto &m)
                                { return m.get_id() == spaceCtx.user_id; }))
        {
            permissions_to_aggregate.push_back(group.get_permissions());
        }
    }

    GroupPermission effective_perm = std::accumulate(
        permissions_to_aggregate.begin(),
        permissions_to_aggregate.end(),
        GroupPermission{.entities = {}},
        [](GroupPermission acc, const GroupPermission &next)
        {
            acc = PermissionCore::merge(acc, next);
            return acc;
        });

    spdlog::debug("Aggregated permissions for user {}: {}", spaceCtx.user_id, effective_perm.to_json().dump());
    PermissionCore::populate_missing(effective_perm);

    spdlog::debug("After aggregation permissions for user {}: {}", spaceCtx.user_id, effective_perm.to_json().dump());
    ctx.permissions = std::move(effective_perm);
}