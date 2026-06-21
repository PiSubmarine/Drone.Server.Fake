#include "PiSubmarine/Drone/Server/Fake/MotorProvider.h"

namespace PiSubmarine::Drone::Server::Fake
{
    Error::Api::Result<Motor::Telemetry::Api::State> MotorProvider::GetState() const
    {
        return m_State;
    }

    void MotorProvider::Tick(const std::chrono::nanoseconds& uptime, const std::chrono::nanoseconds&)
    {
        const auto phase = (std::chrono::duration_cast<std::chrono::seconds>(uptime).count() / 2) % 3;
        if (phase == 0)
        {
            m_State = Motor::Telemetry::Api::State{
                .Operational = Motor::Telemetry::Api::OperationalState::Operational,
                .ActiveFaults = static_cast<Motor::Telemetry::Api::Faults>(0),
                .ActiveWarnings = static_cast<Motor::Telemetry::Api::Warnings>(0)};
            return;
        }

        if (phase == 1)
        {
            m_State = Motor::Telemetry::Api::State{
                .Operational = Motor::Telemetry::Api::OperationalState::Degraded,
                .ActiveFaults = static_cast<Motor::Telemetry::Api::Faults>(0),
                .ActiveWarnings = Motor::Telemetry::Api::Warnings::Temperature};
            return;
        }

        m_State = Motor::Telemetry::Api::State{
            .Operational = Motor::Telemetry::Api::OperationalState::Faulted,
            .ActiveFaults = Motor::Telemetry::Api::Faults::Overcurrent,
            .ActiveWarnings = static_cast<Motor::Telemetry::Api::Warnings>(0)};
    }
}
