#include "crow.h"
#include "shared/adapters/persistence/DatabaseManager.hpp"
#include "modules/spaces/adapters/SpaceRouteHandler.hpp"
#include "modules/spaces/adapters/SpaceService.hpp"
#include "modules/spaces/adapters/persistence/PgSpaceRepo.hpp"
#include "modules/users/adapters/UserRouteHandler.hpp"
#include "modules/users/adapters/UserService.hpp"
#include "modules/users/adapters/persistence/PgUserRepo.hpp"
#include "shared/adapters/JwtGuard.hpp"
#include "shared/adapters/PermissionGuard.hpp"
#include "shared/adapters/Logger.hpp"
#include "shared/adapters/SpaceGuard.hpp"
#include "modules/tasks/adapters/TaskRouteHandler.hpp"
#include "modules/groups/adapters/GroupRouteHandler.hpp"

#include "modules/automation/adapters/AutomationStaticService.hpp"
#include "modules/automation/adapters/persistance/AutomationDomainOdb.hpp"

#include "modules/groups/adapters/GroupModule.hpp"
#include "modules/automation/adapters/AutomationModule.hpp"
#include "modules/automation/adapters/AutomationRouteHandler.hpp"
#include "modules/boards/adapters/BoardModule.hpp"
#include "modules/boards/adapters/BoardRouteHandler.hpp"
#include "modules/boardCategories/adapters/BoardCategoryModule.hpp"
#include "modules/boardCategories/adapters/BoardCategoryRouteHandler.hpp"

#include <exception>
#include <string>
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

    auto groupModule = std::make_shared<GroupModule>(db, spaceRepo);
    auto taskModule = std::make_shared<TaskModule>(db, spaceRepo);
    auto automationModule = std::make_shared<AutomationModule>(db);

    auto automationRepo = std::make_shared<PgAutomationRepo<AutomationDomainOdb>>(db);

    AutomationStaticService::initialize(automationRepo);

    crow::App<JwtMiddleware, SpaceMiddleware, PermissionMiddleware> app{JwtMiddleware(), SpaceMiddleware(spaceRepo), PermissionMiddleware(groupModule)};

    UserRouteHandler userRouteHandler(userService);
    SpaceRouteHandler spaceRouteHandler(spaceService);
    TaskRouteHandler taskRouteHandler(taskModule);
    GroupRouteHandler groupRouteHandler(groupModule);
    AutomationRouteHandler automationRouteHandler(automationModule);
    auto boardModule = std::make_shared<BoardModule>(db);
    auto boardCategoryModule = std::make_shared<BoardCategoryModule>(db);
    BoardRouteHandler boardRouteHandler(boardModule);
    BoardCategoryRouteHandler boardCategoryRouteHandler(boardCategoryModule);
    userRouteHandler.setup(app);
    spaceRouteHandler.setup(app);
    taskRouteHandler.setup(app);
    groupRouteHandler.setup(app);
    automationRouteHandler.setup(app);
    boardRouteHandler.setup(app);
    boardCategoryRouteHandler.setup(app);

    const std::string http_host = EnvProvider::get(Env::HTTP_HOST);
    const std::string http_port = EnvProvider::get(Env::HTTP_PORT);

    spdlog::info("Starting HTTP server on {}:{}", http_host, http_port);

    const auto parsed_port = static_cast<std::uint16_t>(std::stoi(http_port));

    app.bindaddr(http_host)
        .port(parsed_port)
        .multithreaded()
        .run();
}