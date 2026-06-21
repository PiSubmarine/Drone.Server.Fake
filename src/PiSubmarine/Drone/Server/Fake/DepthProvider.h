#pragma once

#include "PiSubmarine/Depth/Telemetry/Api/IProvider.h"

namespace PiSubmarine::Drone::Server::Fake
{
    class DepthProvider final : public Depth::Telemetry::Api::IProvider
    {
    public:
        [[nodiscard]] Error::Api::Result<Depth::Telemetry::Api::State> GetState() const override;

    private:
        mutable int m_Tick = 0;
    };
}
