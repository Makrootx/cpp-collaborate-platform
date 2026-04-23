#pragma once

#include "modules/groups/ports/IGroupService.hpp"
#include "modules/groups/ports/IGroupRepo.hpp"
#include "modules/spaces/ports/ISpaceRepo.hpp"
#include <memory>

class GroupService : public IGroupService
{
    long int space_id_ = -1;
    std::shared_ptr<IGroupRepo> repo_;
    std::shared_ptr<ISpaceRepo> space_repo_;

public:
    explicit GroupService(std::shared_ptr<IGroupRepo> repo_,
                          std::shared_ptr<ISpaceRepo> space_repo_)
        : repo_(std::move(repo_)),
          space_repo_(std::move(space_repo_)) {};

    explicit GroupService(long int space_id, std::shared_ptr<IGroupRepo> repo_,
                          std::shared_ptr<ISpaceRepo> space_repo_) : GroupService(std::move(repo_), std::move(space_repo_))
    {
        space_id_ = space_id;
    }

    long int create_group(const GroupCreateDto &group) override;
    std::optional<GroupResultDto> get_group_by_id_query(long int id, const std::vector<std::string> &relations) override;
    std::vector<GroupResultDto> get_all_groups_query(const std::vector<std::string> &relations) override;
    void update_group(const GroupUpdateDto &group) override;
    void delete_group_by_id(long int id) override;
    void add_group_members(long int group_id, const std::vector<long int> &member_ids) override;
    void remove_group_members(long int group_id, const std::vector<long int> &member_ids) override;
    void set_group_entity_permissions(long int group_id, const std::string &entity, EntityPermission permissions) override;
};