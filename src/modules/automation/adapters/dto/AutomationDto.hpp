#pragma once

#include "shared/helpers/json/JsonDto.hpp"
#include "shared/adapters/automation_core/AutomationStructs.hpp"

// GET: returns the full automation config stored for the space
class AutomationResultDto : public JsonDto<AutomationResultDto>
{
public:
    long int id;
    AutomationConfig config;

    static const std::vector<FieldDescriptor> &fields()
    {
        static const std::vector<FieldDescriptor> f = {
            field("id", &AutomationResultDto::id),
            nested_field("config", &AutomationResultDto::config),
        };
        return f;
    }

    static AutomationResultDto from_domain(const AutomationDomain &domain)
    {
        return AutomationResultDto{
            .id = domain.get_id(),
            .config = domain.get_config()};
    }
};

// PUT: replaces the full automation config for the space
class AutomationUpdateDto : public JsonDto<AutomationUpdateDto>
{
public:
    AutomationConfig config;

    static const std::vector<FieldDescriptor> &fields()
    {
        static const std::vector<FieldDescriptor> f = {
            nested_field("config", &AutomationUpdateDto::config),
        };
        return f;
    }
};
