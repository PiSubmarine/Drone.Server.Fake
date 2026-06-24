#include "PiSubmarine/Drone/Server/Fake/BidirectionalMotor.h"

#include <cmath>

namespace PiSubmarine::Drone::Server::Fake
{
    namespace
    {
        [[nodiscard]] Motor::Telemetry::Api::DriveDirection ToDriveDirection(const double dutyCycle)
        {
            if (dutyCycle > 0.0)
            {
                return Motor::Telemetry::Api::DriveDirection::Forward;
            }

            if (dutyCycle < 0.0)
            {
                return Motor::Telemetry::Api::DriveDirection::Reverse;
            }

            return Motor::Telemetry::Api::DriveDirection::Idle;
        }
    }

    Error::Api::Result<void> BidirectionalMotor::SetPowered(const bool enabled)
    {
        m_IsPowered = enabled;
        if (!m_IsPowered)
        {
            m_DutyCycle = SignedNormalizedFraction{0.0};
        }

        return {};
    }

    Error::Api::Result<bool> BidirectionalMotor::IsPowered() const
    {
        return m_IsPowered;
    }

    Error::Api::Result<SignedNormalizedFraction> BidirectionalMotor::GetDutyCycle() const
    {
        return m_DutyCycle;
    }

    Error::Api::Result<void> BidirectionalMotor::SetDutyCycle(const SignedNormalizedFraction dutyCycle)
    {
        m_DutyCycle = dutyCycle;
        m_IsPowered = static_cast<double>(m_DutyCycle) != 0.0;
        return {};
    }

    Error::Api::Result<NormalizedFraction> BidirectionalMotor::GetForwardMinimalEffectiveDutyCycle() const
    {
        return m_ForwardMinimalEffectiveDutyCycle;
    }

    Error::Api::Result<NormalizedFraction> BidirectionalMotor::GetReverseMinimalEffectiveDutyCycle() const
    {
        return m_ReverseMinimalEffectiveDutyCycle;
    }

    Error::Api::Result<Motor::Telemetry::Api::State> BidirectionalMotor::GetState() const
    {
        const auto requestedDutyCycle = m_IsPowered ? static_cast<double>(m_DutyCycle) : 0.0;
        const auto absoluteDutyCycle = std::abs(requestedDutyCycle);
        const auto minimumEffectiveDutyCycle = requestedDutyCycle >= 0.0
            ? static_cast<double>(m_ForwardMinimalEffectiveDutyCycle)
            : static_cast<double>(m_ReverseMinimalEffectiveDutyCycle);
        const auto driveEffort = absoluteDutyCycle >= minimumEffectiveDutyCycle
            ? NormalizedFraction{absoluteDutyCycle}
            : NormalizedFraction{0.0};
        const auto effectiveRequestedDutyCycle = static_cast<double>(driveEffort) > 0.0 ? requestedDutyCycle : 0.0;

        return Motor::Telemetry::Api::State{
            .Operational = Motor::Telemetry::Api::OperationalState::Operational,
            .ActiveFaults = static_cast<Motor::Telemetry::Api::Faults>(0),
            .ActiveWarnings = static_cast<Motor::Telemetry::Api::Warnings>(0),
            .Direction = ToDriveDirection(effectiveRequestedDutyCycle),
            .DriveEffort = driveEffort};
    }
}
