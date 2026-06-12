#pragma once

#include "PiSubmarine/Control/Gimbal/Api/IController.h"

namespace PiSubmarine::Drone::Server::Fake
{
    class GimbalController final : public Control::Gimbal::Api::IController
    {
    public:
        [[nodiscard]] Error::Api::Result<void> SetTarget(const Control::Gimbal::Api::Command& target) override;

    private:
        Control::Gimbal::Api::Command m_Target = Control::Gimbal::Api::Command::Create(Radians{0});
    };
}
