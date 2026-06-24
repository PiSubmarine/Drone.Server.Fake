#pragma once

#include "PiSubmarine/Ballast/BallastFillFraction.h"
#include "PiSubmarine/Ballast/Telemetry/Api/IProvider.h"
#include "PiSubmarine/Depth/Telemetry/Api/State.h"
#include "PiSubmarine/Proximity/Telemetry/Api/State.h"
#include "PiSubmarine/Time/ITickable.h"

namespace PiSubmarine::Drone::Server::Fake
{
    struct VerticalSimulationConfig
    {
        double DroneMassKilograms = 8.0;
        double FrictionCoefficient = 1.0;
        double BallastMaximumMassKilograms = 0.3;
        Ballast::BallastFillFraction EquilibriumBallastPosition = Ballast::BallastFillFraction{NormalizedFraction{0.5}};
        Meters InitialDepth = 1.0_m;
        Meters SeaFloorDepth = 25.0_m;
        double CargoMassKilograms = 0.1;
    };

    class VerticalSimulationEngine final : public Time::ITickable
    {
    public:
        VerticalSimulationEngine(
            Ballast::Telemetry::Api::IProvider& ballastProvider,
            const VerticalSimulationConfig& config);

        [[nodiscard]] Depth::Telemetry::Api::State GetDepthState() const noexcept;
        [[nodiscard]] Proximity::Telemetry::Api::State GetProximityState() const noexcept;
        void Tick(const std::chrono::nanoseconds& uptime, const std::chrono::nanoseconds& deltaTime) override;

    private:
        [[nodiscard]] double GetBallastPosition() const;
        [[nodiscard]] double GetActiveCargoMassKilograms() const noexcept;
        void ClampToPhysicalBounds() noexcept;

        Ballast::Telemetry::Api::IProvider& m_BallastProvider;
        VerticalSimulationConfig m_Config;
        double m_DepthMeters;
        double m_VerticalSpeedMetersPerSecond = 0.0;

        bool m_HasCargo = false;
    };
}
