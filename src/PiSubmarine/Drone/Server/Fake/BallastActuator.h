#pragma once

#include "PiSubmarine/Drone/Server/Fake/BallastProvider.h"
#include "PiSubmarine/Motor/Bidirectional/Api/IController.h"
#include "PiSubmarine/Time/ITickable.h"

namespace PiSubmarine::Drone::Server::Fake
{
    class BallastActuator final : public Time::ITickable
    {
    public:
        struct Config
        {
            double MaximumFillFractionPerSecond = 0.1;
        };

        BallastActuator(
            Motor::Bidirectional::Api::IController& motorController,
            BallastProvider& ballastProvider,
            const Config& config) noexcept;

        BallastActuator(
            Motor::Bidirectional::Api::IController& motorController,
            BallastProvider& ballastProvider) noexcept;

        void Tick(const std::chrono::nanoseconds& uptime, const std::chrono::nanoseconds& deltaTime) override;

    private:
        [[nodiscard]] double GetSpeedFactor(const std::chrono::nanoseconds& uptime) const noexcept;

        Motor::Bidirectional::Api::IController& m_MotorController;
        BallastProvider& m_BallastProvider;
        Config m_Config;
    };
}
