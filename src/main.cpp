#include "crow.h"
#include "shared/adapters/persistence/DatabaseManager.hpp"
#include "modules/spaces/adapters/SpaceRouteHandler.hpp"
#include "modules/spaces/adapters/SpaceService.hpp"
#include "modules/spaces/adapters/persistence/PgSpaceRepo.hpp"
#include "modules/users/adapters/UserRouteHandler.hpp"
#include "modules/users/adapters/UserService.hpp"
#include "modules/users/adapters/persistence/PgUserRepo.hpp"
#include "shared/adapters/JwtGuard.hpp"
#include "shared/adapters/Logger.hpp"
#include "shared/adapters/SpaceGuard.hpp"

#include <exception>
#include <spdlog/spdlog.h>
#include "shared/helpers/EnvProvider.hpp"

#ifdef _WIN32
#define setenv(name, value, overwrite) _putenv_s(name, value)
#endif
#include <dotenv/dotenv.hpp>

int main()
{
    std::set_terminate([]()
                       {
                           try
                           {
                               auto ex = std::current_exception();
                               if (ex)
                               {
                                   std::rethrow_exception(ex);
                               }
                               spdlog::critical("Fatal: terminate called without active exception");
                           }
                           catch (const std::exception &e)
                           {
                               spdlog::critical("Fatal unhandled exception: {}", e.what());
                           }
                           catch (...)
                           {
                               spdlog::critical("Fatal unknown unhandled exception");
                           }

                           Logger::shutdown();
                           std::abort(); });

    dotenv::load();
    EnvProvider::init();
    Logger::init();
    spdlog::info("Starting server...");
    auto db = DatabaseManager::create_pool();

    auto userRepo = std::make_shared<PgUserRepo<UserOdb>>(db);
    auto userService = std::make_shared<UserService>(userRepo);

    auto spaceRepo = std::make_shared<PgSpaceRepo<SpaceOdb>>(db);
    auto spaceService = std::make_shared<SpaceService>(spaceRepo, userRepo);
    crow::App<JwtMiddleware, SpaceMiddleware> app{JwtMiddleware(), SpaceMiddleware(spaceRepo)};

    UserRouteHandler userRouteHandler(userService);
    SpaceRouteHandler spaceRouteHandler(spaceService);

    userRouteHandler.setup(app);
    spaceRouteHandler.setup(app);

    app.port(8080)
        .multithreaded()
        .run();
}