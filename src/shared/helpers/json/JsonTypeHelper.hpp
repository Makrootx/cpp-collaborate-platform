#pragma once

#include "crow.h"

#include <concepts>
#include <vector>
#include <string>

template <typename Dto>
class JsonDto;
template <typename T>
concept JsonDtoDerived = std::is_base_of_v<JsonDto<T>, T>;

template <typename T>
struct JsonTypeHelper;

template <typename T>
concept HasJsonTypeHelper = requires(const crow::json::rvalue &rv, const T &value) {
    { JsonTypeHelper<T>::type } -> std::convertible_to<crow::json::type>;
    { JsonTypeHelper<T>::read(rv) } -> std::same_as<T>;
    { JsonTypeHelper<T>::write(value) } -> std::same_as<crow::json::wvalue>;
};

template <typename T>
struct JsonWriter
{
    static crow::json::wvalue write(const T &v);
};

template <typename T>
    requires JsonDtoDerived<T>
struct JsonTypeHelper<T>
{
    static constexpr crow::json::type type = crow::json::type::Object;

    static T read(const crow::json::rvalue &v);

    static crow::json::wvalue write(const T &v);
};

template <>
struct JsonTypeHelper<std::string> : JsonWriter<std::string>
{
    static constexpr crow::json::type type = crow::json::type::String;

    static std::string read(const crow::json::rvalue &value)
    {
        return value.s();
    }
};

template <>
struct JsonTypeHelper<long int> : JsonWriter<long int>
{
    static constexpr crow::json::type type = crow::json::type::Number;

    static long int read(const crow::json::rvalue &value)
    {
        return value.i();
    }
};

template <typename T>
    requires HasJsonTypeHelper<T>
struct JsonTypeHelper<std::vector<T>>
{
    static constexpr crow::json::type type = crow::json::type::List;

    static std::vector<T> read(const crow::json::rvalue &value);

    static crow::json::wvalue write(const std::vector<T> &v);
};

#include "JsonTypeHelper.tpp"