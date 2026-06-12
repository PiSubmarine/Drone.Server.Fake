#include "PiSubmarine/Drone/Server/Fake/FakeGimbalController.h"

namespace PiSubmarine::Drone::Server::Fake
{
    Error::Api::Result<void> FakeGimbalController::SetTarget(const Control::Gimbal::Api::Command& target)
    {
        m_Target = target;
        return {};
    }
}
