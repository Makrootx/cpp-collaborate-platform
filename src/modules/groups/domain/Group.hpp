#pragma once
#include "shared/domain/BaseDomain.hpp"
#include "modules/users/domain/User.hpp"
#include "shared/adapters/permission_core/PermissionUtils.hpp"
#include <ranges>
#include <unordered_set>

class Group : public BaseDomain
{
protected:
    std::string name;
    std::vector<User> members;
    GroupPermission permissions;

public:
    Group() = default;

    explicit Group(std::string name, std::vector<User> members,
                   std::optional<GroupPermission> permissions, unsigned long id = 0)
        : name(std::move(name)), members(std::move(members)),
          permissions(permissions.has_value() ? std::move(permissions.value()) : std::move(GroupPermission{.entities = {}})),
          BaseDomain(id) {}

    Group(const Group &) = default;
    Group(Group &&) noexcept = default;
    Group &operator=(Group &&) noexcept = default;
    Group &operator=(const Group &) = default;
    ~Group() = default;

    const std::string &get_name() const { return name; };
    const std::vector<User> &get_members() const { return members; };
    const GroupPermission &get_permissions() const { return permissions; };

    void set_name(std::string name) { this->name = std::move(name); }
    void set_permissions(GroupPermission permissions) { this->permissions = std::move(permissions); }
    void set_members(std::vector<User> members) { this->members = std::move(members); }

    bool member_added(const User &user) const
    {
        return std::ranges::any_of(members, [&](const User &m)
                                   { return m.get_id() == user.get_id(); });
    }

    void add_members(const std::vector<User> &members_to_add)
    {
        std::unordered_set<unsigned long> existing;
        existing.reserve(members.size());
        for (const auto &m : members)
            existing.insert(m.get_id());

        for (auto &m : members_to_add)
            if (existing.insert(m.get_id()).second)
                members.emplace_back(std::move(m));
    }

    void remove_members(const std::vector<User> &members_to_remove)
    {
        std::unordered_set<unsigned long> to_remove;
        for (const auto &m : members_to_remove)
            to_remove.insert(m.get_id());

        this->members.erase(
            std::remove_if(this->members.begin(), this->members.end(),
                           [&to_remove](const User &m)
                           {
                               return to_remove.count(m.get_id()) > 0;
                           }),
            this->members.end());
    }

    void set_permissions_for_entity(const std::string &entity, EntityPermission &perm)
    {
        const auto &schema_map = EntitySchemas::EntityAccessor::get_schema_map();
        auto it = schema_map.find(entity);
        if (it == schema_map.end())
        {
            throw std::invalid_argument("Invalid entity for permissions: " + entity);
        }

        const auto &schema = it->second;

        std::erase_if(perm.attributes, [&](const auto &item)
                      { return !schema.has_attr(item.first); });

        std::erase_if(perm.relations, [&](const auto &item)
                      { return !schema.has_rel(item.first); });

        permissions.entities[entity] = std::move(perm);
    }

protected:
    bool entity_can_setted_permission(const std::string &entity) const
    {
        const auto &map = EntitySchemas::EntityAccessor::get_schema_map();

        auto it = map.find(entity);
        if (it == map.end())
            return false;
        return true;
    }

    bool attribute_can_setted_permission(const std::string &entity, const std::string &attr) const
    {
        const auto &map = EntitySchemas::EntityAccessor::get_schema_map();

        auto it = map.find(entity);
        if (it == map.end())
            return false;
        return it->second.has_attr(attr);
    }

    bool relation_can_setted_permission(const std::string &entity, const std::string &rel) const
    {
        const auto &map = EntitySchemas::EntityAccessor::get_schema_map();

        auto it = map.find(entity);
        if (it == map.end())
            return false;
        return it->second.has_rel(rel);
    }
};
