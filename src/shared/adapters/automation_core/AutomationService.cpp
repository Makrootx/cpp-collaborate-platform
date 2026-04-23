#include "shared/adapters/automation_core/AutomationService.hpp"

#include <chrono>
#include <format>

#include <spdlog/spdlog.h>

void AutomationService::register_space(long int space_id, AutomationConfig config)
{
    std::unique_lock lock(mutex_);
    auto &index = spaces_[space_id];

    index.clear();
    for (const auto &automation : config.automations)
    {
        auto key = make_key(
            automation.trigger.entity,
            automation.trigger.event,
            automation.trigger.timing,
            automation.trigger.attribute);
        index[key].push_back(automation);
    }
}

void AutomationService::unregister_space(long int space_id)
{
    std::unique_lock lock(mutex_);
    spaces_.erase(space_id);
}

bool AutomationService::has_space(long space_id) const
{
    std::shared_lock lock(mutex_);
    return spaces_.contains(space_id);
}

void AutomationService::execute_automations(long int space_id, const DomainAtomAutomation &domain_automation)
{
    spdlog::debug("Executing automations for space {}, entity {}", space_id, domain_automation.entity);
    spdlog::debug("Domain actions: {}", domain_automation.actions.size());
    std::shared_lock lock(mutex_);
    auto space_it = spaces_.find(space_id);
    if (space_it == spaces_.end())
    {
        spdlog::warn("AutomationService: space {} not registered", space_id);
        return;
    }

    auto field_accessors = domain_automation.field_full_accessors;

    for (const auto &action : domain_automation.actions)
    {
        auto key = make_key(domain_automation.entity, action.event, AutomationTiming::BEFORE, action.attribute);
        auto automations_it = space_it->second.find(key);
        if (automations_it == space_it->second.end())
            continue;
        spdlog::debug("Firing automations for event {}, attribute {}", AutomationParseUtils::to_string(action.event), action.attribute);
        spdlog::debug("Automations to fire: {}", automations_it->second[0].to_json().dump());
        fire_matching(automations_it->second, field_accessors);
    }
    if (domain_automation.is_updated())
    {
        auto key = make_key(domain_automation.entity, AutomationEvent::UPDATE, AutomationTiming::BEFORE, std::nullopt);
        auto automations_it = space_it->second.find(key);
        if (automations_it != space_it->second.end())
            fire_matching(automations_it->second, field_accessors);
    }
    if (domain_automation.is_read())
    {
        auto key = make_key(domain_automation.entity, AutomationEvent::READ, AutomationTiming::BEFORE, std::nullopt);
        auto automations_it = space_it->second.find(key);
        if (automations_it != space_it->second.end())
            fire_matching(automations_it->second, field_accessors);
    }

    for (const auto &action : domain_automation.actions)
    {
        auto key = make_key(domain_automation.entity, action.event, AutomationTiming::AFTER, action.attribute);
        auto automations_it = space_it->second.find(key);
        if (automations_it == space_it->second.end())
            continue;
        spdlog::debug("Firing automations for event {}, attribute {}", AutomationParseUtils::to_string(action.event), action.attribute);
        spdlog::debug("Automations to fire: {}", automations_it->second[0].to_json().dump());
        fire_matching(automations_it->second, field_accessors);
    }
    if (domain_automation.is_updated())
    {
        auto key = make_key(domain_automation.entity, AutomationEvent::UPDATE, AutomationTiming::AFTER, std::nullopt);
        auto automations_it = space_it->second.find(key);
        if (automations_it != space_it->second.end())
            fire_matching(automations_it->second, field_accessors);
    }
    if (domain_automation.is_read())
    {
        auto key = make_key(domain_automation.entity, AutomationEvent::READ, AutomationTiming::AFTER, std::nullopt);
        auto automations_it = space_it->second.find(key);
        if (automations_it != space_it->second.end())
            fire_matching(automations_it->second, field_accessors);
    }
}

void AutomationService::fire_matching(
    const std::vector<Automation> &automations,
    const std::unordered_map<std::string, FieldFullAccessor> &field_accessors)
{
    for (const auto &automation : automations)
    {
        if (automation.condition.has_value())
        {
            spdlog::info("Evaluating conditions for automation {}", automation.id);
            if (!evaluate_condition(*automation.condition, field_accessors))
            {
                spdlog::info("Conditions not met for automation {}, skipping actions", automation.id);
                continue;
            }
        }
        for (const auto &action : automation.actions)
        {
            spdlog::info("Executing action {} for automation {}", action.target_field, automation.id);
            execute_action(action, field_accessors);
        }
    }
}

bool AutomationService::evaluate_condition(
    const ConditionGroup &group,
    const std::unordered_map<std::string, FieldFullAccessor> &field_accessors)
{
    auto eval_rule = [&](const ConditionRule &rule) -> bool
    {
        auto it = field_accessors.find(rule.field_name);
        if (it == field_accessors.end())
            return false;
        return compare(it->second.get_before(), rule.op, rule.value);
    };

    auto eval_group = [&](const ConditionGroup &g, auto &self) -> bool
    {
        bool result = (g.operator_ == "AND");
        for (const auto &rule : g.rules)
        {
            bool r = eval_rule(rule);
            result = (g.operator_ == "AND") ? (result && r) : (result || r);
        }
        if (g.groups.has_value())
        {
            for (const auto &nested : g.groups.value())
            {
                bool r = self(nested, self);
                result = (g.operator_ == "AND") ? (result && r) : (result || r);
            }
        }
        return result;
    };

    return eval_group(group, eval_group);
}

bool AutomationService::compare(
    const std::optional<LiteralValue> &actual,
    ConditionOp op,
    const std::optional<LiteralValue> &expected)
{
    if (!actual.has_value() || !expected.has_value())
    {
        if (op == ConditionOp::EQ)
            return actual.has_value() == expected.has_value();
        if (op == ConditionOp::NEQ)
            return actual.has_value() != expected.has_value();
        return false;
    }
    return std::visit([&](auto &&a, auto &&e) -> bool
                      {
        using A = std::decay_t<decltype(a)>;
        using E = std::decay_t<decltype(e)>;
        if constexpr (std::is_same_v<A, E>) {
            switch (op) {
                case ConditionOp::EQ:       return a == e;
                case ConditionOp::NEQ:      return a != e;
                case ConditionOp::GT:       return a >  e;
                case ConditionOp::LT:       return a <  e;
                case ConditionOp::GTE:      return a >= e;
                case ConditionOp::LTE:      return a <= e;
                case ConditionOp::CONTAINS:
                    if constexpr (std::is_same_v<A, std::string>)
                        return a.find(e) != std::string::npos;
                    return false;
            }
        }
        return false; }, actual.value(), expected.value());
}

void AutomationService::execute_action(
    const AutomationAction &action,
    const std::unordered_map<std::string, FieldFullAccessor> &accessors)
{
    auto it = accessors.find(action.target_field);
    if (it == accessors.end())
    {
        spdlog::warn("AutomationService: field {} not found", action.target_field);
        return;
    }
    auto &acc = it->second;
    switch (action.type)
    {
    case AutomationActionType::SET_FIELD:
        acc.set(resolve_value(action.value, accessors));
        break;
    case AutomationActionType::INCREMENT_FIELD:
    {
        auto resolved = resolve_value(action.value, accessors);
        if (acc.increment && resolved.has_value())
            acc.increment(std::get<int>(resolved.value()));
        break;
    }
    case AutomationActionType::DECREMENT_FIELD:
    {
        auto resolved = resolve_value(action.value, accessors);
        if (acc.increment && resolved.has_value())
            acc.increment(-std::get<int>(resolved.value()));
        break;
    }
    }
}

std::optional<LiteralValue> AutomationService::resolve_value(
    const AutomationValue &val,
    const std::unordered_map<std::string, FieldFullAccessor> &accessors)
{
    switch (val.kind)
    {
    case AutomationValueKind::LITERAL:
        return val.data.value();
    case AutomationValueKind::NOW:
        return std::format("{:%Y-%m-%dT%H:%M:%SZ}",
                           std::chrono::floor<std::chrono::seconds>(
                               std::chrono::system_clock::now()));
    case AutomationValueKind::NULL_VALUE:
        return std::string{};
    case AutomationValueKind::FIELD_COPY:
    {
        auto it = accessors.find(val.field_copy.value());
        if (it != accessors.end())
            return it->second.get();
        return std::string{};
    }
    }
    return std::string{};
}

std::string AutomationService::make_key(
    const std::string &entity,
    AutomationEvent event,
    AutomationTiming timing,
    std::optional<std::string> attribute)
{
    return entity + ":" +
           std::to_string(static_cast<int>(event)) + ":" +
           std::to_string(static_cast<int>(timing)) + ":" +
           attribute.value_or("*");
}

std::unordered_map<std::string, std::optional<LiteralValue>> AutomationService::snapshot(IAutomatable &target)
{
    std::unordered_map<std::string, std::optional<LiteralValue>> snap;
    for (const auto &[field, acc] : target.field_accessors())
        snap[field] = acc.get();
    return snap;
}