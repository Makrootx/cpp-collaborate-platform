#pragma once

#include "modules/automation/ports/IAutomationDomainService.hpp"
#include "modules/automation/ports/IAutomationRepo.hpp"
#include "shared/adapters/automation_core/AutomationService.hpp"

#include <memory>
#include <stdexcept>

class AutomationDomainService : public IAutomationDomainService
{
    long int space_id_;
    std::shared_ptr<IAutomationRepo> repo_;

public:
    explicit AutomationDomainService(long int space_id, std::shared_ptr<IAutomationRepo> repo)
        : space_id_(space_id), repo_(std::move(repo)) {}

    AutomationResultDto get_automation() override
    {
        auto domain = repo_->find_by_id_space_contexted(space_id_);
        if (!domain.has_value())
        {
            throw std::runtime_error("Automation config not found for space " + std::to_string(space_id_));
        }
        return AutomationResultDto::from_domain(domain.value());
    }

    void update_automation(const AutomationUpdateDto &dto) override
    {
        auto domain = repo_->find_by_id_space_contexted(space_id_);
        if (!domain.has_value())
        {
            throw std::runtime_error("Automation config not found for space " + std::to_string(space_id_));
        }
        domain->set_config(dto.config);
        repo_->update(domain.value());

        // Re-register the updated config in the runtime engine
        AutomationService::instance().unregister_space(space_id_);
        AutomationService::instance().register_space(space_id_, dto.config);
    }
};
