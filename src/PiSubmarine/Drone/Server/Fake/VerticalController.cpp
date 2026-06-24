#include "PiSubmarine/Drone/Server/Fake/VerticalController.h"

namespace PiSubmarine::Drone::Server::Fake
{
    VerticalController::VerticalController(BallastProvider& ballastProvider) noexcept
        : m_BallastProvider(ballastProvider)
    {
    }

    Error::Api::Result<void> VerticalController::SetTarget(const Control::Vertical::Api::Command& target)
    {
        m_Target = target;
        if (const auto* setBallastPosition = target.TryGet<Control::Vertical::Api::Command::SetBallastPosition>())
        {
            m_BallastProvider.SetPosition(setBallastPosition->Position);
        }

        return {};
    }
}
