#pragma once
#include "shared/domain/BaseDomainUse.hpp"
#include <string>
class User;

class IUserUse : public virtual BaseDomainUse<User>
{

public:
    virtual ~IUserUse() = default;
    virtual void set_email(std::string email) = 0;
    virtual std::string get_email() const = 0;
    virtual void set_password(std::string password) = 0;
    virtual const std::string &get_password() const = 0;
};

class IUserSecureUse : public virtual BaseDomainSecureUse<User>
{
public:
    virtual ~IUserSecureUse() = default;
    virtual bool set_email(std::string email) = 0;
    virtual std::optional<std::string> get_email() const = 0;
    virtual bool set_password(std::string password) = 0;
    virtual std::optional<std::string> get_password() const = 0;
};