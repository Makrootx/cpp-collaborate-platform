#pragma once

#include "shared/adapters/JwtAuthenticatorService.hpp"
#include "shared/adapters/http/response/HttpResponses.hpp"

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
                       context &ctx);

    void after_handle(crow::request &, crow::response &, context &) {}
};
