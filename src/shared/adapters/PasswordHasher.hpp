#pragma once
#include <string>

/**
 * @brief Argon2-based password hashing and verification helper.
 */
class PasswordHasher
{
public:
    /** @brief Produces secure hash for plaintext password. */
    static std::string hash(const std::string &password);
    /** @brief Verifies plaintext password against stored hash. */
    static bool verify(const std::string &password, const std::string &hash);
};