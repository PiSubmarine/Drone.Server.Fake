#include "PiSubmarine/Drone/Server/Fake/LampController.h"

namespace PiSubmarine::Drone::Server::Fake
{
    Error::Api::Result<void> LampController::SetTarget(const Control::Lamp::Api::Command& target)
    {
        m_Target = target;
        m_Status.Intensity = m_Target.Intensity();
        return {};
    }

    Error::Api::Result<Lamp::Telemetry::Api::Status> LampController::GetStatus() const
    {
        return m_Status;
    }

    void LampController::Tick(const std::chrono::nanoseconds& uptime, const std::chrono::nanoseconds&)
    {
        const auto intensity = m_Target.Intensity();
        const auto isActive = static_cast<double>(intensity) > 0.0;
        const auto phase = (std::chrono::duration_cast<std::chrono::seconds>(uptime).count() / 2) % 3;

        m_Status = Lamp::Telemetry::Api::Status{
            .Intensity = intensity,
            .HasOvercurrentFault = isActive && phase == 2,
            .HasOvertemperatureWarning = isActive && phase == 1};
    }
}
