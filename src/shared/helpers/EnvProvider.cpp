#include "shared/helpers/EnvProvider.hpp"
#include <spdlog/spdlog.h>
#include <cstdlib>

std::map<std::string, std::string> EnvProvider::env_values = {};

const std::map<std::string, std::string> EnvProvider::env_default_values = {
    {Env::DB_USER, "postgres"},
    {Env::DB_NAME, "postgres"},
    {Env::DB_PASS, "password"},
    {Env::DB_HOST, "localhost"},
    {Env::DB_PORT, "5432"},
    {Env::DB_POOL_MAX, "20"},
    {Env::DB_POOL_MIN, "5"},
    {Env::HTTP_HOST, "0.0.0.0"},
    {Env::HTTP_PORT, "8081"},
    {Env::JWT_KEY, "dev-jwt-secret"},
    {Env::JWT_TOKEN_EXPIRY, "24"},
    {Env::LOG_LEVEL, "info"},
    {Env::LOG_FILE, "logs/app.log"},
};

void EnvProvider::init()
{
    for (const auto &key : Env::ALL)
    {
        const auto default_it = env_default_values.find(key);
        const std::string default_value =
            (default_it != env_default_values.end()) ? default_it->second : std::string{};

        if (const char *env_val = std::getenv(key))
        {
            env_values[key] = env_val;
        }
        else
        {
            env_values[key] = default_value;
            if (!default_value.empty())
            {
                spdlog::warn("Environment variable '{}' not set, defaulting to '{}'", key, default_value);
            }
            else
            {
                spdlog::warn("Environment variable '{}' not set and no default value is configured", key);
            }
        }
    }
}

std::string EnvProvider::get(const std::string &key)
{
    auto it = env_values.find(key);
    if (it != env_values.end())
    {
        return it->second;
    }
    else
    {
        spdlog::warn("Requested environment variable '{}' not found, returning empty string", key);
        return "";
    }
}
