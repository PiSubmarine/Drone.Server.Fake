#include "PiSubmarine/Drone/Server/Fake/DepthProvider.h"

namespace PiSubmarine::Drone::Server::Fake
{
    Error::Api::Result<Depth::Telemetry::Api::State> DepthProvider::GetState() const
    {
        ++m_Tick;
        const auto cycle = static_cast<double>(m_Tick % 120) / 20.0;
        return Depth::Telemetry::Api::State{
            .Depth = Meters{1.0 + cycle}};
    }
}
