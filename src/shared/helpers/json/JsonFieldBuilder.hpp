#pragma once

#include "crow.h"

#include <functional>
#include <span>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

template <typename Dto>
struct JsonFieldDescriptor
{
    using JsonConstraintFunc = std::function<bool(const crow::json::rvalue &)>;
    using JsonSerializationFunc = std::function<void(const Dto &, crow::json::wvalue &)>;
    using JsonAssignFunc = std::function<void(Dto &, const crow::json::rvalue &)>;

    const char *key;
    std::span<const crow::json::type> expected_types;
    bool required = true;
    JsonAssignFunc assign_func;
    JsonSerializationFunc serialize_func;
    std::vector<std::pair<JsonConstraintFunc, std::string>> constraints;
    std::function<std::tuple<bool, std::vector<std::string>>(const crow::json::rvalue &)> nested_validator = nullptr;
};

template <typename Dto>
struct JsonFieldBuilder
{
public:
    using JsonConstraintFunc = typename JsonFieldDescriptor<Dto>::JsonConstraintFunc;
    JsonFieldDescriptor<Dto> descriptor;

    JsonFieldBuilder &with(JsonConstraintFunc constraint, const std::string &error_message)
    {
        descriptor.constraints.emplace_back(constraint, error_message);
        return *this;
    }

    template <typename OptionalMember>
    JsonFieldBuilder &with_secure_serializer(OptionalMember Dto::*member);

    JsonFieldDescriptor<Dto> build()
    {
        return descriptor;
    }

    operator JsonFieldDescriptor<Dto>()
    {
        return build();
    }
};

#include "JsonFieldBuilder.tpp"