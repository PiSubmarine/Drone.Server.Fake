#include "PiSubmarine/Drone/Server/Fake/GimbalController.h"

namespace PiSubmarine::Drone::Server::Fake
{
    Error::Api::Result<void> GimbalController::SetTarget(const Control::Gimbal::Api::Command& target)
    {
        m_Target = target;
        return {};
    }
}
