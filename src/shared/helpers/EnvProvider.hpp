#pragma once

#include "shared/helpers/env_keys.hpp"

#include <string>
#include <map>

/// @brief Provides access to application configuration resolved from environment variables with fallback defaults.
class EnvProvider
{
protected:
    static std::map<std::string, std::string> env_values;
    static const std::map<std::string, std::string> env_default_values;

public:
    EnvProvider() = delete;

    /// @brief Loads all known environment variables, applying defaults when a variable is absent.
    static void init();
    /// @brief Returns the value of a named environment variable, or an empty string if not found.
    static std::string get(const std::string &key);
};