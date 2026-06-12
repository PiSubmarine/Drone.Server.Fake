#include "PiSubmarine/Drone/Server/Fake/FakeHorizontalController.h"

namespace PiSubmarine::Drone::Server::Fake
{
    Error::Api::Result<void> FakeHorizontalController::SetTarget(const Control::Horizontal::Api::Command& target)
    {
        m_Target = target;
        return {};
    }
}
