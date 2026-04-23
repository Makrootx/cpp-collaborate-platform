#pragma once

#include "modules/automation/adapters/persistance/PgAutomationRepo.hpp"

class AutomationStaticService
{
private:
    inline static std::shared_ptr<IAutomationRepo> repo_;
    explicit AutomationStaticService() = default;

public:
    static void initialize(std::shared_ptr<IAutomationRepo> repo)
    {
        instance().repo_ = std::move(repo);
    }

    static AutomationStaticService &instance()
    {
        static AutomationStaticService instance;
        return instance;
    }

    static void ensure_space_loaded(long space_id)
    {
        instance().repo_->ensure_space_loaded(space_id);
    }
};