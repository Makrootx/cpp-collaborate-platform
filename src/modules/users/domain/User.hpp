#pragma once
#include "shared/domain/BaseDomain.hpp"
#include <string>

/**
 * @brief User aggregate with credential management rules.
 */
class User : public BaseDomain
{
protected:
    std::string password;
    std::string email;
    /** @brief Hashes plaintext password before storing it in domain state. */
    std::string hash_password(const std::string &password);

public:
    User() = default;
    explicit User(std::string email, std::string password) : email(std::move(email)), password(hash_password(std::move(password))) {};
    explicit User(std::string email, std::string password, unsigned long id) : email(std::move(email)), password(hash_password(std::move(password))), BaseDomain(id) {};

    /** @brief Reconstructs user from already hashed persisted credentials. */
    static User from_persisted(std::string email, std::string hashed_password, unsigned long id)
    {
        User user;
        user.email = std::move(email);
        user.password = std::move(hashed_password);
        user.id = id;
        return user;
    }

    User(const User &) = default;
    User(User &&) noexcept = default;
    User &operator=(User &&) noexcept = default;
    User &operator=(const User &) = default;
    ~User() = default;

    void set_email(std::string email) { this->email = std::move(email); };
    std::string get_email() const { return email; };
    void set_password(std::string password) { this->password = this->hash_password(std::move(password)); };
    const std::string &get_password() const { return password; }
    /** @brief Verifies input password against stored hash. */
    bool verify_password(const std::string &password) const;
};