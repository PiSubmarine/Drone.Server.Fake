#include "PiSubmarine/Drone/Server/Fake/MotorProvider.h"

namespace PiSubmarine::Drone::Server::Fake
{
    Error::Api::Result<Motor::Telemetry::Api::State> MotorProvider::GetState() const
    {
        return Motor::Telemetry::Api::State{
            .Operational = Motor::Telemetry::Api::OperationalState::Operational,
            .ActiveFaults = static_cast<Motor::Telemetry::Api::Faults>(0),
            .ActiveWarnings = static_cast<Motor::Telemetry::Api::Warnings>(0)};
    }
}
