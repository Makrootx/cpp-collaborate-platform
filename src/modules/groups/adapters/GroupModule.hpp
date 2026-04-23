#pragma once

#include "modules/groups/ports/IGroupRepo.hpp"
#include "modules/groups/ports/IGroupService.hpp"

#include "modules/groups/adapters/persistance/PgGroupRepo.hpp"
#include "modules/groups/adapters/GroupService.hpp"

#include "modules/groups/adapters/persistance/GroupOdb.hpp"
#include "modules/groups/adapters/persistance/GroupOdb-odb.hxx"

#include <odb/database.hxx>
#include <odb/transaction.hxx>

class GroupModule
{
    std::shared_ptr<odb::database> db_;
    std::shared_ptr<ISpaceRepo> space_repo_;

public:
    explicit GroupModule(std::shared_ptr<odb::database> db, std::shared_ptr<ISpaceRepo> space_repo) : db_(std::move(db)), space_repo_(std::move(space_repo)) {}

    GroupService get_contexted_service(long int space_id)
    {
        auto context = "space_" + std::to_string(space_id);

        auto repo = std::make_shared<PgGroupRepo<GroupOdb>>(db_, context);

        return GroupService(space_id, repo, space_repo_);
    };

    PgGroupRepo<GroupOdb> get_contexted_repo(std::string context)
    {
        return PgGroupRepo<GroupOdb>(db_, context);
    }
};