#pragma once

/// @brief Application-level logging lifecycle management backed by spdlog.
namespace Logger
{
    /// @brief Initialises the async logger with console and optional rotating file sinks based on environment configuration.
    void init();
    /// @brief Flushes all pending log entries and shuts down the spdlog thread pool.
    void shutdown();
}
