#include "PiSubmarine/Drone/Server/Fake/UnidirectionalMotor.h"

namespace PiSubmarine::Drone::Server::Fake
{
	Error::Api::Result<void> UnidirectionalMotor::SetPowered(bool enabled)
	{
		m_IsPowered = enabled;
		return {};
	}

	Error::Api::Result<bool> UnidirectionalMotor::IsPowered() const
	{
		return m_IsPowered;
	}

	Error::Api::Result<NormalizedFraction> UnidirectionalMotor::GetDutyCycle() const
	{
		return m_DutyCycle;
	}

	Error::Api::Result<void> UnidirectionalMotor::SetDutyCycle(NormalizedFraction dutyCycle)
	{
		m_DutyCycle = dutyCycle;
		m_IsPowered = m_DutyCycle > 0;
		return {};
	}

	Error::Api::Result<NormalizedFraction> UnidirectionalMotor::GetMinimumEffectiveDutyCycle() const
	{
		return 0.2;
	}

	Error::Api::Result<Motor::Telemetry::Api::State> UnidirectionalMotor::GetState() const
	{
		return m_State;
	}

	void UnidirectionalMotor::Tick(const std::chrono::nanoseconds& uptime, const std::chrono::nanoseconds& deltaTime)
	{
		auto driveEffort = m_DutyCycle;
		if (driveEffort < GetMinimumEffectiveDutyCycle().value() || !m_IsPowered)
		{
			driveEffort = 0;
		}
		auto direction = driveEffort == 0
			                 ? Motor::Telemetry::Api::DriveDirection::Idle
			                 : Motor::Telemetry::Api::DriveDirection::Forward;

		const auto phase = (std::chrono::duration_cast<std::chrono::seconds>(uptime).count() / 2) % 3;
		if (phase == 0)
		{
			m_State = Motor::Telemetry::Api::State{
				.Operational = Motor::Telemetry::Api::OperationalState::Operational,
				.ActiveFaults = static_cast<Motor::Telemetry::Api::Faults>(0),
				.ActiveWarnings = static_cast<Motor::Telemetry::Api::Warnings>(0),
				.Direction = direction,
				.DriveEffort = driveEffort
			};
			return;
		}

		if (phase == 1)
		{
			m_State = Motor::Telemetry::Api::State{
				.Operational = Motor::Telemetry::Api::OperationalState::Degraded,
				.ActiveFaults = static_cast<Motor::Telemetry::Api::Faults>(0),
				.ActiveWarnings = Motor::Telemetry::Api::Warnings::Temperature,
				.Direction = direction,
				.DriveEffort = driveEffort
			};
			return;
		}

		m_State = Motor::Telemetry::Api::State{
			.Operational = Motor::Telemetry::Api::OperationalState::Faulted,
			.ActiveFaults = Motor::Telemetry::Api::Faults::Overcurrent,
			.ActiveWarnings = static_cast<Motor::Telemetry::Api::Warnings>(0),
			.Direction = direction,
			.DriveEffort = driveEffort
		};
	}
}
