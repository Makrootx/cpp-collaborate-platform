#include "shared/domain/SecureWrapper.hpp"

template <typename Fn>
bool SecurePolicy::before(Fn, PermissionFactor perm_factor, GroupPermission context, std::string entity)
{
    spdlog::debug("Permission check of {} for attribute: {}", entity, perm_factor.access_object);
    const auto &schema_map = EntitySchemas::EntityAccessor::get_schema_map();
    auto it = schema_map.find(entity);
    if (it == schema_map.end())
    {
        spdlog::error("Entity {} not found in schema map", entity);
        return false;
    }
    const auto &schema = it->second;
    AttributePermission attr_perm;

    auto entity_it = context.entities.find(entity);
    if (entity_it == context.entities.end())
    {
        spdlog::error("Entity {} not found in security context", entity);
        return false;
    }

    if (schema.has_attr(perm_factor.access_object))
    {
        auto attr_it = entity_it->second.attributes.find(perm_factor.access_object);
        if (attr_it == entity_it->second.attributes.end())
        {
            spdlog::error("Attribute {} not found in security context for entity {}", perm_factor.access_object, entity);
            return false;
        }
        attr_perm = attr_it->second;
    }
    else if (schema.has_rel(perm_factor.access_object))
    {
        auto rel_it = entity_it->second.relations.find(perm_factor.access_object);
        if (rel_it == entity_it->second.relations.end())
        {
            spdlog::error("Relation {} not found in security context for entity {}", perm_factor.access_object, entity);
            return false;
        }
        auto rel_perm = rel_it->second;
        attr_perm = AttributePermission{.read = rel_perm.read, .update = rel_perm.update};
    }
    else
    {
        spdlog::error("Access object {} not found in entity {}", perm_factor.access_object, entity);
        return false;
    }

    auto access_granted = perm_factor.check_access(attr_perm);
    spdlog::debug("Access {} for attribute {} of entity {}", access_granted ? "granted" : "denied", perm_factor.access_object, entity);

    return access_granted;
}

template <typename Fn>
void SecurePolicy::after(Fn)
{
}

template <typename UseType, typename Domain, typename Policy>
template <typename ReturnType, typename... Args>
std::optional<std::remove_cvref_t<ReturnType>> SecureWrapper<UseType, Domain, Policy>::secure_get(ReturnType (UseType::*method)(Args...) const, PermissionFactor perm_factor, Args... args) const
{
    if (!Policy::before(method, perm_factor, security_context_, get_domain_permission_name()))
    {
        return std::nullopt;
    }
    auto result = Base::perform(method, std::forward<Args>(args)...);
    return std::optional<std::remove_cvref_t<ReturnType>>(result);
}

template <typename UseType, typename Domain, typename Policy>
template <typename ReturnType, typename... Args>
bool SecureWrapper<UseType, Domain, Policy>::secure_set(ReturnType (UseType::*method)(Args...), PermissionFactor perm_factor, Args... args)
{
    if (!Policy::before(method, perm_factor, security_context_, get_domain_permission_name()))
    {
        return false;
    }
    Base::perform(method, std::forward<Args>(args)...);
    return true;
}