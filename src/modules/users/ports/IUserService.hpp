#pragma once

#include <string>
#include <optional>
#include <vector>
#include "modules/users/adapters/dto/UserDto.hpp"

/** @brief Application service contract for user-related use cases. */
class IUserService
{
public:
    virtual ~IUserService() = default;

    /** @brief Registers a new user using validated create DTO payload. */
    virtual void create_user(const UserCreateDto &dto) = 0;
    /** @brief Returns user DTO by identifier when found. */
    virtual std::optional<UserResultDto> get_user_by_id(unsigned long id) = 0;
    /** @brief Authenticates user credentials and returns JWT when successful. */
    virtual std::optional<std::string> authenticate_user(const UserAuthenticateDto &dto) = 0;

    virtual std::vector<UserResultDto> get_all_users() = 0;

    virtual void update_user(const UserUpdateDto &dto) = 0;
};