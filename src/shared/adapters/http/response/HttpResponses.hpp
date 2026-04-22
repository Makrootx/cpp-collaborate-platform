#pragma once

#include "crow.h"

#include <string>
#include <vector>

namespace HttpResponses
{
    crow::response invalid_json();
    crow::response invalid_payload(const std::vector<std::string> &errors);
    crow::response bad_request(const std::string &message);
    crow::response unauthorized(const std::string &message = "Unauthorized");
    crow::response forbidden(const std::string &message = "Forbidden");
    crow::response not_found(const std::string &message);
    crow::response internal_server_error();
    crow::response internal_server_error(const std::string &message);
    crow::response ok(const std::string &message);
    crow::response created(const std::string &message);
    crow::response json_ok(const std::string &body);
    crow::response json_created(const std::string &body);
    crow::response no_content();
}
