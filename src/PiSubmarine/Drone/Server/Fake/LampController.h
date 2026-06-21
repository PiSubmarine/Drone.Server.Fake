#pragma once

#include "PiSubmarine/Control/Lamp/Api/IController.h"
#include "PiSubmarine/Lamp/Telemetry/Api/IProvider.h"
#include "PiSubmarine/Time/ITickable.h"

namespace PiSubmarine::Drone::Server::Fake
{
    class LampController final
        : public Control::Lamp::Api::IController
        , public Lamp::Telemetry::Api::IProvider
        , public Time::ITickable
    {
    public:
        [[nodiscard]] Error::Api::Result<void> SetTarget(const Control::Lamp::Api::Command& target) override;
        [[nodiscard]] Error::Api::Result<Lamp::Telemetry::Api::Status> GetStatus() const override;
        void Tick(const std::chrono::nanoseconds& uptime, const std::chrono::nanoseconds& deltaTime) override;

    private:
        Control::Lamp::Api::Command m_Target = Control::Lamp::Api::Command::Create(NormalizedFraction{0});
        Lamp::Telemetry::Api::Status m_Status{};
    };
}
