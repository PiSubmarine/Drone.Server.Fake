#include <gtest/gtest.h>

#include "PiSubmarine/Drone/Server/Fake/BidirectionalMotor.h"

namespace PiSubmarine::Drone::Server::Fake
{
    TEST(BidirectionalMotorTest, StoresSignedDutyCycleAndPowerState)
    {
        BidirectionalMotor motor;

        ASSERT_TRUE(motor.SetDutyCycle(SignedNormalizedFraction{-0.4}).has_value());

        const auto powered = motor.IsPowered();
        const auto dutyCycle = motor.GetDutyCycle();

        ASSERT_TRUE(powered.has_value());
        ASSERT_TRUE(dutyCycle.has_value());
        EXPECT_TRUE(*powered);
        EXPECT_DOUBLE_EQ(static_cast<double>(*dutyCycle), -0.4);
    }

    TEST(BidirectionalMotorTest, PoweringDownResetsDutyCycle)
    {
        BidirectionalMotor motor;

        ASSERT_TRUE(motor.SetDutyCycle(SignedNormalizedFraction{0.3}).has_value());
        ASSERT_TRUE(motor.SetPowered(false).has_value());

        const auto powered = motor.IsPowered();
        const auto dutyCycle = motor.GetDutyCycle();

        ASSERT_TRUE(powered.has_value());
        ASSERT_TRUE(dutyCycle.has_value());
        EXPECT_FALSE(*powered);
        EXPECT_DOUBLE_EQ(static_cast<double>(*dutyCycle), 0.0);
    }
}
