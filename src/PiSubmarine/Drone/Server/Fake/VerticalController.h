#pragma once

#include "PiSubmarine/Drone/Server/Fake/BallastProvider.h"
#include "PiSubmarine/Control/Vertical/Api/IController.h"

namespace PiSubmarine::Drone::Server::Fake
{
    class VerticalController final : public Control::Vertical::Api::IController
    {
    public:
        explicit VerticalController(BallastProvider& ballastProvider) noexcept;

        [[nodiscard]] Error::Api::Result<void> SetTarget(const Control::Vertical::Api::Command& target) override;

    private:
        BallastProvider& m_BallastProvider;
        Control::Vertical::Api::Command m_Target = Control::Vertical::Api::Command::KeepCurrentValue();
    };
}
