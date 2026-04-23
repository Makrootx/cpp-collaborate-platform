#pragma once
#include "modules/automation/domain/AutomationDomain.hpp"
#include "shared/adapters/automation_core/AutomationService.hpp"
#include "shared/ports/IOdbMappable.hpp"
#include "shared/ports/IOdbMappableQuery.hpp"
#include "shared/adapters/persistence/PgBaseRepo.hpp"
#include "modules/automation/ports/IAutomationRepo.hpp"

template <typename Y>
    requires OdbMappable<AutomationDomain, Y> && OdbBase<Y> && OdbMappableQuery<AutomationDomain, Y>
class PgAutomationRepo : public IAutomationRepo, public PgBaseRepo<AutomationDomain, Y>
{
public:
    using PgBaseRepo<AutomationDomain, Y>::PgBaseRepo;
    using Odb = Y;
    using Domain = AutomationDomain;

    void ensure_space_loaded(long space_id) override
    {
        auto &svc = AutomationService::instance();
        if (!svc.has_space(space_id))
        {
            auto automation = find_by_id_space_contexted(space_id);
            if (!automation.has_value())
            {
                spdlog::error("Failed to load automation config for space {}: no automation config found with id {}", space_id, space_id);
                return;
            }
            svc.register_space(space_id, std::move(automation.value().get_config()));
        }
    }

    std::optional<AutomationDomain> find_by_id_space_contexted(long space_id) override
    {
        try
        {
            odb::session s;
            odb::transaction t(this->db_->begin());
            this->execute_context_query("space_" + std::to_string(space_id));
            using id_type = typename odb::object_traits<Odb>::id_type;
            std::optional<Domain> result;

            if (auto loaded = this->db_->template find<Odb>(static_cast<id_type>(1)); loaded)
            {
                result = this->to_domain(*loaded);
            }

            t.commit();
            return result;
        }
        catch (const std::exception &)
        {
            return std::nullopt;
        }
    }
};