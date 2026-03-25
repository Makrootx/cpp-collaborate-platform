#pragma once

#include "shared/helpers/env_keys.hpp"

#include <string>
#include <map>

class EnvProvider
{
protected:
    static std::map<std::string, std::string> env_values;
    static const std::map<std::string, std::string> env_default_values;

public:
    EnvProvider() = delete;

    static void init();
    static std::string get(const std::string &key);
};