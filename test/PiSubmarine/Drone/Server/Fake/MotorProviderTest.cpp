#include <gtest/gtest.h>

#include "PiSubmarine/Drone/Server/Fake/UnidirectionalMotor.h"

namespace PiSubmarine::Drone::Server::Fake
{
    TEST(UnidirectionalMotorTest, CyclesThroughDifferentStates)
    {
        UnidirectionalMotor provider;
        ASSERT_TRUE(provider.SetPowered(true).has_value());
        ASSERT_TRUE(provider.SetDutyCycle(NormalizedFraction(0.65)).has_value());

        provider.Tick(std::chrono::seconds(0), std::chrono::seconds(0));
        const auto initial = provider.GetState();
        ASSERT_TRUE(initial.has_value());

        provider.Tick(std::chrono::seconds(2), std::chrono::seconds(2));
        const auto degraded = provider.GetState();
        provider.Tick(std::chrono::seconds(4), std::chrono::seconds(2));
        const auto faulted = provider.GetState();

        ASSERT_TRUE(degraded.has_value());
        ASSERT_TRUE(faulted.has_value());
        EXPECT_NE(*initial, *degraded);
        EXPECT_NE(*degraded, *faulted);
        EXPECT_EQ(degraded->Operational, Motor::Telemetry::Api::OperationalState::Degraded);
        EXPECT_EQ(faulted->Operational, Motor::Telemetry::Api::OperationalState::Faulted);
        EXPECT_EQ(initial->Direction, Motor::Telemetry::Api::DriveDirection::Forward);
        EXPECT_EQ(initial->DriveEffort, NormalizedFraction(0.65));
        EXPECT_EQ(faulted->Direction, Motor::Telemetry::Api::DriveDirection::Idle);
        EXPECT_EQ(faulted->DriveEffort, NormalizedFraction(0));
    }
}
