#pragma once

#include <map>
#include <optional>
#include <string>
#include <vector>

#include "shared/helpers/json/JsonDto.hpp"

struct AttributePermission : JsonDto<AttributePermission>
{
    std::optional<bool> read = std::nullopt;
    std::optional<bool> update = std::nullopt;

    static const std::vector<FieldDescriptor> &fields()
    {
        static const std::vector<FieldDescriptor> f = {
            optional_field("READ", &AttributePermission::read),
            optional_field("UPDATE", &AttributePermission::update)};
        return f;
    };
};

struct PermissionFactor
{
    std::optional<bool> read = std::nullopt;
    std::optional<bool> update = std::nullopt;
    std::string access_object;

    bool check_access(const AttributePermission &attr_perm) const;

    static PermissionFactor get_read_factor(std::string access_object);

    static PermissionFactor get_update_factor(std::string access_object);
};

struct RelationPermission : JsonDto<RelationPermission>
{
    std::optional<bool> read = std::nullopt;
    std::optional<bool> update = std::nullopt;

    static const std::vector<FieldDescriptor> &fields()
    {
        static const std::vector<FieldDescriptor> f = {
            optional_field("READ", &RelationPermission::read),
            optional_field("UPDATE", &RelationPermission::update),
        };
        return f;
    };
};

struct EntityPermission : JsonDto<EntityPermission>
{
    std::optional<bool> read = false;
    std::optional<bool> update = false;
    std::optional<bool> delete_ = false;
    std::optional<bool> create = false;
    std::map<std::string, AttributePermission> attributes = {};
    std::map<std::string, RelationPermission> relations = {};

    static const std::vector<FieldDescriptor> &fields();

    static std::optional<EntityPermission> from_json(const crow::json::rvalue &j);

    crow::json::wvalue to_json() const;
};

struct GroupPermission : JsonDto<GroupPermission>
{
    std::map<std::string, EntityPermission> entities = {};

    static const std::vector<FieldDescriptor> &fields();

    static std::optional<GroupPermission> from_json(const crow::json::rvalue &j);

    crow::json::wvalue to_json() const;
};
