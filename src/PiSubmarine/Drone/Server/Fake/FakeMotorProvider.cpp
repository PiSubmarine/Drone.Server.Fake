#include "PiSubmarine/Drone/Server/Fake/FakeMotorProvider.h"

namespace PiSubmarine::Drone::Server::Fake
{
    Error::Api::Result<Motor::Telemetry::Api::State> FakeMotorProvider::GetState() const
    {
        return Motor::Telemetry::Api::State{
            .Operational = Motor::Telemetry::Api::OperationalState::Operational,
            .ActiveFaults = static_cast<Motor::Telemetry::Api::Faults>(0),
            .ActiveWarnings = static_cast<Motor::Telemetry::Api::Warnings>(0)};
    }
}
