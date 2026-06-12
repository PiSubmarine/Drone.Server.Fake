#pragma once

#include "PiSubmarine/Control/Lamp/Api/IController.h"

namespace PiSubmarine::Drone::Server::Fake
{
    class FakeLampController final : public Control::Lamp::Api::IController
    {
    public:
        [[nodiscard]] Error::Api::Result<void> SetTarget(const Control::Lamp::Api::Command& target) override;

    private:
        Control::Lamp::Api::Command m_Target = Control::Lamp::Api::Command::Create(NormalizedFraction{0});
    };
}
