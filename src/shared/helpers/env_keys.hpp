#pragma once

namespace Env
{
    inline constexpr const char *DB_USER = "DB_USER";
    inline constexpr const char *DB_HOST = "DB_HOST";
    inline constexpr const char *DB_PORT = "DB_PORT";
    inline constexpr const char *DB_PASS = "DB_PASSWORD";
    inline constexpr const char *DB_NAME = "DB_NAME";
    inline constexpr const char *DB_POOL_MAX = "DB_POOL_MAX";
    inline constexpr const char *DB_POOL_MIN = "DB_POOL_MIN";
    inline constexpr const char *HTTP_HOST = "HTTP_HOST";
    inline constexpr const char *HTTP_PORT = "HTTP_PORT";
    inline constexpr const char *JWT_KEY = "JWT_SECRET_KEY";
    inline constexpr const char *LOG_LEVEL = "LOG_LEVEL";
    inline constexpr const char *LOG_FILE = "LOG_FILE";
    inline constexpr const char *JWT_TOKEN_EXPIRY = "JWT_TOKEN_EXPIRY";

    inline constexpr const char *ALL[] = {
        DB_USER, DB_HOST, DB_PORT, DB_PASS, DB_NAME,
        DB_POOL_MAX, DB_POOL_MIN, HTTP_HOST, HTTP_PORT, JWT_KEY, LOG_LEVEL, LOG_FILE, JWT_TOKEN_EXPIRY};
}