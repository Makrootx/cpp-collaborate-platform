#pragma once

#ifdef _HAS_AUTO_PTR
#define _HAS_AUTO_PTR 1
#endif

#include <memory>
#include <string>
#include <odb/pgsql/database.hxx>

/**
 * @brief Factory for configured PostgreSQL ODB database connections.
 */
class DatabaseManager
{

private:
    struct DatabaseEnvConfig
    {
        std::string user;
        std::string password;
        std::string db_name;
        std::string host;
        std::string port;
        std::string pool_max;
        std::string pool_min;
    };

    static DatabaseEnvConfig getenv_config();

public:
    /** @brief Creates and returns configured database instance/pool handle. */
    static std::shared_ptr<odb::pgsql::database> create_pool();
};