#include "PiSubmarine/Drone/Server/Fake/FakeVerticalController.h"

namespace PiSubmarine::Drone::Server::Fake
{
    Error::Api::Result<void> FakeVerticalController::SetTarget(const Control::Vertical::Api::Command& target)
    {
        m_Target = target;
        return {};
    }
}
