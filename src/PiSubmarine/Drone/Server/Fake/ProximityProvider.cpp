#include "PiSubmarine/Drone/Server/Fake/ProximityProvider.h"

#include <cmath>

namespace PiSubmarine::Drone::Server::Fake
{
    namespace
    {
        [[nodiscard]] double FractionalCycle(
            const std::chrono::nanoseconds& uptime,
            const std::chrono::duration<double>& period)
        {
            return std::fmod(std::chrono::duration<double>(uptime).count(), period.count()) / period.count();
        }
    }

    Error::Api::Result<Proximity::Telemetry::Api::State> ProximityProvider::GetState() const
    {
        return m_State;
    }

    void ProximityProvider::Tick(const std::chrono::nanoseconds& uptime, const std::chrono::nanoseconds&)
    {
        m_State = Proximity::Telemetry::Api::State{
            .Distance = Meters{0.5 + (FractionalCycle(uptime, std::chrono::seconds(4)) * 2.0)}};
    }
}
