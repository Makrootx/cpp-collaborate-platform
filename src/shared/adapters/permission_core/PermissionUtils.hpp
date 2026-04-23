#pragma once

#include <optional>
#include <string_view>

#include "shared/adapters/permission_core/PermissionStruct.hpp"
#include "shared/helpers/entity_schema.hpp"

namespace PermissionCore
{
    template <typename Schema>
        requires EntitySchemas::HasEntitySchema<Schema>
    bool has_attribute(std::string_view attr);

    GroupPermission create_default_permissions(bool default_access = false);

    template <typename EntitySchema>
        requires EntitySchemas::HasEntitySchema<EntitySchema>
    EntityPermission populate_missing(EntityPermission &perm);

    GroupPermission populate_missing(GroupPermission &perm);

    std::optional<bool> get_merged_access(const std::optional<bool> &a, const std::optional<bool> &b);
    AttributePermission merge(AttributePermission a, AttributePermission b);
    RelationPermission merge(RelationPermission a, RelationPermission b);
    EntityPermission merge(EntityPermission a, EntityPermission b);
    GroupPermission merge(GroupPermission a, GroupPermission b);
}

#include "PermissionUtils.tpp"