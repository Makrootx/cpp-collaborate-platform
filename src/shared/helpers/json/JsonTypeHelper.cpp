#include "shared/helpers/json/JsonTypeHelper.hpp"

bool json_type_matches(crow::json::type actual, std::span<const crow::json::type> expected)
{
    for (const auto t : expected)
    {
        if (t == actual)
            return true;
    }
    return false;
}