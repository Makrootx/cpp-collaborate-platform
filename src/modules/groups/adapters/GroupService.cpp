#include "modules/groups/adapters/GroupService.hpp"

long int GroupService::create_group(const GroupCreateDto &dto)
{
    auto space_opt = space_repo_->find_by_id_query(space_id_, {"members"});
    if (!space_opt.has_value())
    {
        spdlog::error("Failed to create group: space with id {} not found", space_id_);
        throw std::runtime_error("Space not found");
    }
    auto space = space_opt.value();
    std::vector<User> members;
    if (dto.member_ids.has_value())
    {
        for (const auto &member_id : dto.member_ids.value())
        {
            if (!space.user_has_access(member_id))
            {
                spdlog::error("Failed to create group: member user with id {} does not have access to space with id {}", member_id, space_id_);
                throw std::runtime_error("Member user does not have access to space");
            }
            auto member_user_opt = space.get_user_by_id(member_id);
            members.push_back(User(member_user_opt.value()));
        }
    }

    GroupPermission permissions = dto.permissions.has_value() ? dto.permissions.value() : GroupPermission{.entities = {}};

    Group group(dto.name, members, permissions);

    return repo_->create(group);
}

std::optional<GroupResultDto> GroupService::get_group_by_id_query(long int id, const std::vector<std::string> &relations)
{
    auto group_opt = repo_->find_by_id_query(id, relations);
    if (!group_opt.has_value())
    {
        return std::nullopt;
    }
    auto group = group_opt.value();
    return GroupResultDto::from_domain(group);
}

std::vector<GroupResultDto> GroupService::get_all_groups_query(const std::vector<std::string> &relations)
{
    auto groups = repo_->get_all_query(relations);
    std::vector<GroupResultDto> group_results;
    for (const auto &group : groups)
    {
        group_results.push_back(GroupResultDto::from_domain(group));
    }
    return group_results;
}

void GroupService::update_group(const GroupUpdateDto &dto)
{
    auto group_opt = repo_->find_by_id(dto.id);
    if (!group_opt.has_value())
    {
        spdlog::error("Failed to update group: group with id {} not found", dto.id);
        throw std::runtime_error("Group not found");
    }
    auto group = group_opt.value();

    if (dto.name.has_value())
    {
        group.set_name(dto.name.value());
    }
    if (dto.permissions.has_value())
    {
        group.set_permissions(dto.permissions.value());
    }

    repo_->update(group);
}

void GroupService::delete_group_by_id(long int id)
{
    repo_->delete_by_id(id);
}

void GroupService::add_group_members(long int group_id, const std::vector<long int> &member_ids)
{
    auto space_opt = space_repo_->find_by_id_query(space_id_, {"members"});
    if (!space_opt.has_value())
    {
        spdlog::error("Failed to remove group members: space with id {} not found", space_id_);
        throw std::runtime_error("Space not found");
    }
    auto space = space_opt.value();

    auto group_opt = repo_->find_by_id(group_id);
    if (!group_opt.has_value())
    {
        spdlog::error("Failed to add group members: group with id {} not found", group_id);
        throw std::runtime_error("Group not found");
    }
    auto group = group_opt.value();

    std::vector<User> members_to_add;
    for (const auto &member_id : member_ids)
    {
        if (!space.user_has_access(member_id))
        {
            spdlog::error("Failed to add group members: member user with id {} does not have access to space with id {}", member_id, space_id_);
            throw std::runtime_error("Member user does not have access to space");
        }
        auto member_user_opt = space.get_user_by_id(member_id);
        members_to_add.push_back(User(member_user_opt.value()));
    }

    group.add_members(members_to_add);
    repo_->update(group);
}

void GroupService::remove_group_members(long int group_id, const std::vector<long int> &member_ids)
{
    auto space_opt = space_repo_->find_by_id_query(space_id_, {"members"});
    if (!space_opt.has_value())
    {
        spdlog::error("Failed to remove group members: space with id {} not found", space_id_);
        throw std::runtime_error("Space not found");
    }
    auto space = space_opt.value();

    auto group_opt = repo_->find_by_id(group_id);
    if (!group_opt.has_value())
    {
        spdlog::error("Failed to remove group members: group with id {} not found", group_id);
        throw std::runtime_error("Group not found");
    }
    auto group = group_opt.value();

    std::vector<User> members_to_remove;
    for (const auto &member_id : member_ids)
    {
        if (!space.user_has_access(member_id))
        {
            spdlog::error("Failed to remove group members: member user with id {} does not have access to space with id {}", member_id, space_id_);
            throw std::runtime_error("Member user does not have access to space");
        }
        auto member_user_opt = space.get_user_by_id(member_id);
        members_to_remove.push_back(User(member_user_opt.value()));
    }

    group.remove_members(members_to_remove);
    repo_->update(group);
}

void GroupService::set_group_entity_permissions(long int group_id, const std::string &entity, EntityPermission permissions)
{
    auto group_opt = repo_->find_by_id(group_id);
    if (!group_opt.has_value())
    {
        spdlog::error("Failed to set group entity permissions: group with id {} not found", group_id);
        throw std::runtime_error("Group not found");
    }
    auto group = group_opt.value();

    group.set_permissions_for_entity(entity, permissions);
    repo_->update(group);
}