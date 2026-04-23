#include "modules/automation/adapters/AutomationLoader.hpp"
#include "shared/adapters/automation_core/AutomationService.hpp"

AutomationLoader::AutomationLoader(long int space_id) : space_id_(space_id)
{
    AutomationStaticService::ensure_space_loaded(space_id_);
}

AutomationLoader::~AutomationLoader()
{
    AutomationService::instance().unregister_space(space_id_);
}
