#include "PiSubmarine/Drone/Server/Fake/Runtime.h"

#include <memory>
#include <optional>
#include <stdexcept>

#include <spdlog/logger.h>
#include <spdlog/spdlog.h>

#include "PiSubmarine/Control/Api/Input/OperatorCommand.h"
#include "PiSubmarine/Control/Engine.h"
#include "PiSubmarine/Control/Pilot/Dummy/Controller.h"
#include "PiSubmarine/Control/Pilot/Manual/Controller.h"
#include "PiSubmarine/Control/Protobuf/Deserializer.h"
#include "PiSubmarine/Control/Server/Udp/Server.h"
#include "PiSubmarine/Control/Video/Api/Command.h"
#include "PiSubmarine/Ballast/Telemetry/Protobuf/Serializer.h"
#include "PiSubmarine/Battery/Telemetry/Protobuf/Serializer.h"
#include "PiSubmarine/Depth/Telemetry/Protobuf/Serializer.h"
#include "PiSubmarine/Drone/Server/Fake/BallastProvider.h"
#include "PiSubmarine/Drone/Server/Fake/BatteryProvider.h"
#include "PiSubmarine/Drone/Server/Fake/DepthProvider.h"
#include "PiSubmarine/Drone/Server/Fake/GimbalController.h"
#include "PiSubmarine/Drone/Server/Fake/HorizontalController.h"
#include "PiSubmarine/Drone/Server/Fake/LampController.h"
#include "PiSubmarine/Drone/Server/Fake/LoggerFactory.h"
#include "PiSubmarine/Drone/Server/Fake/MotorProvider.h"
#include "PiSubmarine/Drone/Server/Fake/ProximityProvider.h"
#include "PiSubmarine/Drone/Server/Fake/VerticalController.h"
#include "PiSubmarine/Error/Api/ErrorCondition.h"
#include "PiSubmarine/Error/Api/MakeError.h"
#include "PiSubmarine/Grpc/Server/Server.h"
#include "PiSubmarine/Lease/Api/ILeaseIssuer.h"
#include "PiSubmarine/Lease/Api/Lease.h"
#include "PiSubmarine/Lease/Api/LeaseRequest.h"
#include "PiSubmarine/Lease/InMemory/Manager.h"
#include "PiSubmarine/Lease/Server/Grpc/Adapter.h"
#include "PiSubmarine/Lamp/Telemetry/Protobuf/Serializer.h"
#include "PiSubmarine/Motor/Telemetry/Protobuf/Serializer.h"
#include "PiSubmarine/Proximity/Telemetry/Protobuf/Serializer.h"
#include "PiSubmarine/Security/Aead/Openssl/Provider.h"
#include "PiSubmarine/Security/Nonce/Openssl/Provider.h"
#include "PiSubmarine/Telemetry/Channels/Api/Channels.h"
#include "PiSubmarine/Telemetry/Server/Udp/Server.h"
#include "PiSubmarine/Time/Manager.h"
#include "PiSubmarine/Time/ITickable.h"
#include "PiSubmarine/Time/Telemetry/Protobuf/Serializer.h"
#include "PiSubmarine/Udp/Asio/Socket.h"
#include "PiSubmarine/Video/Server/GStreamer/Controller.h"
#include "PiSubmarine/Video/Telemetry/Protobuf/Serializer.h"
#include "PiSubmarine/Video/Subscription/Api/IService.h"
#include "PiSubmarine/Video/Subscription/Api/SubscribeRequest.h"
#include "PiSubmarine/Video/Subscription/Api/UnsubscribeRequest.h"
#include "PiSubmarine/Video/Subscription/Grpc/Server/Adapter.h"

namespace PiSubmarine::Drone::Server::Fake
{
	namespace
	{
		constexpr std::string_view ControlResourceIdValue = "control-main";

		[[nodiscard]] Telemetry::Api::ChannelId MakeChannelId(const std::string_view value)
		{
			return Telemetry::Api::ChannelId{.Value = std::string(value)};
		}

		[[nodiscard]] Error::Api::Error MakeCommunicationError(const ErrorCode code) noexcept
		{
			return Error::Api::MakeError(Error::Api::ErrorCondition::CommunicationError, make_error_code(code));
		}

		[[nodiscard]] Lease::Api::ResourceId MakeControlResourceId()
		{
			return Lease::Api::ResourceId{.Value = std::string(ControlResourceIdValue)};
		}

		class StartupVideoSubscriber final : public Time::ITickable
		{
		public:
			StartupVideoSubscriber(
				std::optional<Video::Subscription::Api::Endpoint> endpoint,
				Lease::Api::ResourceId resourceId,
				Lease::Api::ILeaseIssuer& leaseIssuer,
				Video::Subscription::Api::IService& videoService,
				Logging::Api::IFactory& loggerFactory)
				: m_Endpoint(std::move(endpoint)),
				  m_ResourceId(std::move(resourceId)),
				  m_LeaseIssuer(leaseIssuer),
				  m_VideoService(videoService),
				  m_Logger(loggerFactory.CreateLogger("Drone.Server.Fake.StartupVideoSubscriber"))
			{
			}

			[[nodiscard]] Error::Api::Result<void> Start()
			{
				if (!m_Endpoint.has_value())
				{
					return {};
				}

				return EnsureSubscribed(std::chrono::nanoseconds::zero());
			}

			void Stop() noexcept
			{
				if (!m_ActiveLease.has_value())
				{
					return;
				}

				const auto leaseId = m_ActiveLease->Id;
				const auto unsubscribeResult = m_VideoService.Unsubscribe(
					Video::Subscription::Api::UnsubscribeRequest{.LeaseId = leaseId});
				if (!unsubscribeResult.has_value())
				{
					SPDLOG_LOGGER_WARN(m_Logger, "Failed to remove startup video subscription for lease {}.",
					                   leaseId.Value);
				}

				const auto releaseResult = m_LeaseIssuer.ReleaseLease(leaseId);
				if (!releaseResult.has_value())
				{
					SPDLOG_LOGGER_WARN(m_Logger, "Failed to release startup video lease {}.", leaseId.Value);
				}

				ClearLeaseState();
			}

			void Tick(const std::chrono::nanoseconds& uptime, const std::chrono::nanoseconds&) override
			{
				if (!m_Endpoint.has_value() || uptime < m_NextActionAt)
				{
					return;
				}

				if (!m_ActiveLease.has_value())
				{
					const auto subscribeResult = EnsureSubscribed(uptime);
					if (!subscribeResult.has_value())
					{
						SPDLOG_LOGGER_WARN(
							m_Logger,
							"Retrying startup video subscription for resource {} after failure.",
							m_ResourceId.Value);
						m_NextActionAt = uptime + RetryDelay;
					}

					return;
				}

				const auto renewResult = m_LeaseIssuer.RenewLease(m_ActiveLease->Id);
				if (!renewResult.has_value())
				{
					SPDLOG_LOGGER_WARN(
						m_Logger,
						"Failed to renew startup video lease {} for resource {}.",
						m_ActiveLease->Id.Value,
						m_ResourceId.Value);
					CleanupStaleSubscription();
					m_NextActionAt = uptime + RetryDelay;
					return;
				}

				m_ActiveLease = renewResult.value();
				m_NextActionAt = uptime + HalfDuration(m_ActiveLease->Duration);
			}

		private:
			[[nodiscard]] Error::Api::Result<void> EnsureSubscribed(const std::chrono::nanoseconds& uptime)
			{
				if (!m_Endpoint.has_value())
				{
					return {};
				}

				const auto acquireResult = m_LeaseIssuer.AcquireLease(
					Lease::Api::LeaseRequest{.Resource = m_ResourceId});
				if (!acquireResult.has_value())
				{
					return std::unexpected(acquireResult.error());
				}

				const auto acquiredLease = acquireResult->GrantedLease;
				const auto subscribeResult = m_VideoService.Subscribe(
					Video::Subscription::Api::SubscribeRequest{
						.LeaseId = acquiredLease.Id,
						.ClientEndpoint = *m_Endpoint
					});
				if (!subscribeResult.has_value())
				{
					const auto releaseResult = m_LeaseIssuer.ReleaseLease(acquiredLease.Id);
					if (!releaseResult.has_value())
					{
						SPDLOG_LOGGER_WARN(
							m_Logger,
							"Failed to release startup video lease {} after subscribe failure.",
							acquiredLease.Id.Value);
					}

					return std::unexpected(subscribeResult.error());
				}

				m_ActiveLease = acquiredLease;
				m_NextActionAt = uptime + HalfDuration(acquiredLease.Duration);
				SPDLOG_LOGGER_INFO(
					m_Logger,
					"Created startup video subscription for {}:{} using lease {} on resource {}.",
					m_Endpoint->Host,
					m_Endpoint->Port,
					acquiredLease.Id.Value,
					m_ResourceId.Value);
				return {};
			}

			void CleanupStaleSubscription() noexcept
			{
				if (!m_ActiveLease.has_value())
				{
					return;
				}

				const auto leaseId = m_ActiveLease->Id;
				const auto unsubscribeResult = m_VideoService.Unsubscribe(
					Video::Subscription::Api::UnsubscribeRequest{.LeaseId = leaseId});
				if (!unsubscribeResult.has_value())
				{
					SPDLOG_LOGGER_WARN(
						m_Logger,
						"Failed to remove stale startup video subscription for lease {}.",
						leaseId.Value);
				}

				ClearLeaseState();
			}

			void ClearLeaseState() noexcept
			{
				m_ActiveLease.reset();
				m_NextActionAt = std::chrono::nanoseconds::zero();
			}

			[[nodiscard]] static std::chrono::nanoseconds HalfDuration(
				const std::chrono::milliseconds duration) noexcept
			{
				const auto halfDuration = duration / 2;
				if (halfDuration > std::chrono::milliseconds::zero())
				{
					return std::chrono::duration_cast<std::chrono::nanoseconds>(halfDuration);
				}

				return std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
			}

			static constexpr auto RetryDelay = std::chrono::seconds(1);

			std::optional<Video::Subscription::Api::Endpoint> m_Endpoint;
			Lease::Api::ResourceId m_ResourceId;
			Lease::Api::ILeaseIssuer& m_LeaseIssuer;
			Video::Subscription::Api::IService& m_VideoService;
			std::shared_ptr<spdlog::logger> m_Logger;
			std::optional<Lease::Api::Lease> m_ActiveLease;
			std::chrono::nanoseconds m_NextActionAt = std::chrono::nanoseconds::zero();
		};
	}

	class Runtime::Impl
	{
	public:
		explicit Impl(const Config& config)
			: LeaseManager(LoggingFactory),
			  GrpcServer(LoggingFactory, config.GrpcServer),
			  LeaseServer(LeaseManager, LoggingFactory),
			  VideoController(config.VideoController, LoggingFactory, LeaseManager, LeaseManager),
			  VideoSubscriptionServer(VideoController, LoggingFactory),
			  StartupSubscriber(
				  config.StartupVideoEndpoint,
				  config.VideoController.ResourceId,
				  LeaseManager,
				  VideoController,
				  LoggingFactory),
			  ControlSocket(config.ReceiveQueueCapacity, config.MaxDatagramSize),
			  TelemetrySocket(config.ReceiveQueueCapacity, config.MaxDatagramSize),
			  BallastTelemetrySerializer(m_BallastProvider),
			  BatteryTelemetrySerializer(m_BatteryProvider),
			  DepthTelemetrySerializer(m_DepthProvider),
			  LampTelemetrySerializer(m_LampController),
			  FrontLeftMotorTelemetrySerializer(m_FrontLeftThruster),
			  FrontRightMotorTelemetrySerializer(m_FrontRightThruster),
			  ProximityTelemetrySerializer(m_ProximityProvider),
			  RearLeftMotorTelemetrySerializer(m_RearLeftThruster),
			  RearRightMotorTelemetrySerializer(m_RearRightThruster),
			  TimeManager(config.TickPeriod),
			  TimeTelemetrySerializer(TimeManager),
			  VideoTelemetrySerializer(VideoController),
			  TelemetrySources({
				  {MakeChannelId(Telemetry::Channels::Api::BallastMain), &BallastTelemetrySerializer},
				  {MakeChannelId(Telemetry::Channels::Api::BatteryMain), &BatteryTelemetrySerializer},
				  {MakeChannelId(Telemetry::Channels::Api::DepthMain), &DepthTelemetrySerializer},
				  {MakeChannelId(Telemetry::Channels::Api::LampMain), &LampTelemetrySerializer},
				  {MakeChannelId(Telemetry::Channels::Api::MotorFrontLeft), &FrontLeftMotorTelemetrySerializer},
				  {MakeChannelId(Telemetry::Channels::Api::MotorFrontRight), &FrontRightMotorTelemetrySerializer},
				  {MakeChannelId(Telemetry::Channels::Api::MotorRearLeft), &RearLeftMotorTelemetrySerializer},
				  {MakeChannelId(Telemetry::Channels::Api::MotorRearRight), &RearRightMotorTelemetrySerializer},
				  {MakeChannelId(Telemetry::Channels::Api::ProximityMain), &ProximityTelemetrySerializer},
				  {MakeChannelId(Telemetry::Channels::Api::TimeMain), &TimeTelemetrySerializer},
				  {MakeChannelId(Telemetry::Channels::Api::VideoMain), &VideoTelemetrySerializer}
			  }),
			  ManualPilot(
				  m_HorizontalController,
				  m_VerticalController,
				  m_GimbalController,
				  m_LampController,
				  VideoController),
			  ControlEngine(
				  LeaseManager,
				  LeaseManager,
				  ManualPilot,
				  HoldPositionPilot),
			  ControlServer(
				  ControlEngine,
				  ControlDeserializer,
				  LeaseManager,
				  ControlAeadProvider,
				  ControlSocket),
			  TelemetryServer(
				  TelemetrySources,
				  LeaseManager,
				  LeaseManager,
				  LeaseManager,
				  TelemetryAeadProvider,
				  TelemetryNonceProvider,
				  TelemetrySocket,
				  TelemetrySocket),
			  ControlEndpoint(config.ControlEndpoint),
			  TelemetryEndpoint(config.TelemetryEndpoint),
			  StartupVideoEnable(config.StartupVideoEnable)
		{
			ThrowIfError(TimeManager.AddTickable(ControlSocket), "adding control socket to Time.Manager");
			ThrowIfError(TimeManager.AddTickable(TelemetrySocket), "adding telemetry socket to Time.Manager");
			ThrowIfError(TimeManager.AddTickable(ControlServer), "adding control server to Time.Manager");
			ThrowIfError(TimeManager.AddTickable(ControlEngine), "adding control engine to Time.Manager");
			ThrowIfError(TimeManager.AddTickable(TelemetryServer), "adding telemetry server to Time.Manager");
			ThrowIfError(TimeManager.AddTickable(VideoController), "adding video controller to Time.Manager");
			ThrowIfError(TimeManager.AddTickable(m_BallastProvider), "adding ballast provider to Time.Manager");
			ThrowIfError(TimeManager.AddTickable(m_BatteryProvider), "adding battery provider to Time.Manager");
			ThrowIfError(TimeManager.AddTickable(m_DepthProvider), "adding depth provider to Time.Manager");
			ThrowIfError(TimeManager.AddTickable(m_LampController), "adding lamp controller to Time.Manager");
			ThrowIfError(TimeManager.AddTickable(m_FrontLeftThruster), "adding front-left motor provider to Time.Manager");
			ThrowIfError(TimeManager.AddTickable(m_FrontRightThruster), "adding front-right motor provider to Time.Manager");
			ThrowIfError(TimeManager.AddTickable(m_ProximityProvider), "adding proximity provider to Time.Manager");
			ThrowIfError(TimeManager.AddTickable(m_RearLeftThruster), "adding rear-left motor provider to Time.Manager");
			ThrowIfError(TimeManager.AddTickable(m_RearRightThruster), "adding rear-right motor provider to Time.Manager");
			ThrowIfError(TimeManager.AddTickable(StartupSubscriber), "adding startup video subscriber to Time.Manager");

			GrpcServer.RegisterService(LeaseServer);
			GrpcServer.RegisterService(VideoSubscriptionServer);
		}

		LoggerFactory LoggingFactory;
		Lease::InMemory::Manager LeaseManager;
		::PiSubmarine::Grpc::Server::Server GrpcServer;
		Lease::Server::Grpc::Adapter LeaseServer;
		Video::Server::GStreamer::Controller VideoController;
		Video::Subscription::Grpc::Server::Adapter VideoSubscriptionServer;
		StartupVideoSubscriber StartupSubscriber;

		Udp::Asio::Socket ControlSocket;
		Udp::Asio::Socket TelemetrySocket;
		Security::Aead::Openssl::Provider ControlAeadProvider;
		Security::Aead::Openssl::Provider TelemetryAeadProvider;
		Security::Nonce::Openssl::Provider TelemetryNonceProvider;

		HorizontalController m_HorizontalController;
		VerticalController m_VerticalController;
		GimbalController m_GimbalController;
		LampController m_LampController;

		BallastProvider m_BallastProvider;
		BatteryProvider m_BatteryProvider;
		DepthProvider m_DepthProvider;
		MotorProvider m_FrontLeftThruster;
		MotorProvider m_FrontRightThruster;
		ProximityProvider m_ProximityProvider;
		MotorProvider m_RearLeftThruster;
		MotorProvider m_RearRightThruster;

		Ballast::Telemetry::Protobuf::Serializer BallastTelemetrySerializer;
		Battery::Telemetry::Protobuf::Serializer BatteryTelemetrySerializer;
		Depth::Telemetry::Protobuf::Serializer DepthTelemetrySerializer;
		Lamp::Telemetry::Protobuf::Serializer LampTelemetrySerializer;
		Motor::Telemetry::Protobuf::Serializer FrontLeftMotorTelemetrySerializer;
		Motor::Telemetry::Protobuf::Serializer FrontRightMotorTelemetrySerializer;
		Proximity::Telemetry::Protobuf::Serializer ProximityTelemetrySerializer;
		Motor::Telemetry::Protobuf::Serializer RearLeftMotorTelemetrySerializer;
		Motor::Telemetry::Protobuf::Serializer RearRightMotorTelemetrySerializer;
		Time::Manager TimeManager;
		Time::Telemetry::Protobuf::Serializer TimeTelemetrySerializer;
		Video::Telemetry::Protobuf::Serializer VideoTelemetrySerializer;
		Telemetry::Server::Udp::Server::Sources TelemetrySources;
		Control::Pilot::Manual::Controller ManualPilot;
		Control::Pilot::Dummy::Controller HoldPositionPilot;
		Control::Engine ControlEngine;
		Control::Protobuf::Deserializer ControlDeserializer;
		Control::Server::Udp::Server ControlServer;
		Telemetry::Server::Udp::Server TelemetryServer;

		Udp::Api::Endpoint ControlEndpoint;
		Udp::Api::Endpoint TelemetryEndpoint;
		bool StartupVideoEnable = false;
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

		const auto grpcStartResult = m_Impl->GrpcServer.Start();
		if (!grpcStartResult.has_value())
		{
			return std::unexpected(MakeRuntimeError(ErrorCode::GrpcServerStartFailed));
		}

		if (m_Impl->StartupVideoEnable)
		{
			const auto controlLeaseResult = m_Impl->LeaseManager.AcquireLease(Lease::Api::LeaseRequest{
				.Resource = MakeControlResourceId()
			});
			if (!controlLeaseResult.has_value())
			{
				return std::unexpected(controlLeaseResult.error());
			}

			const auto controlLeaseId = controlLeaseResult->GrantedLease.Id;
			const auto submitResult = m_Impl->ControlEngine.Submit(Control::Api::Input::OperatorCommand{
				.LeaseId = controlLeaseId,
				.VideoControl = Control::Video::Api::Command::Enable(
					Control::Video::Api::StreamProfile::LowLatency,
					Control::Video::Api::AutoFocus{})
			});
			const auto releaseResult = m_Impl->LeaseManager.ReleaseLease(controlLeaseId);

			if (!submitResult.has_value())
			{
				return std::unexpected(submitResult.error());
			}

			if (!releaseResult.has_value())
			{
				return std::unexpected(releaseResult.error());
			}
		}

		const auto startupVideoSubscriptionResult = m_Impl->StartupSubscriber.Start();
		if (!startupVideoSubscriptionResult.has_value())
		{
			return std::unexpected(MakeRuntimeError(ErrorCode::StartupVideoSubscriptionFailed));
		}

		return {};
	}

	void Runtime::StopServices()
	{
		if (!m_Impl)
		{
			return;
		}

		m_Impl->StartupSubscriber.Stop();
		m_Impl->GrpcServer.Stop();
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
