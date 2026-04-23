#pragma once

#include "modules/groups/domain/Group.hpp"
#include "shared/ports/IOdbMappable.hpp"
#include "shared/ports/IOdbMappableQuery.hpp"
#include "shared/adapters/persistence/odb/BaseOdb.hpp"
#include "modules/users/adapters/persistence/UserOdb.hpp"

#include "shared/adapters/permission_core/PermissionUtils.hpp"

#include <odb/core.hxx>
#include <odb/lazy-ptr.hxx>
#include <odb/nullable.hxx>

#include <memory>

#pragma db object table("groups") pointer(std::shared_ptr)
class GroupOdb : public BaseOdb, public IOdbMappable<Group, GroupOdb>, public IOdbMappableQuery<Group, GroupOdb>
{
public:
    GroupOdb() = default;

    explicit GroupOdb(std::string name,
                      std::vector<odb::lazy_shared_ptr<UserOdb>> members,
                      std::optional<GroupPermission> permissions, unsigned long id = 0)
        : name_(std::move(name)), members_(std::move(members)),
          permissions_(permissions.has_value() ? permissions.value().to_json().dump() : GroupPermission{.entities = {}}.to_json().dump()),
          BaseOdb(id) {}

    GroupOdb(const GroupOdb &) = default;
    GroupOdb(GroupOdb &&) noexcept = default;
    GroupOdb &operator=(GroupOdb &&) noexcept = default;
    GroupOdb &operator=(const GroupOdb &) = default;
    ~GroupOdb() = default;

    const std::string &name_value() const { return name_; }
    const std::vector<odb::lazy_shared_ptr<UserOdb>> &members_value() const { return members_; }
    std::optional<GroupPermission> permissions_value() const
    {
        if (permissions_.empty())
            return std::nullopt;
        auto json = crow::json::load(permissions_);
        if (!json)
            return std::nullopt;
        try
        {
            return GroupPermission::from_json(json);
        }
        catch (const std::exception &e)
        {
            return std::nullopt;
        }
    }

    static Group to_domain(const GroupOdb &odb);
    static GroupOdb to_odb(const Group &domain);

protected:
    const std::map<std::string, std::function<std::vector<BaseOdb::Loader>()>> get_loaders_map() const override;

private:
    friend class odb::access;

#pragma db column("name") type("VARCHAR(255)")
    std::string name_;

#pragma db value_not_null unordered \
    table("group_members")          \
        id_column("group_id")       \
            value_column("user_id")
    std::vector<odb::lazy_shared_ptr<UserOdb>> members_;

#pragma db type("TEXT")
    std::string permissions_;
};
