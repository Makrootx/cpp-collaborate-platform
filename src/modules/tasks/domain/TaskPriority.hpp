#pragma once

enum TaskPriority
{
    Low,
    Medium,
    High
};

namespace TaskPriorityUtils
{
    inline std::string to_string(TaskPriority p)
    {
        switch (p)
        {
        case TaskPriority::Low:
            return "Low";
        case TaskPriority::Medium:
            return "Medium";
        case TaskPriority::High:
            return "High";
        default:
            return "Unknown";
        }
    }

    inline TaskPriority from_string(const std::string &str)
    {
        if (str == "Low")
            return TaskPriority::Low;
        if (str == "Medium")
            return TaskPriority::Medium;
        if (str == "High")
            return TaskPriority::High;

        throw std::invalid_argument("Invalid TaskPriority string: " + str);
    }
}