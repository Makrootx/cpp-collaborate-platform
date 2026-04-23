#pragma once

#include "crow.h"

#include <cstddef>
#include <concepts>
#include <map>
#include <span>
#include <string>
#include <vector>

#include "modules/tasks/domain/TaskPriority.hpp"
#include "shared/helpers/TmStringUtils.hpp"

template <typename Dto>
class JsonDto;
/// @brief Satisfied when T is a concrete type derived from JsonDto<T>.
template <typename T>
concept JsonDtoDerived = std::is_base_of_v<JsonDto<T>, T>;

template <typename T>
struct JsonTypeHelper;

bool json_type_matches(crow::json::type actual, std::span<const crow::json::type> expected);

template <std::size_t N>
inline bool json_type_matches(crow::json::type actual, const crow::json::type (&expected)[N])
{
    return json_type_matches(actual, std::span<const crow::json::type>(expected));
}

/// @brief Satisfied when T has a specialised JsonTypeHelper providing JSON type list, read, and write.
template <typename T>
concept HasJsonTypeHelper = requires(const crow::json::rvalue &rv, const T &value) {
    { json_type_matches(rv.t(), JsonTypeHelper<T>::types) } -> std::convertible_to<bool>;
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
    static inline constexpr crow::json::type types[] = {crow::json::type::Object};

    static T read(const crow::json::rvalue &v);

    static crow::json::wvalue write(const T &v);
};

template <typename T>
    requires JsonDtoDerived<T>
struct JsonTypeHelper<std::map<std::string, T>>
{
    static inline constexpr crow::json::type types[] = {crow::json::type::Object};

    static std::map<std::string, T> read(const crow::json::rvalue &value);

    static crow::json::wvalue write(const std::map<std::string, T> &map);
};

template <>
struct JsonTypeHelper<std::string> : JsonWriter<std::string>
{
    static inline constexpr crow::json::type types[] = {crow::json::type::String};

    static std::string read(const crow::json::rvalue &value)
    {
        return value.s();
    }
};

template <>
struct JsonTypeHelper<long int> : JsonWriter<long int>
{
    static inline constexpr crow::json::type types[] = {crow::json::type::Number};

    static long int read(const crow::json::rvalue &value)
    {
        return value.i();
    }
};

template <>
struct JsonTypeHelper<bool> : JsonWriter<bool>
{
    static inline constexpr crow::json::type types[] = {crow::json::type::True, crow::json::type::False};

    static bool read(const crow::json::rvalue &value)
    {
        return value.b();
    }
};

template <>
struct JsonTypeHelper<std::tm> : JsonWriter<std::tm>
{
    static inline constexpr crow::json::type types[] = {crow::json::type::String};

    static std::tm read(const crow::json::rvalue &value);
};

template <>
struct JsonTypeHelper<TaskPriority> : JsonWriter<TaskPriority>
{
    static inline constexpr crow::json::type types[] = {crow::json::type::String};

    static TaskPriority read(const crow::json::rvalue &value)
    {
        return TaskPriorityUtils::from_string(value.s());
    }
};

template <typename T>
    requires HasJsonTypeHelper<T>
struct JsonTypeHelper<std::vector<T>>
{
    static inline constexpr crow::json::type types[] = {crow::json::type::List};

    static std::vector<T> read(const crow::json::rvalue &value);

    static crow::json::wvalue write(const std::vector<T> &v);
};

#include "JsonTypeHelper.tpp"
