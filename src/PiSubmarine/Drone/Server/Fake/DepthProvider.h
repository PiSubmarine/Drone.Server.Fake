#pragma once

#include "PiSubmarine/Depth/Telemetry/Api/IProvider.h"
#include "PiSubmarine/Time/ITickable.h"

namespace PiSubmarine::Drone::Server::Fake
{
    class DepthProvider final
        : public Depth::Telemetry::Api::IProvider
        , public Time::ITickable
    {
    public:
        [[nodiscard]] Error::Api::Result<Depth::Telemetry::Api::State> GetState() const override;
        void Tick(const std::chrono::nanoseconds& uptime, const std::chrono::nanoseconds& deltaTime) override;

    private:
        Depth::Telemetry::Api::State m_State{.Depth = Meters{1.0}};
    };
}
