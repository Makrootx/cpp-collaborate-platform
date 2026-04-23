#pragma once

#include "crow.h"

#include <concepts>
#include <initializer_list>
#include <optional>
#include <ranges>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

#include "shared/helpers/json/JsonTypeHelper.hpp"
#include "shared/helpers/json/JsonFieldBuilder.hpp"

template <typename Dto>
class JsonDto;

template <typename T>
struct IsVectorOfJsonDto : std::false_type
{
};

template <typename T, typename Alloc>
struct IsVectorOfJsonDto<std::vector<T, Alloc>> : std::bool_constant<JsonDtoDerived<T>>
{
    using value_type = T;
};

template <typename T>
concept VectorOfJsonDto = IsVectorOfJsonDto<T>::value;

template <typename T>
concept NestedDto = JsonDtoDerived<T> || VectorOfJsonDto<T>;

template <typename Dto>
concept HasDtoFields = requires {
    { Dto::fields() } -> std::ranges::input_range;
} && std::convertible_to<std::ranges::range_value_t<decltype(Dto::fields())>, JsonFieldDescriptor<Dto>>;

/// @brief CRTP base providing JSON serialization, deserialization, and validation for data transfer objects.
template <typename Dto>
class JsonDto
{
public:
    using FieldDescriptor = JsonFieldDescriptor<Dto>;
    using FieldBuilder = JsonFieldBuilder<Dto>;

    /// @brief Deserializes JSON into an optional Dto; returns nullopt when validation fails.
    static std::optional<Dto>
    from_json(const crow::json::rvalue &json)
        requires HasDtoFields<Dto>;

    /// @brief Serializes a Dto to a JSON write-value using declared field descriptors.
    static crow::json::wvalue to_json(const Dto &dto)
        requires HasDtoFields<Dto>;

    crow::json::wvalue to_json() const
        requires HasDtoFields<Dto>;

    /// @brief Validates a JSON value against the Dto's field descriptors; returns validity flag and collected error messages.
    static std::tuple<bool, std::vector<std::string>> validate(const crow::json::rvalue &json)
        requires HasDtoFields<Dto>;

protected:
    template <NestedDto Nested>
    static std::tuple<bool, std::vector<std::string>> validate_nested(const crow::json::rvalue &val);

    template <HasJsonTypeHelper T>
    static FieldBuilder field(const char *key, T Dto::*member);

    template <HasJsonTypeHelper T>
    static FieldBuilder field_secured(const char *key, std::optional<T> Dto::*member);

    template <HasJsonTypeHelper T>
    static FieldBuilder optional_field(const char *key, std::optional<T> Dto::*member);

    template <HasJsonTypeHelper T>
    static FieldBuilder optional_field_secured(const char *key, std::optional<std::optional<T>> Dto::*member);

    template <NestedDto Nested>
    static FieldBuilder nested_field(const char *key, Nested Dto::*member);

    template <NestedDto Nested>
    static FieldBuilder nested_field_secured(const char *key, std::optional<Nested> Dto::*member);

    template <NestedDto Nested>
    static FieldBuilder optional_nested_field(const char *key, std::optional<Nested> Dto::*member);

    template <NestedDto Nested>
    static FieldBuilder optional_nested_field_secured(const char *key, std::optional<std::optional<Nested>> Dto::*member);
};

#include "JsonDto.tpp"