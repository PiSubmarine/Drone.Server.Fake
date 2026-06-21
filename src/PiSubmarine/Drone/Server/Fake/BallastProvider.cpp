#include "PiSubmarine/Drone/Server/Fake/BallastProvider.h"

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

    Error::Api::Result<Ballast::Telemetry::Api::State> BallastProvider::GetState() const
    {
        return m_State;
    }

    void BallastProvider::Tick(const std::chrono::nanoseconds& uptime, const std::chrono::nanoseconds&)
    {
        m_State = Ballast::Telemetry::Api::State{
            .Position = NormalizedFraction{FractionalCycle(uptime, std::chrono::seconds(10))}};
    }
}
