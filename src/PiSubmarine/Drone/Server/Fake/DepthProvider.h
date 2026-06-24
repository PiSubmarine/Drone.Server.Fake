#pragma once

#include "PiSubmarine/Depth/Telemetry/Api/IProvider.h"

namespace PiSubmarine::Drone::Server::Fake
{
    class VerticalSimulationEngine;
}

namespace PiSubmarine::Drone::Server::Fake
{
    class DepthProvider final : public Depth::Telemetry::Api::IProvider
    {
    public:
        explicit DepthProvider(const VerticalSimulationEngine& simulationEngine) noexcept;

        [[nodiscard]] Error::Api::Result<Depth::Telemetry::Api::State> GetState() const override;

    private:
        const VerticalSimulationEngine& m_SimulationEngine;
    };
}
