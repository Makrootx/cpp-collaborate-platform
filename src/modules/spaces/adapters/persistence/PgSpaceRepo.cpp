#include "modules/spaces/adapters/persistence/PgSpaceRepo.hpp"
#include "modules/spaces/adapters/persistence/SpaceOdb-odb.hxx"
#include "modules/users/adapters/persistence/UserOdb-odb.hxx"

#include <string>

template <typename Y>
    requires OdbMappable<Space, Y> && OdbBase<Y> && OdbMappableQuery<Space, Y>
long int PgSpaceRepo<Y>::create(const Space &domain)
{
    const auto space_id = PgBaseRepo<Space, Y>::create(domain);

    const std::string schema_name = "space_" + std::to_string(space_id);
    const std::string create_schema_sql = "CREATE SCHEMA IF NOT EXISTS \"" + schema_name + "\"";

    odb::transaction tx(this->db_->begin());
    this->db_->execute(create_schema_sql);
    tx.commit();

    return space_id;
}

template <typename Y>
    requires OdbMappable<Space, Y> && OdbBase<Y> && OdbMappableQuery<Space, Y>
std::vector<Space> PgSpaceRepo<Y>::find_user_spaces(long int user_id)
{
    std::vector<Space> result;

    odb::transaction t(this->db_->begin());

    // Query spaces where user is a member (via join table)
    // OR where user is the owner
    auto rows = this->db_->template query<Y>(
        "id IN ("
        "SELECT space_id FROM space_members WHERE user_id = " +
        odb::query<Y>::_val(static_cast<unsigned long>(user_id)) +
        ") OR owner_id = " +
        odb::query<Y>::_val(static_cast<unsigned long>(user_id)));

    for (auto it = rows.begin(); it != rows.end(); ++it)
    {
        // Load lazy relations while transaction is still open
        // it.load();
        result.push_back(this->to_domain(*it));
    }

    t.commit();
    return result;
}

template class PgSpaceRepo<SpaceOdb>;