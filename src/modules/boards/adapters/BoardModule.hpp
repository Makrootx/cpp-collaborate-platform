#pragma once

#include "modules/boards/ports/IBoardRepo.hpp"
#include "modules/boards/ports/IBoardService.hpp"
#include "modules/boards/adapters/persistance/PgBoardRepo.hpp"
#include "modules/boards/adapters/BoardService.hpp"
#include "modules/boards/adapters/persistance/BoardOdb.hpp"
#include "shared/adapters/permission_core/PermissionStruct.hpp"
#include <odb/database.hxx>

class BoardModule
{
    std::shared_ptr<odb::database> db_;

public:
    explicit BoardModule(std::shared_ptr<odb::database> db) : db_(std::move(db)) {}

    BoardService get_contexted_service(long int space_id, GroupPermission required_permission = {})
    {
        auto context = "space_" + std::to_string(space_id);
        auto repo = std::make_shared<PgBoardRepo<BoardOdb>>(db_, context);
        return BoardService(space_id, repo, std::move(required_permission));
    }
};
