#pragma once

#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

namespace TmStringUtils
{
    inline std::string to_string(const std::tm &time, const char *format = "%Y-%m-%d %H:%M:%S")
    {
        std::ostringstream oss;
        oss << std::put_time(&time, format);
        return oss.str();
    }

    inline std::tm from_string(const std::string &value, const char *format = "%Y-%m-%d %H:%M:%S")
    {
        std::tm tm = {};
        std::istringstream iss(value);
        iss >> std::get_time(&tm, format);
        return tm;
    }
}
