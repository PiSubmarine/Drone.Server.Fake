#pragma once

#include "PiSubmarine/Motor/Unidirectional/Api/IController.h"
#include "PiSubmarine/Motor/Telemetry/Api/IProvider.h"
#include "PiSubmarine/Time/ITickable.h"


namespace PiSubmarine::Drone::Server::Fake
{
	class UnidirectionalMotor : public Motor::Unidirectional::Api::IController,
	                            public Motor::Telemetry::Api::IProvider,
	                            public Time::ITickable
	{
	public:
		[[nodiscard]] Error::Api::Result<void> SetPowered(bool enabled) override;
		[[nodiscard]] Error::Api::Result<bool> IsPowered() const override;
		[[nodiscard]] Error::Api::Result<NormalizedFraction> GetDutyCycle() const override;
		[[nodiscard]] Error::Api::Result<void> SetDutyCycle(NormalizedFraction dutyCycle) override;
		[[nodiscard]] Error::Api::Result<NormalizedFraction> GetMinimumEffectiveDutyCycle() const override;

		[[nodiscard]] Error::Api::Result<Motor::Telemetry::Api::State> GetState() const override;
		void Tick(const std::chrono::nanoseconds& uptime, const std::chrono::nanoseconds& deltaTime) override;

	private:
		bool m_IsPowered = false;
		NormalizedFraction m_DutyCycle = 0;

		Motor::Telemetry::Api::State m_State{
			.Operational = Motor::Telemetry::Api::OperationalState::Operational,
			.ActiveFaults = static_cast<Motor::Telemetry::Api::Faults>(0),
			.ActiveWarnings = static_cast<Motor::Telemetry::Api::Warnings>(0),
			.Direction = Motor::Telemetry::Api::DriveDirection::Idle,
			.DriveEffort = NormalizedFraction{0}};
	};
}
