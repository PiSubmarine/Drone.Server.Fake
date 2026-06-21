#include "PiSubmarine/Drone/Server/Fake/LampController.h"

namespace PiSubmarine::Drone::Server::Fake
{
    Error::Api::Result<void> LampController::SetTarget(const Control::Lamp::Api::Command& target)
    {
        m_Target = target;
        return {};
    }

    Error::Api::Result<Lamp::Telemetry::Api::Status> LampController::GetStatus() const
    {
        return Lamp::Telemetry::Api::Status{
            .IsActive = static_cast<double>(m_Target.Intensity()) > 0.0};
    }
}
