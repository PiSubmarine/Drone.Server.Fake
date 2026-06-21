#pragma once

#include "PiSubmarine/Ballast/Telemetry/Api/IProvider.h"
#include "PiSubmarine/Time/ITickable.h"

namespace PiSubmarine::Drone::Server::Fake
{
    class BallastProvider final
        : public Ballast::Telemetry::Api::IProvider
        , public Time::ITickable
    {
    public:
        [[nodiscard]] Error::Api::Result<Ballast::Telemetry::Api::State> GetState() const override;
        void Tick(const std::chrono::nanoseconds& uptime, const std::chrono::nanoseconds& deltaTime) override;

    private:
        Ballast::Telemetry::Api::State m_State{};
    };
}
