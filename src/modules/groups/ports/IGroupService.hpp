#pragma once

#include "modules/groups/domain/Group.hpp"
#include "modules/groups/adapters/dto/GroupDto.hpp"

class IGroupService
{
public:
    virtual ~IGroupService() = default;

    virtual long int create_group(const GroupCreateDto &group) = 0;
    virtual std::optional<GroupResultDto> get_group_by_id_query(long int id, const std::vector<std::string> &relations) = 0;
    virtual std::vector<GroupResultDto> get_all_groups_query(const std::vector<std::string> &relations) = 0;
    virtual void update_group(const GroupUpdateDto &group) = 0;
    virtual void delete_group_by_id(long int id) = 0;
    virtual void add_group_members(long int group_id, const std::vector<long int> &member_ids) = 0;
    virtual void remove_group_members(long int group_id, const std::vector<long int> &member_ids) = 0;
    virtual void set_group_entity_permissions(long int group_id, const std::string &entity, EntityPermission permissions) = 0;
};