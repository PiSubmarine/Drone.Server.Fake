#include "PiSubmarine/Drone/Server/Fake/ServoController.h"

#include "PiSubmarine/Error/Api/MakeError.h"

namespace PiSubmarine::Drone::Server::Fake
{
    namespace
    {
        constexpr AngularSector AllowedTargetAngleSector{
            Radians{0.0},
            Degrees{180.0}.ToRadians()
        };

        [[nodiscard]] constexpr bool IsWithinAllowedSector(const Radians angle) noexcept
        {
            return angle.Value >= AllowedTargetAngleSector.Start.Value &&
                angle.Value <=
                AllowedTargetAngleSector.Start.Value + AllowedTargetAngleSector.Sweep.Value;
        }
    }

    Error::Api::Result<void> ServoController::SetTargetAngle(const Radians angle)
    {
        if (!IsWithinAllowedSector(angle))
        {
            return std::unexpected(Error::Api::MakeError(Error::Api::ErrorCondition::ContractError));
        }

        m_TargetAngle = angle;
        return {};
    }

    Radians ServoController::GetTargetAngle() const
    {
        return m_TargetAngle;
    }

    AngularSector ServoController::GetAllowedTargetAngleSector() const
    {
        return AllowedTargetAngleSector;
    }

    Error::Api::Result<void> ServoController::SetEnabled(const bool isEnabled)
    {
        m_IsEnabled = isEnabled;
        return {};
    }

    Error::Api::Result<bool> ServoController::IsEnabled() const
    {
        return m_IsEnabled;
    }
}
