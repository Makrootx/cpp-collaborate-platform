#pragma once

#include "shared/adapters/persistence/PgBaseRepo.hpp"
#include "modules/tasks/ports/ITaskRepo.hpp"

#include "modules/tasks/adapters/persistance/TaskOdb.hpp"
#include "modules/tasks/adapters/persistance/TaskOdb-odb.hxx"
#include <concepts>

template <typename Y>
    requires OdbMappable<Task, Y> && OdbBase<Y> && OdbMappableQuery<Task, Y>
class PgTaskRepo : public ITaskRepo, public PgBaseRepo<Task, Y>
{
public:
    using PgBaseRepo<Task, Y>::PgBaseRepo;

    // Additional query methods specific to Task repository can be declared here.
};