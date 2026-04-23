#pragma once
#include "modules/boardCategories/domain/BoardCategory.hpp"
#include "shared/ports/IBaseRepo.hpp"

class IBoardCategoryRepo : public virtual IBaseRepo<BoardCategory>
{
public:
    virtual void link_task(long int category_id, long int task_id) = 0;
};
