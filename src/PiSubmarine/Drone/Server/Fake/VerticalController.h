#pragma once

#include "PiSubmarine/Control/Vertical/Api/IController.h"

namespace PiSubmarine::Drone::Server::Fake
{
    class VerticalController final : public Control::Vertical::Api::IController
    {
    public:
        [[nodiscard]] Error::Api::Result<void> SetTarget(const Control::Vertical::Api::Command& target) override;

    private:
        Control::Vertical::Api::Command m_Target = Control::Vertical::Api::Command::KeepCurrentValue();
    };
}
