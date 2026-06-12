#include "PiSubmarine/Drone/Server/Fake/Runtime.h"

#include <memory>
#include <stdexcept>

#include "PiSubmarine/Control/Engine.h"
#include "PiSubmarine/Control/Pilot/Dummy/Controller.h"
#include "PiSubmarine/Control/Pilot/Manual/Controller.h"
#include "PiSubmarine/Control/Protobuf/Deserializer.h"
#include "PiSubmarine/Control/Server/Udp/Server.h"
#include "PiSubmarine/Drone/Server/Fake/BatteryProvider.h"
#include "PiSubmarine/Drone/Server/Fake/GimbalController.h"
#include "PiSubmarine/Drone/Server/Fake/HorizontalController.h"
#include "PiSubmarine/Drone/Server/Fake/LampController.h"
#include "PiSubmarine/Drone/Server/Fake/LoggerFactory.h"
#include "PiSubmarine/Drone/Server/Fake/MotorProvider.h"
#include "PiSubmarine/Drone/Server/Fake/VerticalController.h"
#include "PiSubmarine/Error/Api/ErrorCondition.h"
#include "PiSubmarine/Error/Api/MakeError.h"
#include "PiSubmarine/Lease/InMemory/Manager.h"
#include "PiSubmarine/Telemetry/Aggregator.h"
#include "PiSubmarine/Telemetry/Protobuf/Serializer.h"
#include "PiSubmarine/Telemetry/Server/Udp/Server.h"
#include "PiSubmarine/Time/Manager.h"
#include "PiSubmarine/Udp/Asio/Socket.h"
#include "PiSubmarine/Video/Server/GStreamer/Controller.h"
#include "PiSubmarine/Video/Subscription/Grpc/Server/Server.h"

namespace PiSubmarine::Drone::Server::Fake
{
    namespace
    {
        [[nodiscard]] Error::Api::Error MakeCommunicationError(const ErrorCode code) noexcept
        {
            return Error::Api::MakeError(Error::Api::ErrorCondition::CommunicationError, make_error_code(code));
        }
    }

    class Runtime::Impl
    {
    public:
        explicit Impl(const Config& config)
            : LoggingFactory()
            , LeaseManager(LoggingFactory)
            , LeaseServer(LeaseManager, LoggingFactory, config.LeaseServer)
            , VideoController(config.VideoController, LoggingFactory, LeaseManager, LeaseManager)
            , VideoSubscriptionServer(VideoController, LoggingFactory, config.VideoSubscriptionServer)
            , ControlSocket(config.ReceiveQueueCapacity, config.MaxDatagramSize)
            , TelemetrySocket(config.ReceiveQueueCapacity, config.MaxDatagramSize)
            , TelemetryAggregator(Telemetry::Aggregator::Providers{
                .Battery = &m_BatteryProvider,
                .Thrusters = {&m_FrontLeftThruster, &m_FrontRightThruster, &m_RearLeftThruster, &m_RearRightThruster}})
            , ManualPilot(
                m_HorizontalController,
                m_VerticalController,
                m_GimbalController,
                m_LampController,
                VideoController)
            , HoldPositionPilot()
            , ControlEngine(
                LeaseManager,
                LeaseManager,
                ManualPilot,
                HoldPositionPilot)
            , ControlDeserializer()
            , ControlServer(
                ControlEngine,
                ControlDeserializer,
                ControlSocket)
            , TelemetrySerializer()
            , TelemetryServer(
                TelemetryAggregator,
                LeaseManager,
                LeaseManager,
                TelemetrySerializer,
                TelemetrySocket,
                TelemetrySocket)
            , TimeManager(config.TickPeriod)
            , ControlEndpoint(config.ControlEndpoint)
            , TelemetryEndpoint(config.TelemetryEndpoint)
        {
            ThrowIfError(TimeManager.AddTickable(ControlSocket), "adding control socket to Time.Manager");
            ThrowIfError(TimeManager.AddTickable(TelemetrySocket), "adding telemetry socket to Time.Manager");
            ThrowIfError(TimeManager.AddTickable(ControlServer), "adding control server to Time.Manager");
            ThrowIfError(TimeManager.AddTickable(ControlEngine), "adding control engine to Time.Manager");
            ThrowIfError(TimeManager.AddTickable(TelemetryServer), "adding telemetry server to Time.Manager");
            ThrowIfError(TimeManager.AddTickable(VideoController), "adding video controller to Time.Manager");
        }

        LoggerFactory LoggingFactory;
        Lease::InMemory::Manager LeaseManager;
        Lease::Server::Grpc::Server LeaseServer;
        Video::Server::GStreamer::Controller VideoController;
        Video::Subscription::Grpc::Server::Server VideoSubscriptionServer;

        Udp::Asio::Socket ControlSocket;
        Udp::Asio::Socket TelemetrySocket;

        HorizontalController m_HorizontalController;
        VerticalController m_VerticalController;
        GimbalController m_GimbalController;
        LampController m_LampController;

        BatteryProvider m_BatteryProvider;
        MotorProvider m_FrontLeftThruster;
        MotorProvider m_FrontRightThruster;
        MotorProvider m_RearLeftThruster;
        MotorProvider m_RearRightThruster;

        Telemetry::Aggregator TelemetryAggregator;
        Control::Pilot::Manual::Controller ManualPilot;
        Control::Pilot::Dummy::Controller HoldPositionPilot;
        Control::Engine ControlEngine;
        Control::Protobuf::Deserializer ControlDeserializer;
        Control::Server::Udp::Server ControlServer;
        Telemetry::Protobuf::Serializer TelemetrySerializer;
        Telemetry::Server::Udp::Server TelemetryServer;
        Time::Manager TimeManager;

        Udp::Api::Endpoint ControlEndpoint;
        Udp::Api::Endpoint TelemetryEndpoint;
        bool ControlSocketBound = false;
        bool TelemetrySocketBound = false;
    };

    Runtime::Runtime(const Config& config)
        : m_Impl(std::make_unique<Impl>(config))
    {
    }

    Runtime::~Runtime()
    {
        Stop();
    }

    Error::Api::Result<void> Runtime::Run()
    {
        if (m_IsRunning)
        {
            return std::unexpected(MakeRuntimeError(ErrorCode::AlreadyStarted));
        }

        const auto startResult = StartServices();
        if (!startResult.has_value())
        {
            StopServices();
            return startResult;
        }

        m_IsRunning = true;
        const auto runResult = m_Impl->TimeManager.Run();
        m_IsRunning = false;
        StopServices();
        return runResult;
    }

    void Runtime::Stop() noexcept
    {
        if (!m_Impl)
        {
            return;
        }

        m_Impl->TimeManager.Stop();
        StopServices();
        m_IsRunning = false;
    }

    bool Runtime::IsRunning() const noexcept
    {
        return m_IsRunning;
    }

    Error::Api::Result<void> Runtime::StartServices()
    {
        if (!m_Impl->ControlSocketBound)
        {
            const auto bindResult = m_Impl->ControlSocket.Bind(m_Impl->ControlEndpoint);
            if (!bindResult.has_value())
            {
                return std::unexpected(MakeRuntimeError(ErrorCode::ControlBindFailed));
            }

            m_Impl->ControlSocketBound = true;
        }

        if (!m_Impl->TelemetrySocketBound)
        {
            const auto bindResult = m_Impl->TelemetrySocket.Bind(m_Impl->TelemetryEndpoint);
            if (!bindResult.has_value())
            {
                return std::unexpected(MakeRuntimeError(ErrorCode::TelemetryBindFailed));
            }

            m_Impl->TelemetrySocketBound = true;
        }

        const auto leaseStartResult = m_Impl->LeaseServer.Start();
        if (!leaseStartResult.has_value())
        {
            return std::unexpected(MakeRuntimeError(ErrorCode::LeaseServerStartFailed));
        }

        const auto videoSubscriptionStartResult = m_Impl->VideoSubscriptionServer.Start();
        if (!videoSubscriptionStartResult.has_value())
        {
            return std::unexpected(MakeRuntimeError(ErrorCode::VideoSubscriptionServerStartFailed));
        }

        return {};
    }

    void Runtime::StopServices()
    {
        if (!m_Impl)
        {
            return;
        }

        m_Impl->VideoSubscriptionServer.Stop();
        m_Impl->LeaseServer.Stop();
    }

    Error::Api::Error Runtime::MakeRuntimeError(const ErrorCode code) noexcept
    {
        return MakeCommunicationError(code);
    }

    void Runtime::ThrowIfError(const Error::Api::Result<void>& result, const char* action)
    {
        if (!result.has_value())
        {
            throw std::runtime_error(std::string("Drone.Server.Fake runtime failed while ") + action);
        }
    }
}
