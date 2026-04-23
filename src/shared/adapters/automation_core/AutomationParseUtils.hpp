#pragma once

#include <optional>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <variant>

#include "shared/adapters/automation_core/AutomationEnums.hpp"
#include "shared/helpers/json/JsonDto.hpp"

using LiteralValue = std::variant<int, std::string, bool>;

/// @brief Bidirectional string conversion utilities for all automation-related enum types.
namespace AutomationParseUtils
{
    template <typename T>
    inline T from_string(const std::string &str);

    inline std::string to_string(AutomationValueKind p)
    {
        switch (p)
        {
        case AutomationValueKind::LITERAL:
            return "LITERAL";
        case AutomationValueKind::NOW:
            return "NOW";
        case AutomationValueKind::NULL_VALUE:
            return "NULL_VALUE";
        case AutomationValueKind::FIELD_COPY:
            return "FIELD_COPY";
        default:
            return "Unknown";
        }
    }

    template <>
    inline AutomationValueKind from_string<AutomationValueKind>(const std::string &str)
    {
        if (str == "LITERAL")
            return AutomationValueKind::LITERAL;
        if (str == "NOW")
            return AutomationValueKind::NOW;
        if (str == "NULL_VALUE")
            return AutomationValueKind::NULL_VALUE;
        if (str == "FIELD_COPY")
            return AutomationValueKind::FIELD_COPY;

        throw std::invalid_argument("Invalid AutomationValueKind string: " + str);
    }

    inline std::string to_string(AutomationActionType p)
    {
        switch (p)
        {
        case AutomationActionType::SET_FIELD:
            return "SET_FIELD";
        case AutomationActionType::INCREMENT_FIELD:
            return "INCREMENT_FIELD";
        case AutomationActionType::DECREMENT_FIELD:
            return "DECREMENT_FIELD";
        default:
            return "Unknown";
        }
    }

    template <>
    inline AutomationActionType from_string<AutomationActionType>(const std::string &str)
    {
        if (str == "SET_FIELD")
            return AutomationActionType::SET_FIELD;
        if (str == "INCREMENT_FIELD")
            return AutomationActionType::INCREMENT_FIELD;
        if (str == "DECREMENT_FIELD")
            return AutomationActionType::DECREMENT_FIELD;

        throw std::invalid_argument("Invalid AutomationActionType string: " + str);
    }

    inline std::string to_string(ConditionOp op)
    {
        switch (op)
        {
        case ConditionOp::EQ:
            return "eq";
        case ConditionOp::NEQ:
            return "neq";
        case ConditionOp::GT:
            return "gt";
        case ConditionOp::LT:
            return "lt";
        case ConditionOp::GTE:
            return "gte";
        case ConditionOp::LTE:
            return "lte";
        case ConditionOp::CONTAINS:
            return "contains";
        default:
            return "Unknown";
        }
    }

    template <>
    inline ConditionOp from_string<ConditionOp>(const std::string &str)
    {
        if (str == "eq")
            return ConditionOp::EQ;
        if (str == "neq")
            return ConditionOp::NEQ;
        if (str == "gt")
            return ConditionOp::GT;
        if (str == "lt")
            return ConditionOp::LT;
        if (str == "gte")
            return ConditionOp::GTE;
        if (str == "lte")
            return ConditionOp::LTE;
        if (str == "contains")
            return ConditionOp::CONTAINS;

        throw std::invalid_argument("Invalid ConditionOp string: " + str);
    }

    inline std::string to_string(AutomationEvent e)
    {
        switch (e)
        {
        case AutomationEvent::READ:
            return "READ";
        case AutomationEvent::UPDATE:
            return "UPDATE";
        case AutomationEvent::CREATE:
            return "CREATE";
        case AutomationEvent::DELETE_:
            return "DELETE";
        default:
            return "Unknown";
        }
    }

    template <>
    inline AutomationEvent from_string<AutomationEvent>(const std::string &str)
    {
        if (str == "READ")
            return AutomationEvent::READ;
        if (str == "UPDATE")
            return AutomationEvent::UPDATE;
        if (str == "CREATE")
            return AutomationEvent::CREATE;
        if (str == "DELETE")
            return AutomationEvent::DELETE_;

        throw std::invalid_argument("Invalid AutomationEvent string: " + str);
    }

    inline std::string to_string(AutomationTiming t)
    {
        switch (t)
        {
        case AutomationTiming::BEFORE:
            return "BEFORE";
        case AutomationTiming::AFTER:
            return "AFTER";
        default:
            return "Unknown";
        }
    }

    template <>
    inline AutomationTiming from_string<AutomationTiming>(const std::string &str)
    {
        if (str == "BEFORE")
            return AutomationTiming::BEFORE;
        if (str == "AFTER")
            return AutomationTiming::AFTER;

        throw std::invalid_argument("Invalid AutomationTiming string: " + str);
    }

    template <typename T>
    struct IsAutomationEnum : std::false_type
    {
    };

    template <>
    struct IsAutomationEnum<AutomationEvent> : std::true_type
    {
    };
    template <>
    struct IsAutomationEnum<AutomationTiming> : std::true_type
    {
    };
    template <>
    struct IsAutomationEnum<AutomationValueKind> : std::true_type
    {
    };
    template <>
    struct IsAutomationEnum<AutomationActionType> : std::true_type
    {
    };
    template <>
    struct IsAutomationEnum<ConditionOp> : std::true_type
    {
    };

    /// @brief Satisfied when T is a known automation enum with registered to_string/from_string conversions.
    template <typename T>
    concept AutomationEnumParsable =
        IsAutomationEnum<T>::value &&
        requires(T a, std::string s) {
            { to_string(a) } -> std::same_as<std::string>;
            { from_string<T>(s) } -> std::same_as<T>;
        };
}

template <typename T>
    requires AutomationParseUtils::AutomationEnumParsable<T>
struct JsonTypeHelper<T>
{
    static constexpr crow::json::type types[] = {crow::json::type::String};

    static T read(const crow::json::rvalue &value)
    {
        return AutomationParseUtils::from_string<T>(value.s());
    }

    static crow::json::wvalue write(const T &v)
    {
        return AutomationParseUtils::to_string(v);
    }
};

template <>
struct JsonTypeHelper<LiteralValue>
{
    static constexpr crow::json::type types[] = {crow::json::type::String,
                                                 crow::json::type::Number,
                                                 crow::json::type::False,
                                                 crow::json::type::True};

    static LiteralValue read(const crow::json::rvalue &value)
    {
        switch (value.t())
        {
        case crow::json::type::Number:
            return LiteralValue(static_cast<int>(value.i()));
        case crow::json::type::String:
            return LiteralValue(value.s());
        case crow::json::type::True:
            return LiteralValue(true);
        case crow::json::type::False:
            return LiteralValue(false);
        default:
            return LiteralValue(std::string(""));
        }
    }

    static crow::json::wvalue write(const LiteralValue &v)
    {
        return std::visit([](const auto &x)
                          { return crow::json::wvalue(x); }, v);
    }
};
