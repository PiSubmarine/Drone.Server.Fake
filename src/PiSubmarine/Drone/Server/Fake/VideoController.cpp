#include "PiSubmarine/Drone/Server/Fake/VideoController.h"

namespace PiSubmarine::Drone::Server::Fake
{
    Error::Api::Result<void> VideoController::SetTarget(const Control::Video::Api::Command& target)
    {
        m_Target = target;
        return {};
    }
}
