#pragma once
#include "modules/spaces/domain/Space.hpp"
#include "shared/ports/IBaseRepo.hpp"

/** @brief Persistence contract for space aggregate operations. */
class ISpaceRepo : public virtual IBaseRepo<Space>
{
public:
    /** @brief Returns spaces where user is owner or member. */
    virtual std::vector<Space> find_user_spaces(long int user_id) = 0;
    // virtual std::vector<Space> (long int user_id) = 0;
};