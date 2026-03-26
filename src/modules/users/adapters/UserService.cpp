#include "modules/users/adapters/persistence/PgUserRepo.hpp"
#include "modules/users/adapters/UserService.hpp"
#include "shared/adapters/JwtAuthenticatorService.hpp"

#include <spdlog/spdlog.h>

void UserService::create_user(const UserCreateDto &dto)
{
    User user(dto.email, dto.password);
    repo_->create(user);
}

std::optional<UserResultDto> UserService::get_user_by_id(unsigned long id)
{
    auto user_opt = repo_->find_by_id(id);
    if (user_opt)
    {
        const auto &user = *user_opt;
        return UserResultDto{user.get_id(), user.get_email()};
    }
    return std::nullopt;
}

std::optional<std::string> UserService::authenticate_user(const UserAuthenticateDto &dto)
{
    auto users = repo_->find_by_email(dto.email);
    if (users.empty())
    {
        return std::nullopt;
    }

    const auto &user = users[0];
    if (user.verify_password(dto.password))
    {
        return JwtAuthenticatorService::generate_Jwt(std::to_string(user.get_id()));
    }
    return std::nullopt;
}