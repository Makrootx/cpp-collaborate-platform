#include "modules/spaces/adapters/SpaceService.hpp"
#include <spdlog/spdlog.h>

long int SpaceService::create_space(const SpaceCreateDto &dto)
{
    auto user_opt = user_repo_->find_by_id(dto.owner_id);
    if (!user_opt.has_value())
    {
        spdlog::error("Failed to create space: owner user with id {} not found", dto.owner_id);
        throw std::runtime_error("Owner user not found");
    }
    Space space(dto.title, user_opt.value(), dto.description);
    return repo_->create(space);
}

std::optional<SpaceResultDto> SpaceService::get_space_by_id(long int id)
{
    auto space_opt = repo_->find_by_id(id);
    if (space_opt.has_value())
    {
        const auto &space = space_opt.value();
        return SpaceResultDto::from_domain(space);
    }
    return std::nullopt;
}

std::optional<SpaceResultDto> SpaceService::get_space_by_id_query(long int id, const std::vector<std::string> &columns)
{
    auto space_opt = repo_->find_by_id_query(id, columns);
    if (space_opt.has_value())
    {
        const auto &space = space_opt.value();
        return SpaceResultDto::from_domain(space);
    }
    return std::nullopt;
}

std::vector<SpaceResultDto> SpaceService::get_user_spaces(long int user_id)
{
    std::vector<SpaceResultDto> result;
    auto spaces = repo_->find_user_spaces(user_id);
    for (const auto &space : spaces)
    {
        result.push_back(SpaceResultDto::from_domain(space).value());
    }
    return result;
}

void SpaceService::update_space(const SpaceUpdateDto &dto)
{
    auto space_opt = repo_->find_by_id(dto.id);
    if (!space_opt.has_value())
    {
        spdlog::error("Failed to update space: space with id {} not found", dto.id);
        throw std::runtime_error("Space not found");
    }
    auto space = space_opt.value();
    if (dto.title.has_value())
    {
        space.set_title(dto.title.value());
    }
    if (dto.description.has_value())
    {
        space.set_description(dto.description);
    }
    if (dto.owner_id.has_value())
    {
        auto new_owner_opt = user_repo_->find_by_id(dto.owner_id.value());
        if (!new_owner_opt.has_value())
        {
            spdlog::error("Failed to update space: new owner user with id {} not found", dto.owner_id.value());
            throw std::runtime_error("New owner user not found");
        }
        space.set_owner(new_owner_opt.value());
    }
    repo_->update(space);
}

void SpaceService::delete_space_by_id(long int id)
{
    repo_->delete_by_id(id);
}

void SpaceService::add_space_members(long int space_id, std::vector<long int> user_ids)
{
    auto space_opt = repo_->find_by_id(space_id);
    if (!space_opt.has_value())
    {
        spdlog::error("Failed to add members to space: space with id {} not found", space_id);
        throw std::runtime_error("Space not found");
    }
    auto space = space_opt.value();
    for (const auto &user_id : user_ids)
    {
        auto user_opt = user_repo_->find_by_id(user_id);
        if (!user_opt.has_value())
        {
            spdlog::error("Failed to add member to space: user with id {} not found", user_id);
            throw std::runtime_error("User not found");
        }
        space.add_member(user_opt.value());
    }
    repo_->update(space);
}

void SpaceService::remove_space_members(long int space_id, std::vector<long int> user_ids)
{
    auto space_opt = repo_->find_by_id(space_id);
    if (!space_opt.has_value())
    {
        spdlog::error("Failed to remove members from space: space with id {} not found", space_id);
        throw std::runtime_error("Space not found");
    }
    auto space = space_opt.value();
    for (const auto &user_id : user_ids)
    {
        space.remove_member(user_id);
    }
    repo_->update(space);
}