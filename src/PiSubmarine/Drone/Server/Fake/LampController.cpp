#include "PiSubmarine/Drone/Server/Fake/LampController.h"

namespace PiSubmarine::Drone::Server::Fake
{
    Error::Api::Result<void> LampController::SetTarget(const Control::Lamp::Api::Command& target)
    {
        m_Target = target;
        return {};
    }
}
