#pragma once

#include <system_error>

namespace PiSubmarine::Drone::Server::Fake
{
    enum class ErrorCode
    {
        AlreadyStarted = 1,
        ControlBindFailed,
        TelemetryBindFailed,
        LeaseServerStartFailed,
        VideoSubscriptionServerStartFailed,
        StartupVideoSubscriptionFailed
    };

    [[nodiscard]] std::error_code make_error_code(ErrorCode errorCode) noexcept;
}

namespace std
{
    template<>
    struct is_error_code_enum<PiSubmarine::Drone::Server::Fake::ErrorCode> : true_type
    {
    };
}
