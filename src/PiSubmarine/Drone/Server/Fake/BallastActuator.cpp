#include "PiSubmarine/Drone/Server/Fake/BallastActuator.h"

#include <algorithm>
#include <chrono>

namespace PiSubmarine::Drone::Server::Fake
{
    BallastActuator::BallastActuator(
        Motor::Bidirectional::Api::IController& motorController,
        BallastProvider& ballastProvider) noexcept
        : BallastActuator(motorController, ballastProvider, Config{})
    {
    }

    BallastActuator::BallastActuator(
        Motor::Bidirectional::Api::IController& motorController,
        BallastProvider& ballastProvider,
        const Config& config) noexcept
        : m_MotorController(motorController)
        , m_BallastProvider(ballastProvider)
        , m_Config(config)
    {
    }

    void BallastActuator::Tick(const std::chrono::nanoseconds& uptime, const std::chrono::nanoseconds& deltaTime)
    {
        const auto deltaSeconds = std::chrono::duration<double>(deltaTime).count();
        if (deltaSeconds <= 0.0)
        {
            return;
        }

        const auto isPoweredResult = m_MotorController.IsPowered();
        if (!isPoweredResult.has_value() || !*isPoweredResult)
        {
            return;
        }

        const auto dutyCycleResult = m_MotorController.GetDutyCycle();
        if (!dutyCycleResult.has_value())
        {
            return;
        }

        const auto dutyCycle = static_cast<double>(*dutyCycleResult);
        if (dutyCycle == 0.0)
        {
            return;
        }

        const auto ballastStateResult = m_BallastProvider.GetState();
        if (!ballastStateResult.has_value() || !ballastStateResult->Position.has_value())
        {
            return;
        }

        const auto speedFactor = GetSpeedFactor(uptime);
        const auto deltaFill = dutyCycle * m_Config.MaximumFillFractionPerSecond * speedFactor * deltaSeconds;
        const auto updatedFill = std::clamp(
            static_cast<double>(*ballastStateResult->Position) + deltaFill,
            0.0,
            1.0);

        m_BallastProvider.SetPosition(::PiSubmarine::Ballast::BallastFillFraction{updatedFill});
    }

    double BallastActuator::GetSpeedFactor(const std::chrono::nanoseconds& uptime) const noexcept
    {
        const auto phase = (std::chrono::duration_cast<std::chrono::seconds>(uptime).count() / 3) % 4;
        switch (phase)
        {
        case 0:
            return 0.7;
        case 1:
            return 1.0;
        case 2:
            return 0.55;
        default:
            return 0.85;
        }
    }
}
