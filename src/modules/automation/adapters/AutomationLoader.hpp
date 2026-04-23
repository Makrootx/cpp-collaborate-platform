#pragma once

#include "modules/automation/adapters/AutomationStaticService.hpp"

class AutomationLoader
{
    long int space_id_;

public:
    explicit AutomationLoader(long int space_id);
    ~AutomationLoader();
};
