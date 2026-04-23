#pragma once

enum class AutomationEvent
{
    READ,
    UPDATE,
    CREATE,
    DELETE_
};

enum class AutomationTiming
{
    BEFORE,
    AFTER
};

enum class AutomationValueKind
{
    LITERAL,
    NOW,
    NULL_VALUE,
    FIELD_COPY
};

enum class AutomationActionType
{
    SET_FIELD,
    INCREMENT_FIELD,
    DECREMENT_FIELD
};

enum class ConditionOp
{
    EQ,
    NEQ,
    GT,
    LT,
    GTE,
    LTE,
    CONTAINS
};