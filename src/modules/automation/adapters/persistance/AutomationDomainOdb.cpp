#include "modules/automation/adapters/persistance/AutomationDomainOdb-odb.hxx"
#include "modules/automation/adapters/persistance/AutomationDomainOdb.hpp"

AutomationDomain AutomationDomainOdb::to_domain(const AutomationDomainOdb &odb)
{
    return AutomationDomain(odb.config_value(), odb.id_value());
}

AutomationDomainOdb AutomationDomainOdb::to_odb(const AutomationDomain &domain)
{
    return AutomationDomainOdb(domain.get_config(), domain.get_id());
}