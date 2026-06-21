#include "PiSubmarine/Drone/Server/Fake/BallastProvider.h"

namespace PiSubmarine::Drone::Server::Fake
{
    Error::Api::Result<Ballast::Telemetry::Api::State> BallastProvider::GetState() const
    {
        ++m_Tick;
        const auto cycle = static_cast<double>(m_Tick % 100) / 100.0;
        return Ballast::Telemetry::Api::State{
            .Position = NormalizedFraction{cycle}};
    }
}
