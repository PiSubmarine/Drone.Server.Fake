#pragma once

#include "PiSubmarine/Control/Lamp/Api/IController.h"
#include "PiSubmarine/Lamp/Telemetry/Api/IProvider.h"

namespace PiSubmarine::Drone::Server::Fake
{
    class LampController final
        : public Control::Lamp::Api::IController
        , public Lamp::Telemetry::Api::IProvider
    {
    public:
        [[nodiscard]] Error::Api::Result<void> SetTarget(const Control::Lamp::Api::Command& target) override;
        [[nodiscard]] Error::Api::Result<Lamp::Telemetry::Api::Status> GetStatus() const override;

    private:
        Control::Lamp::Api::Command m_Target = Control::Lamp::Api::Command::Create(NormalizedFraction{0});
    };
}
