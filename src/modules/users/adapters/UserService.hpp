#pragma once

#include "modules/users/ports/IUserService.hpp"
#include "modules/users/ports/IUserRepo.hpp"
#include "modules/users/adapters/dto/UserDto.hpp"
#include <memory>

/**
 * @brief User use-case implementation backed by IUserRepo.
 */
class UserService : public IUserService
{
    std::shared_ptr<IUserRepo> repo_;

public:
    explicit UserService(std::shared_ptr<IUserRepo> repo_) : repo_(std::move(repo_)) {};

    /** @brief Creates and persists new user from DTO payload. */
    void create_user(const UserCreateDto &dto) override;
    /** @brief Returns user DTO by id when user exists. */
    std::optional<UserResultDto> get_user_by_id(unsigned long id) override;
    /** @brief Authenticates credentials and returns JWT when successful. */
    std::optional<std::string> authenticate_user(const UserAuthenticateDto &dto) override;
};