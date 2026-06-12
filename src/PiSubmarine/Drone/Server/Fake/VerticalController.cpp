#include "PiSubmarine/Drone/Server/Fake/VerticalController.h"

namespace PiSubmarine::Drone::Server::Fake
{
    Error::Api::Result<void> VerticalController::SetTarget(const Control::Vertical::Api::Command& target)
    {
        m_Target = target;
        return {};
    }
}
