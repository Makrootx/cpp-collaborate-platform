#include "shared/helpers/json/JsonFieldBuilder.hpp"

template <typename Dto>
template <typename OptionalMember>
JsonFieldBuilder<Dto> &JsonFieldBuilder<Dto>::with_secure_serializer(OptionalMember Dto::*member)
{
    const auto prev_assign = descriptor.assign_func;
    const auto prev_serialize = descriptor.serialize_func;
    const auto key = descriptor.key;

    descriptor.assign_func = [member, prev_assign](Dto &dto, const crow::json::rvalue &val)
    {
        if ((val.t() == crow::json::type::Object && val.has("error")))
        {
            dto.*member = std::nullopt;
        }
        else
        {
            prev_assign(dto, val);
        }
    };

    descriptor.serialize_func = [member, prev_serialize, key](const Dto &dto, crow::json::wvalue &out)
    {
        if (!(dto.*member).has_value())
        {
            crow::json::wvalue error_obj;
            error_obj["error"] = "Access denied";
            out[key] = std::move(error_obj);
        }
        else
        {
            prev_serialize(dto, out);
        }
    };

    return *this;
}