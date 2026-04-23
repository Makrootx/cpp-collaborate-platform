#pragma once

#include "modules/automation/domain/AutomationDomain.hpp"
#include "shared/ports/IBaseRepo.hpp"

class IAutomationRepo : public virtual IBaseRepo<AutomationDomain>
{
public:
    virtual void ensure_space_loaded(long space_id) = 0;
    virtual std::optional<AutomationDomain> find_by_id_space_contexted(long space_id) = 0;
};