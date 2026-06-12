#include "PiSubmarine/Drone/Server/Fake/HorizontalController.h"

namespace PiSubmarine::Drone::Server::Fake
{
    Error::Api::Result<void> HorizontalController::SetTarget(const Control::Horizontal::Api::Command& target)
    {
        m_Target = target;
        return {};
    }
}
