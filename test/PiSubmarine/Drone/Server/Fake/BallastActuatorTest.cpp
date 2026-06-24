#include <gtest/gtest.h>

#include "PiSubmarine/Drone/Server/Fake/BallastActuator.h"
#include "PiSubmarine/Drone/Server/Fake/BallastProvider.h"
#include "PiSubmarine/Drone/Server/Fake/BidirectionalMotor.h"

namespace PiSubmarine::Drone::Server::Fake
{
    TEST(BallastActuatorTest, IncreasesBallastFillForPositiveDutyCycle)
    {
        BallastProvider ballastProvider(Ballast::BallastFillFraction{0.5});
        BidirectionalMotor motor;
        BallastActuator actuator(motor, ballastProvider);

        ASSERT_TRUE(motor.SetDutyCycle(SignedNormalizedFraction{0.5}).has_value());

        actuator.Tick(std::chrono::seconds{0}, std::chrono::seconds{1});

        const auto state = ballastProvider.GetState();
        ASSERT_TRUE(state.has_value());
        ASSERT_TRUE(state->Position.has_value());
        EXPECT_GT(static_cast<double>(*state->Position), 0.5);
    }

    TEST(BallastActuatorTest, DecreasesBallastFillForNegativeDutyCycle)
    {
        BallastProvider ballastProvider(Ballast::BallastFillFraction{0.5});
        BidirectionalMotor motor;
        BallastActuator actuator(motor, ballastProvider);

        ASSERT_TRUE(motor.SetDutyCycle(SignedNormalizedFraction{-0.5}).has_value());

        actuator.Tick(std::chrono::seconds{0}, std::chrono::seconds{1});

        const auto state = ballastProvider.GetState();
        ASSERT_TRUE(state.has_value());
        ASSERT_TRUE(state->Position.has_value());
        EXPECT_LT(static_cast<double>(*state->Position), 0.5);
    }
}
