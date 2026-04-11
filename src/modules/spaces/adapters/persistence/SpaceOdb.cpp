#include "modules/users/adapters/persistence/UserOdb-odb.hxx"
#include "modules/spaces/adapters/persistence/SpaceOdb-odb.hxx"
#include "modules/spaces/adapters/persistence/SpaceOdb.hpp"
#include <algorithm>
#include <optional>
#include <spdlog/spdlog.h>

#include <fmt/ranges.h>

Space SpaceOdb::to_domain(const SpaceOdb &odb)
{
    auto owner_ptr = odb.owner_.load();
    if (!owner_ptr)
    {
        spdlog::error("SpaceOdb::to_domain: owner failed to load");
        throw std::runtime_error("SpaceOdb::to_domain: owner failed to load");
    }

    std::optional<std::vector<User>> members;
    if (odb.is_field_included("members"))
    {
        members = std::vector<User>{};
        for (const auto &m : odb.get_members())
        {
            if (!m.loaded())
            {
                members = std::nullopt;
                break;
            }

            members->push_back(UserOdb::to_domain(*m));
        }
    }

    return Space(
        odb.get_title(),
        UserOdb::to_domain(*owner_ptr),
        odb.get_description(),
        members,
        odb.id_);
}

const std::map<std::string, std::function<std::vector<BaseOdb::Loader>()>> SpaceOdb::get_loaders_map() const
{
    std::map<std::string, std::function<std::vector<Loader>()>> map;
    map["members"] = [this]()
    {
        return BaseOdb::to_loaders(this->members_);
    };
    map["owner"] = [this]()
    {
        return BaseOdb::to_loaders(this->owner_);
    };
    return map;
}

SpaceOdb SpaceOdb::to_odb(const Space &domain)
{
    SpaceOdb odb(
        domain.get_title(),
        domain.get_description(),
        static_cast<unsigned long>(domain.get_id()));

    odb.owner_ = std::make_shared<UserOdb>(UserOdb::to_odb(domain.get_owner()));

    if (domain.get_members().has_value())
        for (const auto &member : *domain.get_members())
            odb.members_.push_back(
                std::make_shared<UserOdb>(UserOdb::to_odb(member)));

    return odb;
}