#include "shared/adapters/JwtGuard.hpp"

void JwtMiddleware::before_handle(crow::request &req,
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
            res = HttpResponses::unauthorized();
            res.end();
            return;
        }
    }
    else
    {
        res = HttpResponses::unauthorized();
        res.end();
        return;
    }
}