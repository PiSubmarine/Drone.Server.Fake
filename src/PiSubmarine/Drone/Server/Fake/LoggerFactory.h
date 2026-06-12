#pragma once

#include <string_view>

#include "PiSubmarine/Logging/Api/IFactory.h"

namespace PiSubmarine::Drone::Server::Fake
{
    class LoggerFactory final : public Logging::Api::IFactory
    {
    public:
        LoggerFactory();

        [[nodiscard]] std::shared_ptr<spdlog::logger> CreateLogger(std::string_view name) override;
    };
}
