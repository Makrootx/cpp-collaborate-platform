#include "shared/adapters/permission_core/PermissionUtils.hpp"

namespace PermissionCore
{
    GroupPermission create_default_permissions(bool default_access)
    {
        GroupPermission perm;
        perm.entities = {};
        for (const auto *entity : EntitySchemas::EntityAccessor::ALL_ENTITIES)
        {
            EntityPermission entity_perm;
            entity_perm.read = default_access;
            entity_perm.create = default_access;
            entity_perm.update = default_access;
            entity_perm.delete_ = default_access;
            entity_perm.attributes = {};
            entity_perm.relations = {};
            perm.entities[entity] = entity_perm;
        }
        return perm;
    }

    GroupPermission populate_missing(GroupPermission &perm)
    {
        for (const auto *entity_name : EntitySchemas::EntityAccessor::ALL_ENTITIES)
        {
            if (!perm.entities.contains(entity_name))
                perm.entities[entity_name] = EntityPermission{};

            if (std::string_view(entity_name) == EntitySchemas::EntityAccessor::TASKS)
            {
                perm.entities[entity_name] = PermissionCore::populate_missing<EntitySchemas::EntityAccessor::TaskSchema>(perm.entities[entity_name]);
            }
            else if (std::string_view(entity_name) == EntitySchemas::EntityAccessor::USERS)
            {
                perm.entities[entity_name] = PermissionCore::populate_missing<EntitySchemas::EntityAccessor::UserSchema>(perm.entities[entity_name]);
            }
        }
        return perm;
    }

    std::optional<bool> get_merged_access(const std::optional<bool> &a, const std::optional<bool> &b)
    {
        if (a.has_value() && b.has_value())
            return a.value() || b.value();
        if (a.has_value())
            return a.value();
        if (b.has_value())
            return b.value();
        return false;
    }

    AttributePermission merge(AttributePermission a, AttributePermission b)
    {
        return {.read = get_merged_access(a.read, b.read), .update = get_merged_access(a.update, b.update)};
    }

    RelationPermission merge(RelationPermission a, RelationPermission b)
    {
        return {.read = get_merged_access(a.read, b.read), .update = get_merged_access(a.update, b.update)};
    }

    EntityPermission merge(EntityPermission a, EntityPermission b)
    {
        EntityPermission result;
        result.read = get_merged_access(a.read, b.read);
        result.create = get_merged_access(a.create, b.create);
        result.update = get_merged_access(a.update, b.update);
        result.delete_ = get_merged_access(a.delete_, b.delete_);

        result.attributes = a.attributes;
        for (auto &[name, perm] : b.attributes)
        {
            if (result.attributes.contains(name))
                result.attributes[name] = merge(result.attributes[name], perm);
            else
                result.attributes[name] = perm;
        }

        result.relations = a.relations;
        for (auto &[name, perm] : b.relations)
        {
            if (result.relations.contains(name))
                result.relations[name] = merge(result.relations[name], perm);
            else
                result.relations[name] = perm;
        }

        return result;
    }

    GroupPermission merge(GroupPermission a, GroupPermission b)
    {
        GroupPermission result = a;
        for (auto &[entity, perm] : b.entities)
        {
            if (result.entities.contains(entity))
                result.entities[entity] = merge(result.entities[entity], perm);
            else
                result.entities[entity] = perm;
        }
        return result;
    }
}