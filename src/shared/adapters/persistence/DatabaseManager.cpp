#include "shared/adapters/persistence/DatabaseManager.hpp"

#include <cstdlib>
#include <exception>
#include <memory>
#include <string>

#ifdef _WIN32
#define setenv(name, value, overwrite) _putenv_s(name, value)
#endif
#include <dotenv/dotenv.hpp>
#include <spdlog/spdlog.h>
#include <odb/pgsql/connection-factory.hxx>

#include <map>
// #include "shared/helpers/env_keys.hpp"
#include "shared/helpers/EnvProvider.hpp"

DatabaseManager::DatabaseEnvConfig DatabaseManager::getenv_config()
{
    DatabaseEnvConfig config{EnvProvider::get(Env::DB_USER),
                             EnvProvider::get(Env::DB_PASS),
                             EnvProvider::get(Env::DB_NAME),
                             EnvProvider::get(Env::DB_HOST),
                             EnvProvider::get(Env::DB_PORT),
                             EnvProvider::get(Env::DB_POOL_MAX),
                             EnvProvider::get(Env::DB_POOL_MIN)};
    return config;
}

std::shared_ptr<odb::pgsql::database> DatabaseManager::create_pool()
{

    const DatabaseEnvConfig config = getenv_config();

    const auto user = config.user;
    const auto password = config.password;
    const auto db_name = config.db_name;
    const auto host = config.host;
    const auto port_str = config.port;
    const auto pool_max_str = config.pool_max;
    const auto pool_min_str = config.pool_min;

    int port;
    try
    {
        port = std::stoi(port_str);
    }
    catch (...)
    {
        auto error_msg = fmt::format("Invalid DB_PORT value '{}'.", port_str);
        throw std::runtime_error(fmt::format("Invalid environment variable. {}", error_msg));
    }

    std::size_t pool_max;
    try
    {
        pool_max = static_cast<std::size_t>(std::stoul(pool_max_str));
    }
    catch (...)
    {
        auto error_msg = fmt::format("Invalid DB_POOL_MAX value '{}'.", pool_max_str);
        throw std::runtime_error(fmt::format("Invalid environment variable. {}", error_msg));
    }

    std::size_t pool_min;
    try
    {
        pool_min = static_cast<std::size_t>(std::stoul(pool_min_str));
    }
    catch (...)
    {
        auto error_msg = fmt::format("Invalid DB_POOL_MIN value '{}'.", pool_min_str);
        throw std::runtime_error(fmt::format("Invalid environment variable. {}", error_msg));
    }

    if (pool_max != 0 && pool_min > pool_max)
    {
        spdlog::warn("DB_POOL_MIN value - {} is bigger than DB_POOL_MAX - {} value. Setting DB_POOL_MIN value to DB_POOL_MAX value.", pool_min, pool_max);
        pool_min = pool_max;
    }

    try
    {
        spdlog::info(
            "Connecting to PostgreSQL at {}:{} db={} user={} pool(max={}, min={})",
            host,
            port,
            db_name,
            user,
            pool_max,
            pool_min);

        std::unique_ptr<odb::pgsql::connection_factory> pool_factory(
            new odb::pgsql::connection_pool_factory(pool_max, pool_min));

        auto db = std::make_shared<odb::pgsql::database>(
            user,
            password,
            db_name,
            host,
            static_cast<unsigned int>(port),
            "",
            std::move(pool_factory));

        {
            odb::transaction tx(db->begin());
            db->execute("SET search_path = public");
            tx.commit();
        }

        spdlog::info("PostgreSQL connection initialized");
        return db;
    }
    catch (const std::exception &e)
    {
        spdlog::critical("Failed to initialize PostgreSQL connection: {}", e.what());
        throw;
    }
}