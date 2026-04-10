#include "shared/helpers/json/JsonDto.hpp"

template <typename Dto>
std::optional<Dto> JsonDto<Dto>::from_json(const crow::json::rvalue &json)
    requires HasDtoFields<Dto>
{
    auto [valid, _] = validate(json);
    if (!valid)
        return std::nullopt;

    Dto dto;
    for (const auto &desc : Dto::fields())
    {
        if (json.has(desc.key))
            desc.assign_func(dto, json[desc.key]);
    }
    return dto;
}

template <typename Dto>
crow::json::wvalue JsonDto<Dto>::to_json(const Dto &dto)
    requires HasDtoFields<Dto>
{
    crow::json::wvalue out;

    for (const auto &desc : Dto::fields())
    {
        desc.serialize_func(dto, out);
    }
    return out;
}

template <typename Dto>
crow::json::wvalue JsonDto<Dto>::to_json() const
    requires HasDtoFields<Dto>
{
    crow::json::wvalue out;
    const Dto &self = static_cast<const Dto &>(*this);
    for (const auto &desc : Dto::fields())
    {
        desc.serialize_func(self, out);
    }
    return out;
}

template <typename Dto>
std::tuple<bool, std::vector<std::string>> JsonDto<Dto>::validate(const crow::json::rvalue &json)
    requires HasDtoFields<Dto>
{
    bool valid = true;
    std::vector<std::string> errors;

    for (const auto &desc : Dto::fields())
    {
        if (!json.has(desc.key))
        {
            if (desc.required)
            {
                valid = false;
                errors.push_back("Missing required field: " + std::string(desc.key));
            }
            continue;
        }

        const auto &val = json[desc.key];

        if (val.t() != desc.expected_type)
        {
            valid = false;
            errors.push_back("Invalid type for field: " + std::string(desc.key));
            continue;
        }

        if (desc.nested_validator)
        {
            auto [nested_valid, nested_errors] = desc.nested_validator(val);
            if (!nested_valid)
            {
                valid = false;
                for (const auto &e : nested_errors)
                    errors.push_back(std::string(desc.key) + "." + e);
            }
        }

        for (const auto &[constraint, message] : desc.constraints)
        {
            if (!constraint(val))
            {
                valid = false;
                errors.push_back("Field '" + std::string(desc.key) + "': " + message);
            }
        }
    }

    return {valid, errors};
}

template <typename Dto>
template <NestedDto Nested>
std::tuple<bool, std::vector<std::string>> JsonDto<Dto>::validate_nested(const crow::json::rvalue &val)
{
    if constexpr (JsonDtoDerived<Nested>)
    {
        return Nested::validate(val);
    }
    else if constexpr (VectorOfJsonDto<Nested>)
    {
        using Item = typename IsVectorOfJsonDto<Nested>::value_type;
        bool valid = true;
        std::vector<std::string> errors;

        if (val.t() != crow::json::type::List)
        {
            return {false, {"Expected list"}};
        }

        const std::size_t count = val.size();
        for (std::size_t i = 0; i < count; ++i)
        {
            const auto &item = val[i];
            if (item.t() != crow::json::type::Object)
            {
                valid = false;
                errors.push_back("[" + std::to_string(i) + "]: Expected object");
                continue;
            }

            auto [nested_valid, nested_errors] = Item::validate(item);
            if (!nested_valid)
            {
                valid = false;
                for (const auto &e : nested_errors)
                {
                    errors.push_back("[" + std::to_string(i) + "]." + e);
                }
            }
        }

        return {valid, errors};
    }
}

template <typename Dto>
template <HasJsonTypeHelper T>
JsonDto<Dto>::FieldBuilder JsonDto<Dto>::field(const char *key, T Dto::*member)
{
    return {.descriptor = {
                .key = key,
                .expected_type = JsonTypeHelper<T>::type,
                .required = true,
                .assign_func = [member](Dto &dto, const crow::json::rvalue &val)
                { dto.*member = JsonTypeHelper<T>::read(val); },
                .serialize_func = [key, member](const Dto &dto, crow::json::wvalue &out) // Added 'key' to capture
                { out[key] = JsonTypeHelper<T>::write(dto.*member); }}};
}

template <typename Dto>
template <HasJsonTypeHelper T>
JsonDto<Dto>::FieldBuilder JsonDto<Dto>::optional_field(const char *key, std::optional<T> Dto::*member)
{
    return {.descriptor = {
                .key = key,
                .expected_type = JsonTypeHelper<T>::type,
                .required = false,
                .assign_func = [member](Dto &dto, const crow::json::rvalue &val)
                { dto.*member = JsonTypeHelper<T>::read(val); },
                .serialize_func = [key, member](const Dto &dto, crow::json::wvalue &out)
                {
                        if ((dto.*member).has_value()) {
                            out[key] = JsonTypeHelper<T>::write(*(dto.*member));
                        } }}};
}

template <typename Dto>
template <NestedDto Nested>
JsonDto<Dto>::FieldBuilder JsonDto<Dto>::nested_field(const char *key, Nested Dto::*member)
{
    return {.descriptor = {
                .key = key,
                .expected_type = JsonTypeHelper<Nested>::type,
                .required = true,
                .assign_func = [member](Dto &dto, const crow::json::rvalue &val)
                { dto.*member = JsonTypeHelper<Nested>::read(val); },
                .serialize_func = [key, member](const Dto &d, crow::json::wvalue &out)
                { out[key] = JsonTypeHelper<Nested>::write(d.*member); },
                .constraints = {},
                .nested_validator = [](const crow::json::rvalue &val)
                { return validate_nested<Nested>(val); }}};
}

template <typename Dto>
template <NestedDto Nested>
JsonDto<Dto>::FieldBuilder JsonDto<Dto>::optional_nested_field(const char *key, std::optional<Nested> Dto::*member)
{
    return {.descriptor = {
                .key = key,
                .expected_type = JsonTypeHelper<Nested>::type,
                .required = false,
                .assign_func = [member](Dto &dto, const crow::json::rvalue &val)
                { dto.*member = JsonTypeHelper<Nested>::read(val); },
                .serialize_func = [key, member](const Dto &d, crow::json::wvalue &out)
                {
                    if ((d.*member).has_value()) {
                        out[key] = JsonTypeHelper<Nested>::write(*(d.*member));
                    } },
                .constraints = {},
                .nested_validator = [](const crow::json::rvalue &val)
                { return validate_nested<Nested>(val); }}};
}