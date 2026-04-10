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
        return UserResultDto::from_domain(user);
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

std::vector<UserResultDto> UserService::get_all_users()
{
    auto users = repo_->get_all();
    std::vector<UserResultDto> result;
    for (const auto &user : users)
    {
        result.push_back(UserResultDto::from_domain(user));
    }
    return result;
}

void UserService::update_user(const UserUpdateDto &dto)
{
    auto user_opt = repo_->find_by_id(dto.id);
    if (!user_opt)
    {
        throw std::runtime_error("User not found");
    }

    auto user = *user_opt;
    if (dto.email.has_value())
    {
        user.set_email(*dto.email);
    }
    if (dto.password.has_value())
    {
        user.set_password(*dto.password);
    }

    repo_->update(user);
}