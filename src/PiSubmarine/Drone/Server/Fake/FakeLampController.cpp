#include "PiSubmarine/Drone/Server/Fake/FakeLampController.h"

namespace PiSubmarine::Drone::Server::Fake
{
    Error::Api::Result<void> FakeLampController::SetTarget(const Control::Lamp::Api::Command& target)
    {
        m_Target = target;
        return {};
    }
}
