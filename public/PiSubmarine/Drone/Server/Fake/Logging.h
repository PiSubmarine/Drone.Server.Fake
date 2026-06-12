#pragma once

#include <memory>
#include <string_view>

#include <spdlog/logger.h>

namespace PiSubmarine::Drone::Server::Fake
{
    [[nodiscard]] std::shared_ptr<spdlog::logger> CreateConfiguredLogger(std::string_view name);
}
