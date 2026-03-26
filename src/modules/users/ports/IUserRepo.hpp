#pragma once
#include "modules/users/domain/User.hpp"
#include "shared/ports/IBaseRepo.hpp"
#include <optional>
#include <string>
#include <vector>

/** @brief Persistence contract for user aggregate operations. */
class IUserRepo : public virtual IBaseRepo<User>
{
public:
    /** @brief Finds all users with matching email value. */
    virtual std::vector<User> find_by_email(const std::string &email) = 0;
};