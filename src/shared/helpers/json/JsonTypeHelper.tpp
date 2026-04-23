#include "shared/helpers/json/JsonTypeHelper.hpp"

template <typename T>
    requires JsonDtoDerived<T>
std::map<std::string, T> JsonTypeHelper<std::map<std::string, T>>::read(const crow::json::rvalue &value)
{
    std::map<std::string, T> result;
    for (const auto &item : value)
    {
        if (auto parsed = T::from_json(item))
            result[std::string(item.key())] = std::move(*parsed);
    }
    return result;
}

template <typename T>
    requires JsonDtoDerived<T>
crow::json::wvalue JsonTypeHelper<std::map<std::string, T>>::write(const std::map<std::string, T> &map)
{
    crow::json::wvalue out;
    for (const auto &[key, val] : map)
        out[key] = val.to_json();
    return out;
}

inline std::tm JsonTypeHelper<std::tm>::read(const crow::json::rvalue &value)
{
    std::tm t = TmStringUtils::from_string(value.s(), "%Y-%m-%dT%H:%M:%S");

    if (std::mktime(&t) == -1)
    {
        throw std::runtime_error(std::string("Failed to parse date string: ").append(value.s()));
    }

    return t;
}

template <typename T>
crow::json::wvalue JsonWriter<T>::write(const T &v)
{
    if constexpr (std::is_integral_v<T> && !std::is_same_v<T, bool>)
    {
        return crow::json::wvalue(static_cast<int64_t>(v));
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
        return crow::json::wvalue(static_cast<double>(v));
    }
    else if constexpr (std::is_same_v<std::decay_t<T>, std::tm>)
    {
        return crow::json::wvalue(TmStringUtils::to_string(v, "%Y-%m-%dT%H:%M:%S"));
    }
    else if constexpr (std::is_same_v<std::decay_t<T>, TaskPriority>)
    {
        return crow::json::wvalue(TaskPriorityUtils::to_string(v));
    }
    else
    {
        return crow::json::wvalue(v);
    }
}

template <typename T>
    requires JsonDtoDerived<T>
T JsonTypeHelper<T>::read(const crow::json::rvalue &v)
{
    auto result = T::from_json(v);
    return result ? std::move(*result) : T();
}

template <typename T>
    requires JsonDtoDerived<T>
crow::json::wvalue JsonTypeHelper<T>::write(const T &v)
{
    return v.to_json();
}

template <typename T>
    requires HasJsonTypeHelper<T>
std::vector<T> JsonTypeHelper<std::vector<T>>::read(const crow::json::rvalue &value)
{
    std::vector<T> result;
    const std::size_t count = value.size();
    result.reserve(count);

    for (std::size_t i = 0; i < count; ++i)
    {
        if (json_type_matches(value[i].t(), JsonTypeHelper<T>::types))
        {
            result.push_back(JsonTypeHelper<T>::read(value[i]));
        }
    }
    return result;
}

template <typename T>
    requires HasJsonTypeHelper<T>
crow::json::wvalue JsonTypeHelper<std::vector<T>>::write(const std::vector<T> &v)
{
    std::vector<crow::json::wvalue> list;
    list.reserve(v.size());
    for (const auto &item : v)
    {
        list.push_back(JsonTypeHelper<T>::write(item));
    }
    return crow::json::wvalue(list);
}
