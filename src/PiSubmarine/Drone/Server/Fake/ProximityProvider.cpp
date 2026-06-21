#include "PiSubmarine/Drone/Server/Fake/ProximityProvider.h"

namespace PiSubmarine::Drone::Server::Fake
{
    Error::Api::Result<Proximity::Telemetry::Api::State> ProximityProvider::GetState() const
    {
        ++m_Tick;
        const auto cycle = static_cast<double>(m_Tick % 80) / 40.0;
        return Proximity::Telemetry::Api::State{
            .Distance = Meters{0.5 + cycle}};
    }
}
