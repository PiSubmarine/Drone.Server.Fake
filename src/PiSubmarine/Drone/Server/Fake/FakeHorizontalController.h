#pragma once

#include "PiSubmarine/Control/Horizontal/Api/IController.h"

namespace PiSubmarine::Drone::Server::Fake
{
    class FakeHorizontalController final : public Control::Horizontal::Api::IController
    {
    public:
        [[nodiscard]] Error::Api::Result<void> SetTarget(const Control::Horizontal::Api::Command& target) override;

    private:
        Control::Horizontal::Api::Command m_Target = Control::Horizontal::Api::Command::Create(0, 0).value();
    };
}
