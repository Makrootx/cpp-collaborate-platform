#include "modules/users/adapters/persistence/PgUserRepo.hpp"
#include "modules/users/adapters/persistence/UserOdb-odb.hxx"

template <typename Y>
    requires OdbMappable<User, Y> && OdbBase<Y> && OdbMappableQuery<User, Y>
std::vector<User> PgUserRepo<Y>::find_by_email(const std::string &email)
{
    odb::transaction t(this->db_->begin());
    std::vector<User> result;
    auto query = odb::query<Y>::email == email;
    auto rows = this->db_->template query<Y>(query);

    for (const auto &odb_user : rows)
    {
        result.push_back(this->to_domain(odb_user));
    }

    t.commit();
    return result;
}

template class PgUserRepo<UserOdb>;