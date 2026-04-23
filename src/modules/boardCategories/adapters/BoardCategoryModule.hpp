#pragma once

#include "modules/boardCategories/ports/IBoardCategoryRepo.hpp"
#include "modules/boardCategories/ports/IBoardCategoryService.hpp"
#include "modules/boardCategories/adapters/persistance/PgBoardCategoryRepo.hpp"
#include "modules/boardCategories/adapters/BoardCategoryService.hpp"
#include "modules/boardCategories/adapters/persistance/BoardCategoryOdb.hpp"
#include "modules/boards/adapters/persistance/PgBoardRepo.hpp"
#include "modules/boards/adapters/persistance/BoardOdb.hpp"
#include "shared/adapters/permission_core/PermissionStruct.hpp"
#include <odb/database.hxx>
#include <odb/transaction.hxx>

class BoardCategoryModule
{
    std::shared_ptr<odb::database> db_;

public:
    explicit BoardCategoryModule(std::shared_ptr<odb::database> db) : db_(std::move(db)) {}

    BoardCategoryService get_contexted_service(long int space_id, GroupPermission required_permission = {})
    {
        auto context = "space_" + std::to_string(space_id);
        auto repo = std::make_shared<PgBoardCategoryRepo<BoardCategoryOdb>>(db_, context);
        auto board_repo = std::make_shared<PgBoardRepo<BoardOdb>>(db_, context);
        return BoardCategoryService(space_id, repo, board_repo, std::move(required_permission));
    }
};
