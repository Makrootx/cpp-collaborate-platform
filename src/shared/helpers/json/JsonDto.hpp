#pragma once

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

template <typename Dto>
class JsonDto
{
public:
    using FieldDescriptor = JsonFieldDescriptor<Dto>;
    using FieldBuilder = JsonFieldBuilder<Dto>;

    static std::optional<Dto>
    from_json(const crow::json::rvalue &json)
        requires HasDtoFields<Dto>;

    static crow::json::wvalue to_json(const Dto &dto)
        requires HasDtoFields<Dto>;

    crow::json::wvalue to_json() const
        requires HasDtoFields<Dto>;

    static std::tuple<bool, std::vector<std::string>> validate(const crow::json::rvalue &json)
        requires HasDtoFields<Dto>;

protected:
    template <NestedDto Nested>
    static std::tuple<bool, std::vector<std::string>> validate_nested(const crow::json::rvalue &val);

    template <HasJsonTypeHelper T>
    static FieldBuilder field(const char *key, T Dto::*member);

    template <HasJsonTypeHelper T>
    static FieldBuilder optional_field(const char *key, std::optional<T> Dto::*member);

    template <NestedDto Nested>
    static FieldBuilder nested_field(const char *key, Nested Dto::*member);

    template <NestedDto Nested>
    static FieldBuilder optional_nested_field(const char *key, std::optional<Nested> Dto::*member);
};

#include "JsonDto.tpp"