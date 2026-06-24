#pragma once

#include <chrono>
#include <memory>
#include <optional>

#include "PiSubmarine/Drone/Server/Fake/ErrorCode.h"
#include "PiSubmarine/Error/Api/Result.h"
#include "PiSubmarine/Grpc/Server/Server.h"
#include "PiSubmarine/Meters.h"
#include "PiSubmarine/NormalizedFraction.h"
#include "PiSubmarine/Udp/Api/Endpoint.h"
#include "PiSubmarine/Video/Server/GStreamer/Config.h"
#include "PiSubmarine/Video/Subscription/Api/Endpoint.h"

namespace PiSubmarine::Drone::Server::Fake
{
    class Runtime
    {
    public:
        struct SimulationConfig
        {
            double DroneMassKilograms = 8.0;
            double FrictionCoefficient = 1.0;
            double BallastMaximumMassKilograms = 0.3;
            NormalizedFraction EquilibriumBallastPosition = NormalizedFraction(0.5);
            Meters InitialDepth = 1.0_m;
            Meters SeaFloorDepth = 25.0_m;
        };

        struct Config
        {
            ::PiSubmarine::Grpc::Server::TlsConfig GrpcServer;
            Video::Server::GStreamer::Config VideoController;
            SimulationConfig Simulation;
            std::optional<Video::Subscription::Api::Endpoint> StartupVideoEndpoint;
            bool StartupVideoEnable = false;
            Udp::Api::Endpoint ControlEndpoint{"127.0.0.1", 50052};
            Udp::Api::Endpoint TelemetryEndpoint{"127.0.0.1", 50053};
            std::chrono::nanoseconds TickPeriod = std::chrono::milliseconds(10);
            std::size_t ReceiveQueueCapacity = 64;
            std::size_t MaxDatagramSize = 65507;
        };

        explicit Runtime(const Config& config);
        ~Runtime();

        [[nodiscard]] Error::Api::Result<void> Run();
        void Stop() noexcept;
        [[nodiscard]] bool IsRunning() const noexcept;

    private:
        [[nodiscard]] Error::Api::Result<void> StartServices();
        void StopServices();
        [[nodiscard]] static Error::Api::Error MakeRuntimeError(ErrorCode code) noexcept;
        static void ThrowIfError(const Error::Api::Result<void>& result, const char* action);

        class Impl;
        std::unique_ptr<Impl> m_Impl;
        bool m_IsRunning = false;
    };
}
