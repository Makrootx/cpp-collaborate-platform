#pragma once
#include "shared/adapters/persistence/PgBaseRepo.hpp"
#include "modules/users/ports/IUserRepo.hpp"
#include "modules/users/adapters/persistence/UserOdb.hpp"
#include "modules/users/adapters/persistence/UserOdb-odb.hxx"
#include <concepts>

/** @brief PostgreSQL repository implementation for User aggregate. */
template <typename Y>
    requires OdbMappable<User, Y> && OdbBase<Y>
class PgUserRepo : public IUserRepo, public PgBaseRepo<User, Y>
{
public:
    using PgBaseRepo<User, Y>::PgBaseRepo;

    /** @brief Returns users by email lookup. */
    std::vector<User> find_by_email(const std::string &email) override;
};