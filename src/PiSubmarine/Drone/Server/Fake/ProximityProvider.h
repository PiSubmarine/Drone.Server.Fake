#pragma once

#include "PiSubmarine/Proximity/Telemetry/Api/IProvider.h"

namespace PiSubmarine::Drone::Server::Fake
{
    class VerticalSimulationEngine;
}

namespace PiSubmarine::Drone::Server::Fake
{
    class ProximityProvider final : public Proximity::Telemetry::Api::IProvider
    {
    public:
        explicit ProximityProvider(const VerticalSimulationEngine& simulationEngine) noexcept;

        [[nodiscard]] Error::Api::Result<Proximity::Telemetry::Api::State> GetState() const override;

    private:
        const VerticalSimulationEngine& m_SimulationEngine;
    };
}
