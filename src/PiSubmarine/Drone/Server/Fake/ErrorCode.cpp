#include "PiSubmarine/Drone/Server/Fake/ErrorCode.h"

#include <array>
#include <string_view>

namespace PiSubmarine::Drone::Server::Fake
{
    namespace
    {
        class ErrorCategory final : public std::error_category
        {
        public:
            [[nodiscard]] const char* name() const noexcept override
            {
                return "PiSubmarine.Drone.Server.Fake";
            }

            [[nodiscard]] std::string message(const int condition) const override
            {
                constexpr std::array<std::string_view, 5> Messages{
                    "success",
                    "server already started",
                    "failed to bind control UDP endpoint",
                    "failed to bind telemetry UDP endpoint",
                    "failed to start lease gRPC server"};

                const auto index = static_cast<std::size_t>(condition);
                if (index >= Messages.size())
                {
                    return "unknown fake drone server error";
                }

                return std::string(Messages[index]);
            }
        };
    }

    [[nodiscard]] std::error_code make_error_code(const ErrorCode errorCode) noexcept
    {
        static const ErrorCategory Category;
        return {static_cast<int>(errorCode), Category};
    }
}
