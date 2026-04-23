#pragma once

#include "shared/adapters/persistence/PgBaseRepo.hpp"
#include "modules/boards/ports/IBoardRepo.hpp"
#include "modules/boards/adapters/persistance/BoardOdb.hpp"
#include "modules/boards/adapters/persistance/BoardOdb-odb.hxx"
#include "modules/boardCategories/adapters/persistance/BoardCategoryOdb-odb.hxx"

template <typename Y>
    requires OdbMappable<Board, Y> && OdbBase<Y> && OdbMappableQuery<Board, Y>
class PgBoardRepo : public IBoardRepo, public PgBaseRepo<Board, Y>
{
public:
    using PgBaseRepo<Board, Y>::PgBaseRepo;

    void link_category(long int board_id, long int category_id) override
    {
        odb::transaction t(this->db_->begin());
        this->execute_context_query();
        auto board = this->db_->template find<Y>(static_cast<unsigned long>(board_id));
        if (!board)
            throw std::runtime_error("Board not found");
        board->add_category_ptr(odb::lazy_shared_ptr<BoardCategoryOdb>(*this->db_, static_cast<unsigned long>(category_id)));
        this->db_->update(*board);
        t.commit();
    }
};
