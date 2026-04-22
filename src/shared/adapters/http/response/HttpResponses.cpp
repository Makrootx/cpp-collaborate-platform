#include "shared/adapters/http/response/HttpResponses.hpp"

namespace HttpResponses
{
    static crow::response json_response(int code, std::string body)
    {
        crow::response res(code);
        res.set_header("Content-Type", "application/json");
        res.body = std::move(body);
        return res;
    }

    crow::response invalid_json()
    {
        return crow::response(400, "Invalid JSON");
    }

    crow::response invalid_payload(const std::vector<std::string> &errors)
    {
        std::string error_msg = "Invalid request payload. Errors: ";
        for (const auto &error : errors)
        {
            error_msg += error + "; ";
        }
        return crow::response(400, error_msg);
    }

    crow::response bad_request(const std::string &message)
    {
        return crow::response(400, message);
    }

    crow::response unauthorized(const std::string &message)
    {
        return crow::response(401, message);
    }

    crow::response forbidden(const std::string &message)
    {
        return crow::response(403, message);
    }

    crow::response not_found(const std::string &message)
    {
        return crow::response(404, message);
    }

    crow::response internal_server_error()
    {
        return crow::response(500, "Internal server error");
    }

    crow::response internal_server_error(const std::string &message)
    {
        return crow::response(500, message);
    }

    crow::response ok(const std::string &message)
    {
        return crow::response(200, message);
    }

    crow::response created(const std::string &message)
    {
        return crow::response(201, message);
    }

    crow::response json_ok(const std::string &body)
    {
        return json_response(200, body);
    }

    crow::response json_created(const std::string &body)
    {
        return json_response(201, body);
    }

    crow::response no_content()
    {
        return crow::response(204);
    }
}
