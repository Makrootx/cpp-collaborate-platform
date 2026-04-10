#pragma once
#include "crow.h"
// #include "shared/helpers/ValidateDto.hpp"
#include "shared/helpers/json/JsonDto.hpp"
#include "modules/users/domain/User.hpp"
#include <string>
#include <optional>

struct UserUpdateDto : public JsonDto<UserUpdateDto>
{

    long int id;
    std::optional<std::string> email;
    std::optional<std::string> password;

    static const std::vector<FieldDescriptor> &fields()
    {
        static const std::vector<FieldDescriptor> f = {
            field("id", &UserUpdateDto::id),
            optional_field("email", &UserUpdateDto::email),
            optional_field("password", &UserUpdateDto::password),
        };
        return f;
    };
};

struct UserCreateDto : public JsonDto<UserCreateDto>
{
public:
    std::string email;
    std::string password;

    static const std::vector<FieldDescriptor> &fields()
    {
        static const std::vector<FieldDescriptor> f = {
            field("email", &UserCreateDto::email),
            field("password", &UserCreateDto::password),
        };
        return f;
    };
};

struct UserAuthenticateDto : public JsonDto<UserAuthenticateDto>
{
public:
    std::string email;
    std::string password;

    static const std::vector<FieldDescriptor> &fields()
    {
        static const std::vector<FieldDescriptor> f = {
            field("email", &UserAuthenticateDto::email),
            field("password", &UserAuthenticateDto::password),
        };
        return f;
    };
};

struct UserResultDto : public JsonDto<UserResultDto>
{
public:
    long int id;
    std::string email;

    static const std::vector<FieldDescriptor> &fields()
    {
        static const std::vector<FieldDescriptor> f = {
            field("id", &UserResultDto::id),
            field("email", &UserResultDto::email),
        };
        return f;
    };

    static UserResultDto from_domain(const User &user)
    {
        return UserResultDto{.id = user.get_id(), .email = user.get_email()};
    }
};