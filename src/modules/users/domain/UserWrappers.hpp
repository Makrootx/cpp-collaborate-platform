#pragma once
#include "modules/users/domain/User.hpp"
#include "shared/domain/AutomativeWrapper.hpp"
#include "shared/domain/SecureWrapper.hpp"
#include "modules/users/domain/IUserUse.hpp"
#include "shared/adapters/permission_core/PermissionUtils.hpp"

#include <spdlog/spdlog.h>

class AutomativeUser : public AutomativeWrapper<IUserUse, User>, public IUserUse
{
public:
    using AutomativeWrapper<IUserUse, User>::AutomativeWrapper;

    using UserSchemaAccessor = EntitySchemas::EntityAccessor::UserSchema;
    using UserA = EntitySchemas::EntityAccessor::UserSchema::Attributes;
    using UserR = EntitySchemas::EntityAccessor::UserSchema::Relations;

    std::string automation_entity() const override { return EntitySchemas::EntityAccessor::USERS; }

    std::unordered_map<std::string, FieldAccessor> field_accessors() override
    {
        return {
            {UserA::ID, FieldAccessor{.get = [this]()
                                      { return wrapper_->get_id(); },
                                      .set = [this](std::optional<LiteralValue> value) {}}},
            {UserA::EMAIL, FieldAccessor{.get = [this]()
                                         { return wrapper_->get_email(); },
                                         .set = [this](std::optional<LiteralValue> value) {}}},
            {"password", FieldAccessor{.get = [this]()
                                       { return wrapper_->get_password(); },
                                       .set = [this](std::optional<LiteralValue> value) {}}},
        };
    }

    long int get_id() const override
    {
        return perform(&IUserUse::get_id, read_trigger(UserA::ID));
    }

    void set_id(long int id) override
    {
        perform(&IUserUse::set_id, update_trigger(UserA::ID), id);
    };

    void set_email(std::string email) override
    {
        perform(&IUserUse::set_email, update_trigger(UserA::EMAIL), std::move(email));
    };

    std::string get_email() const override
    {
        return perform(&IUserUse::get_email, read_trigger(UserA::EMAIL));
    };

    void set_password(std::string password) override
    {
        perform(&IUserUse::set_password, update_trigger("password"), std::move(password));
    };

    const std::string &get_password() const override
    {
        return perform(&IUserUse::get_password, read_trigger("password"));
    };
};

class SecureUser : public SecureWrapper<IUserUse, User>, public IUserSecureUse
{
public:
    using SecureWrapper<IUserUse, User>::SecureWrapper;

    std::string get_domain_permission_name() const override { return EntitySchemas::EntityAccessor::USERS; }

    std::optional<long int> get_id() const override
    {
        return secure_get(&IUserUse::get_id, PermissionFactor::get_read_factor("id"));
    }

    bool set_id(long int id) override
    {
        return secure_set(&IUserUse::set_id, PermissionFactor::get_update_factor("id"), id);
    };

    bool set_email(std::string email) override
    {
        return secure_set(&IUserUse::set_email, PermissionFactor::get_update_factor("email"), std::move(email));
    };

    std::optional<std::string> get_email() const override
    {
        return secure_get(&IUserUse::get_email, PermissionFactor::get_read_factor("email"));
    };

    bool set_password(std::string password) override
    {
        return secure_set(&IUserUse::set_password, PermissionFactor::get_update_factor("password"), std::move(password));
    };

    std::optional<std::string> get_password() const override
    {
        return secure_get(&IUserUse::get_password, PermissionFactor::get_read_factor("password"));
    };
};