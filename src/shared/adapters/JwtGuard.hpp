#pragma once

#include "shared/adapters/JwtAuthenticatorService.hpp"

#include "crow.h"
#include <utility>

struct JwtMiddleware : public crow::ILocalMiddleware
{
    /** @brief Request-scoped authentication context populated from JWT. */
    struct context
    {
        std::optional<std::string> user_id;
    };

    /** @brief Rejects request when JWT header is missing or invalid. */
    void before_handle(crow::request &req,
                       crow::response &res,
                       context &ctx)
    {
        const auto auth = req.get_header_value("Authorization");
        if (auth.size() >= 8 && auth.rfind("Bearer ", 0) == 0)
        {
            auto user_id = JwtAuthenticatorService::validate_Jwt(auth.substr(7));
            if (user_id.has_value())
            {
                ctx.user_id = std::move(user_id.value());
            }
            else
            {
                res.code = 401;
                res.end();
                return;
            }
        }
        else
        {
            res.code = 401;
            res.end();
            return;
        }
    }

    void after_handle(crow::request &, crow::response &, context &) {}
};