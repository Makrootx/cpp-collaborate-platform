#include "shared/helpers/json/JsonTypeHelper.hpp"

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
        if (value[i].t() == JsonTypeHelper<T>::type)
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