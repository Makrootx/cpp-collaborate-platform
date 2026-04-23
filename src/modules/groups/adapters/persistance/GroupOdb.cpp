#include "modules/groups/adapters/persistance/GroupOdb-odb.hxx"
#include "modules/users/adapters/persistence/UserOdb-odb.hxx"
#include "modules/groups/adapters/persistance/GroupOdb.hpp"

Group GroupOdb::to_domain(const GroupOdb &odb)
{
    std::vector<User> members;
    odb.populate({"members"});
    if (odb.is_field_included("members"))
    {
        for (const auto &m : odb.members_value())
        {
            if (!m.loaded())
            {
                members = {};
                break;
            }

            members.push_back(UserOdb::to_domain(*m));
        }
    }

    return Group(
        odb.name_value(),
        members,
        odb.permissions_value(),
        odb.id_);
}

const std::map<std::string, std::function<std::vector<BaseOdb::Loader>()>> GroupOdb::get_loaders_map() const
{
    std::map<std::string, std::function<std::vector<Loader>()>> map;
    map["members"] = [this]()
    {
        return BaseOdb::to_loaders(this->members_);
    };
    return map;
}

GroupOdb GroupOdb::to_odb(const Group &domain)
{
    std::vector<odb::lazy_shared_ptr<UserOdb>> member_ptrs;
    for (const auto &m : domain.get_members())
    {
        member_ptrs.push_back(std::make_shared<UserOdb>(UserOdb::to_odb(m)));
    }

    return GroupOdb(
        domain.get_name(),
        member_ptrs,
        domain.get_permissions(),
        domain.get_id());
}
