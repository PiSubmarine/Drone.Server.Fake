#include "PiSubmarine/Drone/Server/Fake/DepthProvider.h"
#include "PiSubmarine/Drone/Server/Fake/VerticalSimulationEngine.h"

namespace PiSubmarine::Drone::Server::Fake
{
    DepthProvider::DepthProvider(const VerticalSimulationEngine& simulationEngine) noexcept
        : m_SimulationEngine(simulationEngine)
    {
    }

    Error::Api::Result<Depth::Telemetry::Api::State> DepthProvider::GetState() const
    {
        return m_SimulationEngine.GetDepthState();
    }
}
