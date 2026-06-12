#pragma once

#include <memory>
#include <string_view>

#include <spdlog/sinks/sink.h>

#include "PiSubmarine/Logging/Api/IFactory.h"

namespace PiSubmarine::Drone::Server::Fake
{
    class LoggerFactory final : public Logging::Api::IFactory
    {
    public:
        LoggerFactory();

        [[nodiscard]] std::shared_ptr<spdlog::logger> CreateLogger(std::string_view name) override;

    private:
        std::shared_ptr<spdlog::sinks::sink> m_Sink;
    };
}
