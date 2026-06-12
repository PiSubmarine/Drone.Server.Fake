#include "PiSubmarine/Drone/Server/Fake/LoggerFactory.h"

#include "PiSubmarine/Drone/Server/Fake/Logging.h"

#include <format>
#include <mutex>
#include <string>

#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace PiSubmarine::Drone::Server::Fake
{
    namespace
    {
        constexpr std::size_t DefaultLoggerNameWidth = 32;

        std::mutex SharedLogFormattingMutex;
        std::size_t SharedLoggerNameWidth = DefaultLoggerNameWidth;
        std::weak_ptr<spdlog::sinks::sink> SharedSink;

        [[nodiscard]] std::string MakeLogPattern(const std::size_t loggerNameWidth)
        {
            return std::format("[%Y-%m-%d %T.%e] [%-{}n] [%^%-8l%$] [%s:%#] %v", loggerNameWidth);
        }

        [[nodiscard]] std::shared_ptr<spdlog::sinks::sink> GetSharedSink()
        {
            std::scoped_lock lock(SharedLogFormattingMutex);

            if (const auto sink = SharedSink.lock())
            {
                return sink;
            }

            auto sink = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();
            sink->set_pattern(MakeLogPattern(SharedLoggerNameWidth));
            SharedSink = sink;
            return sink;
        }

        void EnsureSinkPatternFitsName(
            const std::shared_ptr<spdlog::sinks::sink>& sink,
            const std::string_view name)
        {
            std::scoped_lock lock(SharedLogFormattingMutex);

            if (name.size() <= SharedLoggerNameWidth)
            {
                return;
            }

            SharedLoggerNameWidth = name.size();
            sink->set_pattern(MakeLogPattern(SharedLoggerNameWidth));
        }
    }

    std::shared_ptr<spdlog::logger> CreateConfiguredLogger(const std::string_view name)
    {
        const auto sink = GetSharedSink();
        EnsureSinkPatternFitsName(sink, name);
        return std::make_shared<spdlog::logger>(std::string(name), sink);
    }

    LoggerFactory::LoggerFactory() = default;

    std::shared_ptr<spdlog::logger> LoggerFactory::CreateLogger(const std::string_view name)
    {
        return CreateConfiguredLogger(name);
    }
}
