#pragma once
#include "modules/groups/domain/Group.hpp"
#include "shared/ports/IOdbMappable.hpp"
#include "shared/ports/IOdbMappableQuery.hpp"
#include "shared/adapters/persistence/PgBaseRepo.hpp"
#include "modules/groups/ports/IGroupRepo.hpp"

template <typename Y>
    requires OdbMappable<Group, Y> && OdbBase<Y> && OdbMappableQuery<Group, Y>
class PgGroupRepo : public IGroupRepo, public PgBaseRepo<Group, Y>
{
public:
    using PgBaseRepo<Group, Y>::PgBaseRepo;
};