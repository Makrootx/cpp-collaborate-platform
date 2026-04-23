#pragma once

#include "shared/adapters/persistence/PgBaseRepo.hpp"
#include "modules/boardCategories/ports/IBoardCategoryRepo.hpp"
#include "modules/boardCategories/adapters/persistance/BoardCategoryOdb.hpp"
#include "modules/boardCategories/adapters/persistance/BoardCategoryOdb-odb.hxx"
#include "modules/tasks/adapters/persistance/TaskOdb-odb.hxx"

template <typename Y>
    requires OdbMappable<BoardCategory, Y> && OdbBase<Y> && OdbMappableQuery<BoardCategory, Y>
class PgBoardCategoryRepo : public IBoardCategoryRepo, public PgBaseRepo<BoardCategory, Y>
{
public:
    using PgBaseRepo<BoardCategory, Y>::PgBaseRepo;

    void link_task(long int category_id, long int task_id) override
    {
        odb::transaction t(this->db_->begin());
        this->execute_context_query();
        auto cat = this->db_->template find<Y>(static_cast<unsigned long>(category_id));
        if (!cat)
            throw std::runtime_error("BoardCategory not found");
        cat->add_task_ptr(odb::lazy_shared_ptr<TaskOdb>(*this->db_, static_cast<unsigned long>(task_id)));
        this->db_->update(*cat);
        t.commit();
    }
};
