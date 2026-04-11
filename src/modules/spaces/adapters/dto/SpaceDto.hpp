#pragma once
#include "shared/helpers/json/JsonDto.hpp"
#include "modules/spaces/domain/Space.hpp"
#include "modules/users/adapters/dto/UserDto.hpp"

class SpaceCreateDto : public JsonDto<SpaceCreateDto>
{
public:
    std::string title;
    std::optional<std::string> description;
    long int owner_id;

    static const std::vector<FieldDescriptor> &fields()
    {
        static const std::vector<FieldDescriptor> f = {
            field("title", &SpaceCreateDto::title),
            optional_field("description", &SpaceCreateDto::description),
            field("owner_id", &SpaceCreateDto::owner_id),
        };
        return f;
    };
};

class SpaceUpdateDto : public JsonDto<SpaceUpdateDto>
{
public:
    long int id;
    std::optional<std::string> title;
    std::optional<std::string> description;
    std::optional<long int> owner_id;

    static const std::vector<FieldDescriptor> &fields()
    {
        static const std::vector<FieldDescriptor> f = {
            field("id", &SpaceUpdateDto::id),
            optional_field("title", &SpaceUpdateDto::title),
            optional_field("description", &SpaceUpdateDto::description),
            optional_field("owner_id", &SpaceUpdateDto::owner_id),
        };
        return f;
    };
};

class SpaceResultDto : public JsonDto<SpaceResultDto>
{
public:
    long int id;
    std::string title;
    std::optional<std::string> description;
    std::optional<std::vector<UserResultDto>> members;
    long int owner_id;

    static const std::vector<FieldDescriptor> &fields()
    {
        static const std::vector<FieldDescriptor> f = {
            field("id", &SpaceResultDto::id),
            field("title", &SpaceResultDto::title),
            optional_field("description", &SpaceResultDto::description),
            optional_nested_field("members", &SpaceResultDto::members),
            field("owner_id", &SpaceResultDto::owner_id),
        };
        return f;
    };

    static std::optional<SpaceResultDto> from_domain(const Space &space)
    {
        SpaceResultDto dto;
        dto.id = space.get_id();
        dto.title = space.get_title();
        dto.description = space.get_description();
        dto.owner_id = space.get_owner().get_id();

        auto members = space.get_members();
        if (members)
        {
            std::vector<UserResultDto> member_dtos;
            for (const auto &member : members.value())
            {
                member_dtos.push_back(UserResultDto::from_domain(member));
            }
            dto.members = member_dtos;
        }

        return dto;
    };
};