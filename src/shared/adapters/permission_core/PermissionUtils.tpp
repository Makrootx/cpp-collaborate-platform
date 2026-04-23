#include "shared/adapters/permission_core/PermissionUtils.hpp"

namespace PermissionCore
{
    template <typename Schema>
        requires EntitySchemas::HasEntitySchema<Schema>
    bool has_attribute(std::string_view attr)
    {
        for (auto a : Schema::Attributes::ALL)
        {
            if (a == attr)
                return true;
        }
        return false;
    }

    template <typename EntitySchema>
        requires EntitySchemas::HasEntitySchema<EntitySchema>
    EntityPermission populate_missing(EntityPermission &perm)
    {
        auto default_perm = false;
        auto alias_read = perm.read.has_value() ? perm.read.value() : default_perm;
        auto alias_update = perm.update.has_value() ? perm.update.value() : default_perm;
        auto alias_create = perm.create.has_value() ? perm.create.value() : default_perm;
        auto alias_delete = perm.delete_.has_value() ? perm.delete_.value() : default_perm;
        for (const auto *attr : EntitySchema::Attributes::ALL)
        {
            if (!perm.attributes.contains(attr))
                perm.attributes[attr] = {};
            if (!perm.attributes[attr].read.has_value())
                perm.attributes[attr].read = alias_read;
            if (!perm.attributes[attr].update.has_value())
                perm.attributes[attr].update = alias_update;
        }
        for (const auto *rel : EntitySchema::Relations::ALL)
        {
            if (!perm.relations.contains(rel))
                perm.relations[rel] = {};
            if (!perm.relations[rel].read.has_value())
                perm.relations[rel].read = alias_read;
            if (!perm.relations[rel].update.has_value())
                perm.relations[rel].update = alias_update;
        }
        perm.read = alias_read;
        perm.update = alias_update;
        perm.create = alias_create;
        perm.delete_ = alias_delete;
        return perm;
    }
}