#pragma once
#include <string>
#include <optional>
#include <chrono>

/**
 * @brief JWT helper for token issuance and validation.
 */
class JwtAuthenticatorService
{
    struct JwtConfig
    {
        std::string secret_key;
        std::chrono::hours token_expiry;
    };
    static const JwtConfig &get_config();

public:
    /** @brief Validates token and returns subject user id when valid. */
    static std::optional<std::string> validate_Jwt(const std::string &token);
    /** @brief Generates token for supplied user id. */
    static std::string generate_Jwt(const std::string &user_id);
};