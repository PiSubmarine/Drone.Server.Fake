#include "PiSubmarine/Drone/Server/Fake/FakeVideoController.h"

namespace PiSubmarine::Drone::Server::Fake
{
    Error::Api::Result<void> FakeVideoController::SetTarget(const Control::Video::Api::Command& target)
    {
        m_Target = target;
        return {};
    }
}
