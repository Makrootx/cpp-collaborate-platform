#pragma once

#include <optional>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "shared/adapters/automation_core/AutomationRuntimeStructs.hpp"

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

    void register_space(long int space_id, AutomationConfig config);
    void unregister_space(long int space_id);
    bool has_space(long space_id) const;
    void execute_automations(long int space_id, const DomainAtomAutomation &domain_automation);

    static std::unordered_map<std::string, std::optional<LiteralValue>> snapshot(IAutomatable &target);
};