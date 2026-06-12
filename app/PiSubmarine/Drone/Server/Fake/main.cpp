#include <cctype>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <limits>
#include <memory>
#include <optional>
#include <string>

#include <CLI/CLI.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stderr_color_sinks.h>

#include "PiSubmarine/Drone/Server/Fake/Runtime.h"
#include "PiSubmarine/Error/Api/Error.h"

namespace PiSubmarine::Drone::Server::Fake
{
    namespace
    {
        [[nodiscard]] std::filesystem::path NormalizePathForCurrentPlatform(const std::filesystem::path& path)
        {
#ifdef __linux__
            const auto raw = path.generic_string();
            if (raw.size() >= 3 &&
                std::isalpha(static_cast<unsigned char>(raw[0])) &&
                raw[1] == ':' &&
                raw[2] == '/')
            {
                std::string normalized = "/mnt/";
                normalized.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(raw[0]))));
                normalized.append(raw.substr(2));
                return normalized;
            }
#endif

            return path;
        }

        [[nodiscard]] std::optional<PiSubmarine::Udp::Api::Endpoint> ParseEndpoint(const std::string& value)
        {
            const auto separator = value.rfind(':');
            if (separator == std::string::npos || separator == 0 || separator == value.size() - 1)
            {
                return std::nullopt;
            }

            try
            {
                const auto port = std::stoul(value.substr(separator + 1));
                if (port > std::numeric_limits<std::uint16_t>::max())
                {
                    return std::nullopt;
                }

                return PiSubmarine::Udp::Api::Endpoint{
                    .Address = value.substr(0, separator),
                    .Port = static_cast<std::uint16_t>(port)};
            }
            catch (...)
            {
                return std::nullopt;
            }
        }

        [[nodiscard]] std::string FormatEndpoint(const PiSubmarine::Udp::Api::Endpoint& endpoint)
        {
            return endpoint.Address + ":" + std::to_string(endpoint.Port);
        }

        [[nodiscard]] std::string ReadTextFile(const std::filesystem::path& path)
        {
            const auto normalizedPath = NormalizePathForCurrentPlatform(path);
            std::ifstream stream(normalizedPath, std::ios::binary);
            if (!stream.is_open())
            {
                throw std::runtime_error("Failed to open file: " + normalizedPath.string());
            }

            return std::string(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>());
        }

        [[nodiscard]] std::shared_ptr<spdlog::logger> CreateLogger()
        {
            auto logger = std::make_shared<spdlog::logger>(
                "Drone.Server.Fake.App",
                std::make_shared<spdlog::sinks::stderr_color_sink_mt>());
            logger->set_pattern("[%Y-%m-%d %T.%e] [%-20n] [%^%-8l%$] [%s:%#] %v");
            return logger;
        }

        [[nodiscard]] const char* ToString(const Error::Api::ErrorCondition condition) noexcept
        {
            switch (condition)
            {
            case Error::Api::ErrorCondition::ContractError:
                return "ContractError";
            case Error::Api::ErrorCondition::CommunicationError:
                return "CommunicationError";
            case Error::Api::ErrorCondition::DeviceError:
                return "DeviceError";
            case Error::Api::ErrorCondition::UnknownError:
                return "UnknownError";
            }

            return "UnknownError";
        }

        [[nodiscard]] std::string ToString(const Error::Api::Error& error)
        {
            std::string text = ToString(error.Condition);
            if (error.HasCause())
            {
                text += " (";
                text += error.Cause.message();
                text += ")";
            }

            return text;
        }
    }
}

int main(const int argc, char** argv)
{
    using namespace PiSubmarine::Drone::Server::Fake;
    const auto logger = CreateLogger();

    try
    {
        Runtime::Config config;
        std::filesystem::path serverCertificatePath;
        std::filesystem::path serverPrivateKeyPath;
        std::filesystem::path clientCertificateAuthorityPath;
        auto controlAddress = FormatEndpoint(config.ControlEndpoint);
        auto telemetryAddress = FormatEndpoint(config.TelemetryEndpoint);

        CLI::App app{"PiSubmarine fake drone server"};
        app.add_option("--lease-address", config.LeaseServer.Address, "Lease gRPC bind address")
            ->default_val("0.0.0.0:50051");
        app.add_option("--server-cert", serverCertificatePath, "PEM server certificate chain file")
            ->required();
        app.add_option("--server-key", serverPrivateKeyPath, "PEM server private key file")
            ->required();
        app.add_option("--client-ca", clientCertificateAuthorityPath, "PEM client certificate authority file")
            ->required();
        app.add_option("--control-address", controlAddress, "Control UDP bind address")
            ->default_val(controlAddress);
        app.add_option("--telemetry-address", telemetryAddress, "Telemetry UDP bind address")
            ->default_val(telemetryAddress);

        auto tickPeriodMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(config.TickPeriod).count();
        app.add_option("--tick-period-ms", tickPeriodMilliseconds, "Tick period in milliseconds")
            ->default_val(tickPeriodMilliseconds);
        app.add_option(
                "--receive-queue-capacity",
                config.ReceiveQueueCapacity,
                "Per-socket receive queue capacity")
            ->default_val(config.ReceiveQueueCapacity);
        app.add_option(
                "--max-datagram-size",
                config.MaxDatagramSize,
                "Maximum UDP datagram size in bytes")
            ->default_val(config.MaxDatagramSize);

        CLI11_PARSE(app, argc, argv);

        const auto parsedControlEndpoint = ParseEndpoint(controlAddress);
        if (!parsedControlEndpoint.has_value())
        {
            SPDLOG_LOGGER_ERROR(logger, "Invalid --control-address value. Expected host:port.");
            return 2;
        }

        const auto parsedTelemetryEndpoint = ParseEndpoint(telemetryAddress);
        if (!parsedTelemetryEndpoint.has_value())
        {
            SPDLOG_LOGGER_ERROR(logger, "Invalid --telemetry-address value. Expected host:port.");
            return 2;
        }

        config.ControlEndpoint = *parsedControlEndpoint;
        config.TelemetryEndpoint = *parsedTelemetryEndpoint;
        config.TickPeriod = std::chrono::milliseconds(tickPeriodMilliseconds);
        config.LeaseServer.ServerCertificateChain = ReadTextFile(serverCertificatePath);
        config.LeaseServer.ServerPrivateKey = ReadTextFile(serverPrivateKeyPath);
        config.LeaseServer.ClientCertificateAuthority = ReadTextFile(clientCertificateAuthorityPath);

        Runtime runtime(config);
        const auto runResult = runtime.Run();
        if (!runResult.has_value())
        {
            SPDLOG_LOGGER_ERROR(logger, "{}", ToString(runResult.error()));
            return 1;
        }

        return 0;
    }
    catch (const std::exception& exception)
    {
        SPDLOG_LOGGER_ERROR(logger, "{}", exception.what());
        return 1;
    }
}
