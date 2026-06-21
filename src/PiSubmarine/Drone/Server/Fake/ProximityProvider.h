#pragma once

#include "PiSubmarine/Proximity/Telemetry/Api/IProvider.h"

namespace PiSubmarine::Drone::Server::Fake
{
    class ProximityProvider final : public Proximity::Telemetry::Api::IProvider
    {
    public:
        [[nodiscard]] Error::Api::Result<Proximity::Telemetry::Api::State> GetState() const override;

    private:
        mutable int m_Tick = 0;
    };
}
