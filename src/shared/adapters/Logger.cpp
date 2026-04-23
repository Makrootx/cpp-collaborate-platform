#include "shared/adapters/Logger.hpp"

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include <spdlog/async.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include "shared/helpers/EnvProvider.hpp"

namespace
{

    spdlog::level::level_enum parse_log_level(const char *env_level)
    {
        if (env_level == nullptr)
            return spdlog::level::info;

        const std::string level(env_level);
        if (level == "trace")
            return spdlog::level::trace;
        if (level == "debug")
            return spdlog::level::debug;
        if (level == "info")
            return spdlog::level::info;
        if (level == "warn")
            return spdlog::level::warn;
        if (level == "error")
            return spdlog::level::err;
        if (level == "critical")
            return spdlog::level::critical;
        if (level == "off")
            return spdlog::level::off;

        return spdlog::level::info;
    }

}

namespace Logger
{

    void init()
    {
        spdlog::init_thread_pool(8192, 1);

        std::vector<spdlog::sink_ptr> sinks;

        sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());

        const std::string log_file = EnvProvider::get("LOG_FILE");
        if (!log_file.empty())
        {
            try
            {
                const std::filesystem::path log_path(log_file);
                if (log_path.has_parent_path())
                {
                    std::filesystem::create_directories(log_path.parent_path());
                }

                sinks.push_back(
                    std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
                        log_file,
                        1024UL * 1024UL * 10,
                        5));
            }
            catch (const std::exception &e)
            {
                fprintf(stderr,
                        "[Logger] File sink failed for '%s': %s"
                        " — falling back to console only\n",
                        log_file.c_str(), e.what());
            }
        }

        auto logger = std::make_shared<spdlog::async_logger>(
            "app",
            sinks.begin(), sinks.end(),
            spdlog::thread_pool(),
            spdlog::async_overflow_policy::block);

        const std::string log_level = EnvProvider::get("LOG_LEVEL");
        const auto level = parse_log_level(log_level.c_str());
        logger->set_level(level);

        logger->flush_on(spdlog::level::trace);

        spdlog::set_default_logger(logger);
        spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [tid %t] %v");
        spdlog::flush_every(std::chrono::seconds(1));

        spdlog::info("Logger initialized — level={} async=true",
                     spdlog::level::to_string_view(level));
    }

    void shutdown()
    {
        spdlog::info("Logger shutting down");
        spdlog::apply_all([](const std::shared_ptr<spdlog::logger> &l)
                          { l->flush(); });
        spdlog::shutdown();
    }

}