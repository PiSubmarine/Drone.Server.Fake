#pragma once

#include "PiSubmarine/Degrees.h"
#include "PiSubmarine/Servo/IController.h"

namespace PiSubmarine::Drone::Server::Fake
{
    class ServoController final : public ::PiSubmarine::Servo::IController
    {
    public:
        [[nodiscard]] Error::Api::Result<void> SetTargetAngle(Radians angle) override;
        [[nodiscard]] Radians GetTargetAngle() const override;
        [[nodiscard]] AngularSector GetAllowedTargetAngleSector() const override;
        [[nodiscard]] Error::Api::Result<void> SetEnabled(bool isEnabled) override;
        [[nodiscard]] Error::Api::Result<bool> IsEnabled() const override;

        Radians m_TargetAngle = Degrees{90.0}.ToRadians();
        bool m_IsEnabled = true;
    };
}
