#include "PiSubmarine/Drone/Server/Fake/LoggerFactory.h"

#include <string>

#include <spdlog/logger.h>
#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace PiSubmarine::Drone::Server::Fake
{
    LoggerFactory::LoggerFactory()
        : m_Sink(std::make_shared<spdlog::sinks::stderr_color_sink_mt>())
    {
        m_Sink->set_pattern("[%Y-%m-%d %T.%e] [%-20n] [%^%-8l%$] [%s:%#] %v");
    }

    std::shared_ptr<spdlog::logger> LoggerFactory::CreateLogger(const std::string_view name)
    {
        return std::make_shared<spdlog::logger>(std::string(name), m_Sink);
    }
}
