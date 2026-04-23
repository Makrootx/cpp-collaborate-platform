#pragma once

#include "modules/automation/ports/IAutomationRepo.hpp"
#include "modules/automation/ports/IAutomationDomainService.hpp"
#include "modules/automation/adapters/AutomationDomainService.hpp"
#include "modules/automation/adapters/persistance/PgAutomationRepo.hpp"
#include "modules/automation/adapters/persistance/AutomationDomainOdb.hpp"
#include "modules/automation/adapters/persistance/AutomationDomainOdb-odb.hxx"

#include <odb/database.hxx>
#include <memory>

class AutomationModule
{
    std::shared_ptr<odb::database> db_;

public:
    explicit AutomationModule(std::shared_ptr<odb::database> db)
        : db_(std::move(db)) {}

    AutomationDomainService get_contexted_service(long int space_id)
    {
        auto context = "space_" + std::to_string(space_id);
        auto repo = std::make_shared<PgAutomationRepo<AutomationDomainOdb>>(db_, context);
        return AutomationDomainService(space_id, repo);
    }
};
