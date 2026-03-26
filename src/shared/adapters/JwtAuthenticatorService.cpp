#include "shared/adapters/JwtAuthenticatorService.hpp"
#include "shared/helpers/EnvProvider.hpp"

#include <jwt-cpp/traits/kazuho-picojson/defaults.h>
#include <jwt-cpp/jwt.h>
#include <chrono>
#include <cstdlib>
#include <string>
#include <spdlog/spdlog.h>

const JwtAuthenticatorService::JwtConfig &JwtAuthenticatorService::get_config()
{
    static const JwtConfig config = []
    {
        JwtConfig c;

        const auto jwt_key = EnvProvider::get(Env::JWT_KEY);
        if (!jwt_key.empty())
        {
            c.secret_key = jwt_key;
        }
        else
        {
            spdlog::error("JWT_SECRET_KEY environment variable is not set. JWT authentication will not work.");
            throw std::runtime_error("JWT_SECRET_KEY environment variable is not set.");
        }

        const auto jwt_expiry = EnvProvider::get(Env::JWT_TOKEN_EXPIRY);
        if (!jwt_expiry.empty())
        {
            try
            {
                c.token_expiry = std::chrono::hours(std::stoul(jwt_expiry));
            }
            catch (const std::exception &e)
            {
                spdlog::error("Invalid JWT_TOKEN_EXPIRY value '{}'. Using default of 24 hours. Error: {}", jwt_expiry, e.what());
                c.token_expiry = std::chrono::hours(24);
            }
        }
        else
        {
            spdlog::warn("JWT_TOKEN_EXPIRY environment variable is not set. Using default of 24 hours.");
            c.token_expiry = std::chrono::hours(24);
        }

        return c;
    }();

    return config;
}

std::string JwtAuthenticatorService::generate_Jwt(const std::string &user_id)
{
    using namespace std::chrono;
    const auto &jwt_config = get_config();
    auto now = system_clock::now();

    return jwt::create()
        .set_type("JWT")
        .set_issuer("cpp-collaborate-platform")
        .set_issued_at(now)
        .set_expires_at(now + jwt_config.token_expiry)
        .set_payload_claim("user_id", jwt::claim(user_id))
        .sign(jwt::algorithm::hs256{jwt_config.secret_key});
}

std::optional<std::string> JwtAuthenticatorService::validate_Jwt(const std::string &token)
{
    try
    {
        const auto &jwt_config = get_config();
        auto decoded = jwt::decode(token);

        jwt::verify()
            .allow_algorithm(jwt::algorithm::hs256{jwt_config.secret_key})
            .with_issuer("cpp-collaborate-platform")
            .verify(decoded);

        return decoded.get_payload_claim("user_id").as_string();
    }
    catch (const std::exception &e)
    {
        return std::nullopt;
    }
}