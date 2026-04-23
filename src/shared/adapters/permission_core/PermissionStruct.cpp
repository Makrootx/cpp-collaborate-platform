#include "shared/adapters/permission_core/PermissionStruct.hpp"

bool PermissionFactor::check_access(const AttributePermission &attr_perm) const
{
    if (read.has_value() && attr_perm.read.has_value() && read.value() && !attr_perm.read.value())
        return false;
    if (update.has_value() && attr_perm.update.has_value() && update.value() && !attr_perm.update.value())
        return false;
    return true;
}

PermissionFactor PermissionFactor::get_read_factor(std::string access_object)
{
    return PermissionFactor{.read = true, .update = std::nullopt, .access_object = std::move(access_object)};
}

PermissionFactor PermissionFactor::get_update_factor(std::string access_object)
{
    return PermissionFactor{.read = std::nullopt, .update = true, .access_object = std::move(access_object)};
}

const std::vector<EntityPermission::FieldDescriptor> &EntityPermission::fields()
{
    static const std::vector<FieldDescriptor> f = {
        optional_field("READ", &EntityPermission::read),
        optional_field("CREATE", &EntityPermission::create),
        optional_field("UPDATE", &EntityPermission::update),
        optional_field("DELETE", &EntityPermission::delete_)};
    return f;
}

std::optional<EntityPermission> EntityPermission::from_json(const crow::json::rvalue &j)
{
    auto base = JsonDto<EntityPermission>::from_json(j);
    if (!base)
        return std::nullopt;

    if (j.has("attributes"))
        base->attributes = JsonTypeHelper<std::map<std::string, AttributePermission>>::read(j["attributes"]);
    if (j.has("relations"))
        base->relations = JsonTypeHelper<std::map<std::string, RelationPermission>>::read(j["relations"]);

    return base;
}

crow::json::wvalue EntityPermission::to_json() const
{
    auto out = JsonDto<EntityPermission>::to_json();
    if (!attributes.empty())
        out["attributes"] = JsonTypeHelper<std::map<std::string, AttributePermission>>::write(attributes);
    if (!relations.empty())
        out["relations"] = JsonTypeHelper<std::map<std::string, RelationPermission>>::write(relations);
    return out;
}

const std::vector<GroupPermission::FieldDescriptor> &GroupPermission::fields()
{
    static const std::vector<FieldDescriptor> f = {};
    return f;
}

std::optional<GroupPermission> GroupPermission::from_json(const crow::json::rvalue &j)
{
    auto base = JsonDto<GroupPermission>::from_json(j);
    if (!base)
        return std::nullopt;

    if (j.has("entities"))
        base->entities = JsonTypeHelper<std::map<std::string, EntityPermission>>::read(j["entities"]);

    return base;
}

crow::json::wvalue GroupPermission::to_json() const
{
    auto out = JsonDto<GroupPermission>::to_json();
    if (!entities.empty())
        out["entities"] = JsonTypeHelper<std::map<std::string, EntityPermission>>::write(entities);
    return out;
}