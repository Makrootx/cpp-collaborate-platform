#pragma once
#include "modules/boards/domain/Board.hpp"
#include "shared/ports/IBaseRepo.hpp"

class IBoardRepo : public virtual IBaseRepo<Board>
{
public:
    virtual void link_category(long int board_id, long int category_id) = 0;
};
