#pragma once

#include "PiSubmarine/Battery/Telemetry/Api/IProvider.h"

namespace PiSubmarine::Drone::Server::Fake
{
    class FakeBatteryProvider final : public Battery::Telemetry::Api::IProvider
    {
    public:
        [[nodiscard]] Error::Api::Result<Battery::Telemetry::Api::State> GetState() const override;
    };
}
