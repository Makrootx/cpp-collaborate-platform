#pragma once
#include "modules/tasks/domain/Task.hpp"
#include "shared/ports/IBaseRepo.hpp"

class ITaskRepo : public virtual IBaseRepo<Task>
{
public:
};