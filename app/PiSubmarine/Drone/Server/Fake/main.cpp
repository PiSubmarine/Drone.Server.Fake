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
#include <spdlog/sinks/stdout_color_sinks.h>

#include "PiSubmarine/Ballast/Pid/Controller.h"
#include "PiSubmarine/Drone/Server/Runtime.h"
#include "PiSubmarine/Drone/Server/Fake/BallastActuator.h"
#include "PiSubmarine/Drone/Server/Fake/BallastProvider.h"
#include "PiSubmarine/Drone/Server/Fake/BatteryProvider.h"
#include "PiSubmarine/Drone/Server/Fake/BidirectionalMotor.h"
#include "PiSubmarine/Drone/Server/Fake/DepthProvider.h"
#include "PiSubmarine/Drone/Server/Fake/LampController.h"
#include "PiSubmarine/Drone/Server/Fake/Logging.h"
#include "PiSubmarine/Drone/Server/Fake/ProximityProvider.h"
#include "PiSubmarine/Drone/Server/Fake/ServoController.h"
#include "PiSubmarine/Drone/Server/Fake/UnidirectionalMotor.h"
#include "PiSubmarine/Drone/Server/Fake/VerticalSimulationEngine.h"
#include "PiSubmarine/Error/Api/Error.h"
#include "PiSubmarine/Video/Server/GStreamer/Source.h"
#include "PiSubmarine/Video/Subscription/Api/Endpoint.h"

namespace PiSubmarine::Drone::Server::Fake
{
    namespace
    {
        // FIXME Remove dirty WSL2 hack
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
            return CreateConfiguredLogger("Drone.Server.Fake.App");
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
    using Runtime = PiSubmarine::Drone::Server::Runtime;
    const auto logger = CreateLogger();

    try
    {
        Runtime::Config config;
        VerticalSimulationConfig simulationConfig;
        std::filesystem::path serverCertificatePath;
        std::filesystem::path serverPrivateKeyPath;
        std::filesystem::path clientCertificateAuthorityPath;
        auto controlAddress = FormatEndpoint(config.ControlEndpoint);
        auto telemetryAddress = FormatEndpoint(config.TelemetryEndpoint);
        std::string grpcAddress = "0.0.0.0:50051";
        double simulationDroneMassKilograms = simulationConfig.DroneMassKilograms;
        double simulationFrictionCoefficient = simulationConfig.FrictionCoefficient;
        double simulationBallastMaximumMassGrams = simulationConfig.BallastMaximumMassKilograms * 1000.0;
        double simulationEquilibriumBallastPosition =
            static_cast<double>(simulationConfig.EquilibriumBallastPosition);
        double verticalControlInitialEquilibriumBallastFill =
            static_cast<double>(config.VerticalControl.InitialEquilibriumBallastFill);
        double simulationInitialDepthMeters = simulationConfig.InitialDepth.Value;
        double simulationSeaFloorDepthMeters = simulationConfig.SeaFloorDepth.Value;
        std::string videoResourceId = config.VideoController.ResourceId.Value;
        std::string videoSourceDescription;
        std::string startupVideoEndpoint;
        bool startupVideoEnable = config.StartupVideoEnable;

        CLI::App app{"PiSubmarine fake drone server"};
        app.add_option("--grpc-address", grpcAddress, "Shared gRPC bind address")
            ->default_val(grpcAddress);
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
        app.add_option("--video-resource-id", videoResourceId, "Lease resource id used for video streaming")
            ->default_val(videoResourceId);
        app.add_option(
                "--simulation-drone-mass-kg",
                simulationDroneMassKilograms,
                "Simulated dry mass of the drone in kilograms.")
            ->default_val(simulationDroneMassKilograms);
        app.add_option(
                "--simulation-friction-coefficient",
                simulationFrictionCoefficient,
                "Linear water friction coefficient applied to vertical speed.")
            ->default_val(simulationFrictionCoefficient);
        app.add_option(
                "--simulation-ballast-max-mass-g",
                simulationBallastMaximumMassGrams,
                "Maximum ballast water mass in grams.")
            ->default_val(simulationBallastMaximumMassGrams);
        app.add_option(
                "--simulation-equilibrium-ballast-position",
                simulationEquilibriumBallastPosition,
                "Ballast position that keeps neutral buoyancy in the simulation.")
            ->default_val(simulationEquilibriumBallastPosition);
        app.add_option(
                "--vertical-initial-equilibrium-ballast-fill",
                verticalControlInitialEquilibriumBallastFill,
                "Initial ballast fill guess used by the vertical ballast controller.")
            ->default_val(verticalControlInitialEquilibriumBallastFill);
        app.add_option(
                "--simulation-initial-depth-m",
                simulationInitialDepthMeters,
                "Initial simulated depth in meters.")
            ->default_val(simulationInitialDepthMeters);
        app.add_option(
                "--simulation-sea-floor-depth-m",
                simulationSeaFloorDepthMeters,
                "Virtual sea floor depth in meters.")
            ->default_val(simulationSeaFloorDepthMeters);
        app.add_option(
                "--video-source",
                videoSourceDescription,
                "Optional explicit GStreamer source element description. Leave empty for autodetect.");
        app.add_option(
                "--startup-video-endpoint",
                startupVideoEndpoint,
                "Optional host:port RTP endpoint to subscribe immediately at startup.");
        app.add_flag(
                "--startup-video-enable",
                startupVideoEnable,
                "Enable video streaming target at startup using low-latency profile and autofocus.");

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

        std::optional<PiSubmarine::Video::Subscription::Api::Endpoint> parsedStartupVideoEndpoint;
        if (!startupVideoEndpoint.empty())
        {
            const auto parsedEndpoint = ParseEndpoint(startupVideoEndpoint);
            if (!parsedEndpoint.has_value())
            {
                SPDLOG_LOGGER_ERROR(logger, "Invalid --startup-video-endpoint value. Expected host:port.");
                return 2;
            }

            parsedStartupVideoEndpoint = PiSubmarine::Video::Subscription::Api::Endpoint{
                .Host = parsedEndpoint->Address,
                .Port = parsedEndpoint->Port};
        }

        config.ControlEndpoint = *parsedControlEndpoint;
        config.TelemetryEndpoint = *parsedTelemetryEndpoint;
        simulationConfig.DroneMassKilograms = simulationDroneMassKilograms;
        simulationConfig.FrictionCoefficient = simulationFrictionCoefficient;
        simulationConfig.BallastMaximumMassKilograms = simulationBallastMaximumMassGrams / 1000.0;
        simulationConfig.EquilibriumBallastPosition =
            PiSubmarine::Ballast::BallastFillFraction{
                PiSubmarine::NormalizedFraction{simulationEquilibriumBallastPosition}};
        config.VerticalControl.InitialEquilibriumBallastFill =
            PiSubmarine::Ballast::BallastFillFraction{
                PiSubmarine::NormalizedFraction{verticalControlInitialEquilibriumBallastFill}};
        simulationConfig.InitialDepth = PiSubmarine::Meters{simulationInitialDepthMeters};
        simulationConfig.SeaFloorDepth = PiSubmarine::Meters{simulationSeaFloorDepthMeters};
        config.StartupVideoEndpoint = parsedStartupVideoEndpoint;
        config.StartupVideoEnable = startupVideoEnable;
        config.TickPeriod = std::chrono::milliseconds(tickPeriodMilliseconds);
        config.GrpcServer.Address = grpcAddress;
        config.GrpcServer.ServerCertificateChain = ReadTextFile(serverCertificatePath);
        config.GrpcServer.ServerPrivateKey = ReadTextFile(serverPrivateKeyPath);
        config.GrpcServer.ClientCertificateAuthority = ReadTextFile(clientCertificateAuthorityPath);
        config.VideoController.ResourceId = PiSubmarine::Lease::Api::ResourceId{.Value = videoResourceId};
        if (!videoSourceDescription.empty())
        {
            config.VideoController.VideoSource = PiSubmarine::Video::Server::GStreamer::ElementSource{
                .Description = videoSourceDescription};
        }

        BallastProvider ballastProvider(PiSubmarine::Ballast::BallastFillFraction::Empty());
        BidirectionalMotor ballastMotor;
        BallastActuator ballastActuator(ballastMotor, ballastProvider);
        PiSubmarine::Ballast::Pid::Controller ballastController(
            ballastMotor,
            ballastProvider,
            PiSubmarine::Ballast::Pid::Controller::Config{
                .ProportionalGain = 20,
                .IntegralGainPerSecond = 0.25,
                .IntegralLimit = 1.0,
                .PositionDeadband = PiSubmarine::NormalizedFraction{0.01},
                .MaxDutyCycle = PiSubmarine::NormalizedFraction{1.0}},
            PiSubmarine::Ballast::BallastFillFraction::Empty());
        BatteryProvider batteryProvider;
        UnidirectionalMotor frontLeftMotor;
        UnidirectionalMotor frontRightMotor;
        UnidirectionalMotor rearLeftMotor;
        UnidirectionalMotor rearRightMotor;
        VerticalSimulationEngine verticalSimulation(ballastProvider, simulationConfig);
        DepthProvider depthProvider(verticalSimulation);
        ProximityProvider proximityProvider(verticalSimulation);
        LampController lampController;
        ServoController servoController;

        Runtime runtime(
            config,
            Runtime::Dependencies{
                .BallastController = ballastController,
                .BallastTelemetryProvider = ballastProvider,
                .BatteryTelemetryProvider = batteryProvider,
                .BallastMotorController = ballastMotor,
                .BallastMotorTelemetryProvider = ballastMotor,
                .FrontLeftMotorController = frontLeftMotor,
                .FrontLeftMotorTelemetryProvider = frontLeftMotor,
                .FrontRightMotorController = frontRightMotor,
                .FrontRightMotorTelemetryProvider = frontRightMotor,
                .RearLeftMotorController = rearLeftMotor,
                .RearLeftMotorTelemetryProvider = rearLeftMotor,
                .RearRightMotorController = rearRightMotor,
                .RearRightMotorTelemetryProvider = rearRightMotor,
                .DepthTelemetryProvider = depthProvider,
                .LampController = lampController,
                .LampTelemetryProvider = lampController,
                .ProximityTelemetryProvider = proximityProvider,
                .ServoController = servoController,
                .PlatformTickables = {
                    ballastActuator,
                    verticalSimulation,
                    batteryProvider,
                    lampController,
                    frontLeftMotor,
                    frontRightMotor,
                    rearLeftMotor,
                    rearRightMotor}});

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
