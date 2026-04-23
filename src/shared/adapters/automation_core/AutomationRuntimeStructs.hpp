#pragma once

#include <algorithm>
#include <functional>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "shared/adapters/automation_core/AutomationStructs.hpp"

/// @brief Holds get, set, and optional increment callbacks for a single automatable domain field.
struct FieldAccessor
{
    std::function<std::optional<LiteralValue>()> get;
    std::function<void(std::optional<LiteralValue>)> set;
    std::function<void(int)> increment;
};

/// @brief Extends FieldAccessor with a get_before callback that returns the field value captured before the domain action.
struct FieldFullAccessor
{
    std::function<std::optional<LiteralValue>()> get;
    std::function<std::optional<LiteralValue>()> get_before;
    std::function<void(std::optional<LiteralValue>)> set;
    std::function<void(int)> increment;

    static FieldFullAccessor get_full_accessor(const FieldAccessor &accessor, std::optional<LiteralValue> before_value)
    {
        return FieldFullAccessor{
            .get = accessor.get,
            .get_before = [before_value]()
            { return before_value; },
            .set = accessor.set,
            .increment = accessor.increment};
    }
};

/// @brief Interface for domain objects that participate in the automation system, exposing field accessors and before-state.
class IAutomatable
{
public:
    virtual std::string automation_entity() const = 0;
    virtual std::unordered_map<std::string, FieldAccessor> field_accessors() = 0;
    virtual std::unordered_map<std::string, std::optional<LiteralValue>> before_domain() const = 0;

    /// @brief Merges field_accessors() with before_domain() snapshots to produce FieldFullAccessor instances.
    std::unordered_map<std::string, FieldFullAccessor> full_field_accessors()
    {
        auto accessors = field_accessors();
        auto before_values = before_domain();

        std::unordered_map<std::string, FieldFullAccessor> full_accessors;
        for (const auto &[field, accessor] : accessors)
        {
            std::optional<LiteralValue> before_value = std::nullopt;
            auto it = before_values.find(field);
            if (it != before_values.end())
            {
                before_value = it->second;
            }
            full_accessors[field] = FieldFullAccessor::get_full_accessor(accessor, before_value);
        }
        return full_accessors;
    }

    virtual ~IAutomatable() = default;
};

/// @brief Records a single automatable event (READ/UPDATE) on a named domain attribute.
struct DomainAction
{
    AutomationEvent event;
    std::string attribute;
};

/// @brief Bundles the entity name, recorded actions, and full field accessors for a single automation dispatch.
struct DomainAtomAutomation
{
    std::string entity;
    std::vector<DomainAction> actions;
    std::unordered_map<std::string, FieldFullAccessor> field_full_accessors;

    bool is_updated() const
    {
        return !actions.empty() && std::any_of(actions.begin(), actions.end(), [](const auto &a)
                                               { return a.event == AutomationEvent::UPDATE; });
    }

    bool is_read() const
    {
        return !actions.empty() && std::any_of(actions.begin(), actions.end(), [](const auto &a)
                                               { return a.event == AutomationEvent::READ; });
    }
};
