#include "shared/adapters/Logger.hpp"

#include <cstdlib>
#include <memory>
#include <string>
#include <vector>

#include <spdlog/async.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

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

        if (const char *log_file = std::getenv("LOG_FILE");
            log_file != nullptr && log_file[0] != '\0')
        {
            try
            {
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
                        log_file, e.what());
            }
        }

        auto logger = std::make_shared<spdlog::async_logger>(
            "app",
            sinks.begin(), sinks.end(),
            spdlog::thread_pool(),
            spdlog::async_overflow_policy::block);

        const auto level = parse_log_level(std::getenv("LOG_LEVEL"));
        logger->set_level(level);

        logger->flush_on(spdlog::level::warn);

        spdlog::set_default_logger(logger);
        spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [tid %t] %v");

        spdlog::info("Logger initialized — level={} async=true",
                     spdlog::level::to_string_view(level));
    }

    void shutdown()
    {
        spdlog::info("Logger shutting down");
        spdlog::shutdown();
    }

}