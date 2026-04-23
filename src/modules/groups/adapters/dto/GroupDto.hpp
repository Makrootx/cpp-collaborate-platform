#pragma once

#include "shared/helpers/json/JsonDto.hpp"
#include "shared/adapters/permission_core/PermissionUtils.hpp"

#include "modules/groups/domain/Group.hpp"
#include "modules/users/adapters/dto/UserDto.hpp"

class GroupCreateDto : public JsonDto<GroupCreateDto>
{
public:
    std::string name;
    std::optional<std::vector<long int>> member_ids;
    std::optional<GroupPermission> permissions;

    static const std::vector<FieldDescriptor> &fields()
    {
        static const std::vector<FieldDescriptor> f = {
            field("name", &GroupCreateDto::name),
            optional_field("member_ids", &GroupCreateDto::member_ids),
            optional_nested_field("permissions", &GroupCreateDto::permissions),
        };
        return f;
    };
};

class GroupUpdateDto : public JsonDto<GroupUpdateDto>
{
public:
    long int id;
    std::optional<std::string> name;
    std::optional<std::vector<long int>> member_ids;
    std::optional<GroupPermission> permissions;

    static const std::vector<FieldDescriptor> &fields()
    {
        static const std::vector<FieldDescriptor> f = {
            field("id", &GroupUpdateDto::id),
            optional_field("name", &GroupUpdateDto::name),
            optional_field("member_ids", &GroupUpdateDto::member_ids),
            optional_nested_field("permissions", &GroupUpdateDto::permissions),
        };
        return f;
    };
};

class GroupResultDto : public JsonDto<GroupResultDto>
{
public:
    long int id;
    std::string name;
    std::optional<std::vector<UserResultDto>> members;
    GroupPermission permissions;

    static const std::vector<FieldDescriptor> &fields()
    {
        static const std::vector<FieldDescriptor> f = {
            field("id", &GroupResultDto::id),
            field("name", &GroupResultDto::name),
            optional_nested_field("members", &GroupResultDto::members),
            nested_field("permissions", &GroupResultDto::permissions),
        };
        return f;
    };

    static GroupResultDto from_domain(const Group &group)
    {
        std::vector<UserResultDto> member_dtos;
        for (const auto &m : group.get_members())
        {
            member_dtos.push_back(UserResultDto::from_domain(m));
        }

        return GroupResultDto{
            .id = group.get_id(),
            .name = group.get_name(),
            .members = member_dtos.empty() ? std::nullopt : std::optional<std::vector<UserResultDto>>(member_dtos),
            .permissions = group.get_permissions()};
    };
};

class GroupMembersDto : public JsonDto<GroupMembersDto>
{
public:
    std::vector<long int> member_ids;

    static const std::vector<FieldDescriptor> &fields()
    {
        static const std::vector<FieldDescriptor> f = {
            field("member_ids", &GroupMembersDto::member_ids),
        };
        return f;
    };
};

class GroupSetPermissionsForEntityDto : public JsonDto<GroupSetPermissionsForEntityDto>
{
public:
    long int group_id;
    std::string entity;
    EntityPermission permissions;

    static const std::vector<FieldDescriptor> &fields()
    {
        static const std::vector<FieldDescriptor> f = {
            field("group_id", &GroupSetPermissionsForEntityDto::group_id),
            field("entity", &GroupSetPermissionsForEntityDto::entity),
            nested_field("permissions", &GroupSetPermissionsForEntityDto::permissions),
        };
        return f;
    };
};
