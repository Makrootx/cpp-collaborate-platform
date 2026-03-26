#pragma once
#include "modules/users/domain/User.hpp"
#include "shared/ports/IOdbMappable.hpp"
#include "shared/adapters/persistence/odb/BaseOdb.hpp"

#include <odb/core.hxx>
#include <memory>

#pragma db object table("users") pointer(std::shared_ptr)
/**
 * @brief ODB persistence model for users table.
 *
 * Contains storage representation and static conversion helpers for User
 * domain mapping.
 */
class UserOdb : public BaseOdb, public IOdbMappable<User, UserOdb>
{
public:
    UserOdb() = default;
    // explicit UserOdb(std::string email, std::string password) : email_(std::move(email)), password_(std::move(password)), id_(0) {};
    explicit UserOdb(std::string email, std::string password, unsigned long id = 0) : email_(std::move(email)), password_(std::move(password)), BaseOdb(id) {};

    UserOdb(const UserOdb &) = default;
    UserOdb(UserOdb &&) noexcept = default;
    UserOdb &operator=(UserOdb &&) noexcept = default;
    UserOdb &operator=(const UserOdb &) = default;
    ~UserOdb() = default;

    const std::string &email_value() const { return email_; }
    const std::string &password_value() const { return password_; }

    static User to_domain(const UserOdb &odb);
    static UserOdb to_odb(const User &domain);

private:
    friend class odb::access;

#pragma db column("email") type("VARCHAR(255)")
    std::string email_;
#pragma db column("password") type("VARCHAR(255)")
    std::string password_;
};