#pragma once

#include "PiSubmarine/Motor/Telemetry/Api/IProvider.h"

namespace PiSubmarine::Drone::Server::Fake
{
    class FakeMotorProvider final : public Motor::Telemetry::Api::IProvider
    {
    public:
        [[nodiscard]] Error::Api::Result<Motor::Telemetry::Api::State> GetState() const override;
    };
}
