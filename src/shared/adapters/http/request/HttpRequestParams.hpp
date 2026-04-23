#pragma once

#include "crow.h"

#include <string>
#include <vector>

namespace HttpRequestParams
{
    /// @brief Parses a comma-separated query parameter into a vector of trimmed string tokens.
    inline std::vector<std::string> parse_csv_param(const crow::request &req, const char *param_name)
    {
        std::vector<std::string> result;
        const char *raw = req.url_params.get(param_name);
        if (!raw)
            return result;

        std::string current;
        for (char ch : std::string_view(raw))
        {
            if (ch == ',')
            {
                if (!current.empty())
                {
                    result.push_back(current);
                    current.clear();
                }
            }
            else if (ch != ' ')
            {
                current.push_back(ch);
            }
        }
        if (!current.empty())
            result.push_back(current);

        return result;
    }
}
