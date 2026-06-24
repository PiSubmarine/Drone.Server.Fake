#include "PiSubmarine/Drone/Server/Fake/VerticalSimulationEngine.h"

#include <algorithm>
#include <chrono>
#include <stdexcept>

namespace PiSubmarine::Drone::Server::Fake
{
    namespace
    {
        constexpr double GravityMetersPerSecondSquared = 9.81;
    }

    VerticalSimulationEngine::VerticalSimulationEngine(
        Ballast::Telemetry::Api::IProvider& ballastProvider,
        const VerticalSimulationConfig& config)
        : m_BallastProvider(ballastProvider)
        , m_Config(config)
        , m_DepthMeters(config.InitialDepth.Value)
    {
        if (m_Config.DroneMassKilograms <= 0.0)
        {
            throw std::invalid_argument("Simulation drone mass must be positive.");
        }

        if (m_Config.FrictionCoefficient < 0.0)
        {
            throw std::invalid_argument("Simulation friction coefficient must be non-negative.");
        }

        if (m_Config.BallastMaximumMassKilograms < 0.0)
        {
            throw std::invalid_argument("Simulation ballast maximum mass must be non-negative.");
        }

        if (m_Config.CargoMassKilograms < 0.0)
        {
            throw std::invalid_argument("Simulation cargo mass must be non-negative.");
        }

        if (m_Config.SeaFloorDepth.Value < 0.0)
        {
            throw std::invalid_argument("Simulation sea floor depth must be non-negative.");
        }

        if (m_Config.InitialDepth.Value < 0.0 || m_Config.InitialDepth.Value > m_Config.SeaFloorDepth.Value)
        {
            throw std::invalid_argument("Simulation initial depth must be within the water column.");
        }
    }

    Depth::Telemetry::Api::State VerticalSimulationEngine::GetDepthState() const noexcept
    {
        return Depth::Telemetry::Api::State{.Depth = Meters{m_DepthMeters}};
    }

    Proximity::Telemetry::Api::State VerticalSimulationEngine::GetProximityState() const noexcept
    {
        return Proximity::Telemetry::Api::State{
            .Distance = Meters{std::max(0.0, m_Config.SeaFloorDepth.Value - m_DepthMeters)}};
    }

    void VerticalSimulationEngine::Tick(const std::chrono::nanoseconds&, const std::chrono::nanoseconds& deltaTime)
    {
        const auto deltaSeconds = std::chrono::duration<double>(deltaTime).count();
        if (deltaSeconds <= 0.0)
        {
            return;
        }

        if (m_DepthMeters <= 0.1 && m_HasCargo)
        {
            m_HasCargo = false;
        }
        if (m_DepthMeters >= m_Config.SeaFloorDepth.Value - 0.1 && !m_HasCargo)
        {
            m_HasCargo = true;
        }

        const auto ballastPosition = GetBallastPosition();
        const auto ballastMassKilograms = m_Config.BallastMaximumMassKilograms * ballastPosition;
        const auto cargoMassKilograms = GetActiveCargoMassKilograms();
        const auto equilibriumBallastMassKilograms =
            m_Config.BallastMaximumMassKilograms * static_cast<double>(m_Config.EquilibriumBallastPosition);
        const auto buoyancyOffsetForceNewtons =
            (ballastMassKilograms + cargoMassKilograms - equilibriumBallastMassKilograms) *
            GravityMetersPerSecondSquared;
        const auto totalMassKilograms =
            m_Config.DroneMassKilograms + ballastMassKilograms + cargoMassKilograms;
        const auto accelerationMetersPerSecondSquared =
            (buoyancyOffsetForceNewtons / totalMassKilograms) -
            (m_Config.FrictionCoefficient * m_VerticalSpeedMetersPerSecond);

        m_VerticalSpeedMetersPerSecond += accelerationMetersPerSecondSquared * deltaSeconds;
        m_DepthMeters += m_VerticalSpeedMetersPerSecond * deltaSeconds;
        ClampToPhysicalBounds();
    }

    double VerticalSimulationEngine::GetBallastPosition() const
    {
        const auto ballastState = m_BallastProvider.GetState();
        if (!ballastState.has_value() || !ballastState->Position.has_value())
        {
            return static_cast<double>(m_Config.EquilibriumBallastPosition);
        }

        return static_cast<double>(*ballastState->Position);
    }

    double VerticalSimulationEngine::GetActiveCargoMassKilograms() const noexcept
    {
        return m_HasCargo ? m_Config.CargoMassKilograms : 0.0;
    }

    void VerticalSimulationEngine::ClampToPhysicalBounds() noexcept
    {
        if (m_DepthMeters <= 0.0)
        {
            m_DepthMeters = 0.0;
            m_VerticalSpeedMetersPerSecond = std::max(0.0, m_VerticalSpeedMetersPerSecond);
            return;
        }

        if (m_DepthMeters >= m_Config.SeaFloorDepth.Value)
        {
            m_DepthMeters = m_Config.SeaFloorDepth.Value;
            m_VerticalSpeedMetersPerSecond = std::min(0.0, m_VerticalSpeedMetersPerSecond);
        }
    }
}
