#pragma once

#include "PiSubmarine/Motor/Telemetry/Api/IProvider.h"
#include "PiSubmarine/Time/ITickable.h"

namespace PiSubmarine::Drone::Server::Fake
{
    class MotorProvider final
        : public Motor::Telemetry::Api::IProvider
        , public Time::ITickable
    {
    public:
        [[nodiscard]] Error::Api::Result<Motor::Telemetry::Api::State> GetState() const override;
        void Tick(const std::chrono::nanoseconds& uptime, const std::chrono::nanoseconds& deltaTime) override;

    private:
        Motor::Telemetry::Api::State m_State{
            .Operational = Motor::Telemetry::Api::OperationalState::Operational,
            .ActiveFaults = static_cast<Motor::Telemetry::Api::Faults>(0),
            .ActiveWarnings = static_cast<Motor::Telemetry::Api::Warnings>(0)};
    };
}
