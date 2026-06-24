#include "PiSubmarine/Drone/Server/Fake/ProximityProvider.h"
#include "PiSubmarine/Drone/Server/Fake/VerticalSimulationEngine.h"

namespace PiSubmarine::Drone::Server::Fake
{
    ProximityProvider::ProximityProvider(const VerticalSimulationEngine& simulationEngine) noexcept
        : m_SimulationEngine(simulationEngine)
    {
    }

    Error::Api::Result<Proximity::Telemetry::Api::State> ProximityProvider::GetState() const
    {
        return m_SimulationEngine.GetProximityState();
    }
}
