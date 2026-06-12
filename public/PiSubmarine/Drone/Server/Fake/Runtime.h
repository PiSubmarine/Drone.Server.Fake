#pragma once

#include <chrono>
#include <memory>

#include "PiSubmarine/Drone/Server/Fake/ErrorCode.h"
#include "PiSubmarine/Error/Api/Result.h"
#include "PiSubmarine/Lease/Server/Grpc/Server.h"
#include "PiSubmarine/Udp/Api/Endpoint.h"
#include "PiSubmarine/Video/Server/GStreamer/Config.h"
#include "PiSubmarine/Video/Subscription/Grpc/Server/Server.h"

namespace PiSubmarine::Drone::Server::Fake
{
    class Runtime
    {
    public:
        struct Config
        {
            Lease::Server::Grpc::TlsConfig LeaseServer;
            Video::Subscription::Grpc::Server::TlsConfig VideoSubscriptionServer;
            Video::Server::GStreamer::Config VideoController;
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
