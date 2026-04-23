#pragma once

#include <optional>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "shared/adapters/automation_core/AutomationRuntimeStructs.hpp"

/// @brief Singleton service that registers, stores, and executes automation rules per space.
class AutomationService
{
private:
    AutomationService() = default;

    std::unordered_map<long, std::unordered_map<std::string, std::vector<Automation>>> spaces_;

    mutable std::shared_mutex mutex_;

    void fire_matching(
        const std::vector<Automation> &automations,
        const std::unordered_map<std::string, FieldFullAccessor> &field_accessors);

    bool evaluate_condition(
        const ConditionGroup &group,
        const std::unordered_map<std::string, FieldFullAccessor> &field_accessors);

    bool compare(
        const std::optional<LiteralValue> &actual,
        ConditionOp op,
        const std::optional<LiteralValue> &expected);

    void execute_action(
        const AutomationAction &action,
        const std::unordered_map<std::string, FieldFullAccessor> &accessors);

    std::optional<LiteralValue> resolve_value(
        const AutomationValue &val,
        const std::unordered_map<std::string, FieldFullAccessor> &accessors);

    static std::string make_key(
        const std::string &entity,
        AutomationEvent event,
        AutomationTiming timing,
        std::optional<std::string> attribute);

public:
    static AutomationService &instance()
    {
        static AutomationService instance;
        return instance;
    }

    /// @brief Indexes and stores an automation config for the given space, replacing any prior registration.
    void register_space(long int space_id, AutomationConfig config);
    /// @brief Removes all automation rules registered for the given space.
    void unregister_space(long int space_id);
    /// @brief Returns true when the given space has at least one registered automation.
    bool has_space(long space_id) const;
    /// @brief Evaluates and fires all matching automation rules for the domain actions recorded in domain_automation.
    void execute_automations(long int space_id, const DomainAtomAutomation &domain_automation);

    /// @brief Captures the current LiteralValue for every automatable field of target as a before-snapshot.
    static std::unordered_map<std::string, std::optional<LiteralValue>> snapshot(IAutomatable &target);
};