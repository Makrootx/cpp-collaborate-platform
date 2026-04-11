#pragma once
#include "modules/spaces/ports/ISpaceRepo.hpp"
#include "shared/adapters/persistence/PgBaseRepo.hpp"
#include "modules/spaces/adapters/persistence/SpaceOdb.hpp"
#include "modules/spaces/adapters/persistence/SpaceOdb-odb.hxx"
#include "modules/spaces/domain/Space.hpp"
#include <concepts>

/** @brief PostgreSQL repository implementation for Space aggregate. */
template <typename Y>
    requires OdbMappable<Space, Y> && OdbBase<Y> && OdbMappableQuery<Space, Y>
class PgSpaceRepo : public ISpaceRepo, public PgBaseRepo<Space, Y>
{
public:
    using PgBaseRepo<Space, Y>::PgBaseRepo;

    /** @brief Persists space and related ownership/membership rows. */
    long int create(const Space &domain) override;
    /** @brief Returns spaces that are visible to the specified user. */
    std::vector<Space> find_user_spaces(long int user_id) override;
};