#pragma once

#include "modules/tasks/ports/ITaskRepo.hpp"
#include "modules/tasks/ports/ITaskService.hpp"

#include "modules/tasks/adapters/persistance/PgTaskRepo.hpp"
#include "modules/tasks/adapters/TaskService.hpp"

#include "modules/tasks/adapters/persistance/TaskOdb.hpp"

#include "modules/boardCategories/adapters/persistance/PgBoardCategoryRepo.hpp"
#include "modules/boardCategories/adapters/persistance/BoardCategoryOdb.hpp"

#include <odb/database.hxx>
#include <odb/transaction.hxx>

class TaskModule
{
    std::shared_ptr<odb::database> db_;
    std::shared_ptr<ISpaceRepo> space_repo_;

public:
    explicit TaskModule(std::shared_ptr<odb::database> db, std::shared_ptr<ISpaceRepo> space_repo) : db_(std::move(db)), space_repo_(std::move(space_repo)) {}

    TaskService get_contexted_service(long int space_id, GroupPermission required_permission = {})
    {
        auto context = "space_" + std::to_string(space_id);

        auto repo = std::make_shared<PgTaskRepo<TaskOdb>>(db_, context);
        auto category_repo = std::make_shared<PgBoardCategoryRepo<BoardCategoryOdb>>(db_, context);

        return TaskService(space_id, required_permission, repo, space_repo_, category_repo);
    };

    PgTaskRepo<TaskOdb> get_contexted_repo(std::string context)
    {
        return PgTaskRepo<TaskOdb>(db_, context);
    }
};