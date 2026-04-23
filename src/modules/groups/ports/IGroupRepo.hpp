#pragma once
#include "modules/groups/domain/Group.hpp"
#include "shared/ports/IBaseRepo.hpp"

class IGroupRepo : public virtual IBaseRepo<Group>
{
public:
};