#include "PiSubmarine/Drone/Server/Fake/DepthProvider.h"

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

    Error::Api::Result<Depth::Telemetry::Api::State> DepthProvider::GetState() const
    {
        return m_State;
    }

    void DepthProvider::Tick(const std::chrono::nanoseconds& uptime, const std::chrono::nanoseconds&)
    {
        m_State = Depth::Telemetry::Api::State{
            .Depth = Meters{1.0 + (FractionalCycle(uptime, std::chrono::seconds(6)) * 6.0)}};
    }
}
