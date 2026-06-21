#pragma once

#include "PiSubmarine/Ballast/Telemetry/Api/IProvider.h"

namespace PiSubmarine::Drone::Server::Fake
{
    class BallastProvider final : public Ballast::Telemetry::Api::IProvider
    {
    public:
        [[nodiscard]] Error::Api::Result<Ballast::Telemetry::Api::State> GetState() const override;

    private:
        mutable int m_Tick = 0;
    };
}
