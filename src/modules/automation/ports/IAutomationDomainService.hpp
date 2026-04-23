#pragma once

#include "modules/automation/adapters/dto/AutomationDto.hpp"

class IAutomationDomainService
{
public:
    virtual ~IAutomationDomainService() = default;

    virtual AutomationResultDto get_automation() = 0;
    virtual void update_automation(const AutomationUpdateDto &dto) = 0;
};