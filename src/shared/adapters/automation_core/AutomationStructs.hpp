#pragma once

#include <optional>
#include <string>
#include <vector>

#include "shared/adapters/automation_core/AutomationParseUtils.hpp"

struct AutomationValue : public JsonDto<AutomationValue>
{
    AutomationValueKind kind;
    std::optional<LiteralValue> data;
    std::optional<std::string> field_copy;

    static const std::vector<FieldDescriptor> &fields()
    {
        static const std::vector<FieldDescriptor> f = {
            field("kind", &AutomationValue::kind),
            optional_field("data", &AutomationValue::data),
            optional_field("field_copy", &AutomationValue::field_copy)};
        return f;
    }
};

struct ConditionRule : public JsonDto<ConditionRule>
{
    std::string field_name;
    ConditionOp op;
    std::optional<LiteralValue> value;

    static const std::vector<FieldDescriptor> &fields()
    {
        static const std::vector<FieldDescriptor> f = {
            field("field", &ConditionRule::field_name),
            field("op", &ConditionRule::op),
            optional_field("value", &ConditionRule::value),
        };
        return f;
    }
};

struct ConditionGroup : public JsonDto<ConditionGroup>
{
    std::string operator_;
    std::vector<ConditionRule> rules;
    std::optional<std::vector<ConditionGroup>> groups;

    static const std::vector<FieldDescriptor> &fields()
    {
        static const std::vector<FieldDescriptor> f = {
            field("operator", &ConditionGroup::operator_),
            nested_field("rules", &ConditionGroup::rules),
            optional_nested_field("groups", &ConditionGroup::groups),
        };
        return f;
    }
};

struct AutomationAction : public JsonDto<AutomationAction>
{
    AutomationActionType type;
    std::string target_field;
    AutomationValue value;

    static const std::vector<FieldDescriptor> &fields()
    {
        static const std::vector<FieldDescriptor> f = {
            field("type", &AutomationAction::type),
            field("target_field", &AutomationAction::target_field),
            nested_field("value", &AutomationAction::value),
        };
        return f;
    }
};

struct AutomationTrigger : public JsonDto<AutomationTrigger>
{
    AutomationEvent event;
    AutomationTiming timing;
    std::string entity;
    std::optional<std::string> attribute;

    static const std::vector<FieldDescriptor> &fields()
    {
        static const std::vector<FieldDescriptor> f = {
            field("event", &AutomationTrigger::event),
            field("timing", &AutomationTrigger::timing),
            field("entity", &AutomationTrigger::entity),
            optional_field("attribute", &AutomationTrigger::attribute),
        };
        return f;
    }
};

struct Automation : public JsonDto<Automation>
{
    std::string id;
    std::string description;
    AutomationTrigger trigger;
    std::optional<ConditionGroup> condition;
    std::vector<AutomationAction> actions;

    static const std::vector<FieldDescriptor> &fields()
    {
        static const std::vector<FieldDescriptor> f = {
            field("id", &Automation::id),
            field("description", &Automation::description),
            nested_field("trigger", &Automation::trigger),
            optional_nested_field("condition", &Automation::condition),
            nested_field("actions", &Automation::actions),
        };
        return f;
    }
};

struct AutomationConfig : public JsonDto<AutomationConfig>
{
    std::vector<Automation> automations = {};

    static const std::vector<FieldDescriptor> &fields()
    {
        static const std::vector<FieldDescriptor> f = {
            nested_field("automations", &AutomationConfig::automations),
        };
        return f;
    }
};
