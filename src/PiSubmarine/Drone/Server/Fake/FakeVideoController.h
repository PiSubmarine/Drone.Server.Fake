#pragma once

#include "PiSubmarine/Control/Video/Api/IController.h"

namespace PiSubmarine::Drone::Server::Fake
{
    class FakeVideoController final : public Control::Video::Api::IController
    {
    public:
        [[nodiscard]] Error::Api::Result<void> SetTarget(const Control::Video::Api::Command& target) override;

    private:
        Control::Video::Api::Command m_Target = Control::Video::Api::Command::Disable();
    };
}
