#include <gtest/gtest.h>

#include "PiSubmarine/Drone/Server/Fake/BallastProvider.h"
#include "PiSubmarine/Drone/Server/Fake/VerticalController.h"

namespace PiSubmarine::Drone::Server::Fake
{
    TEST(BallastProviderTest, UsesInitialPosition)
    {
        BallastProvider provider(NormalizedFraction(0.35));

        const auto state = provider.GetState();

        ASSERT_TRUE(state.has_value());
        ASSERT_TRUE(state->Position.has_value());
        EXPECT_EQ(*state->Position, NormalizedFraction(0.35));
    }

    TEST(BallastProviderTest, VerticalControllerUpdatesBallastPositionCommand)
    {
        BallastProvider provider(NormalizedFraction(0.5));
        VerticalController controller(provider);

        ASSERT_TRUE(controller.SetTarget(Control::Vertical::Api::Command::SetBallastPositionTo(
            NormalizedFraction(0.8))).has_value());

        const auto state = provider.GetState();
        ASSERT_TRUE(state.has_value());
        ASSERT_TRUE(state->Position.has_value());
        EXPECT_EQ(*state->Position, NormalizedFraction(0.8));
    }
}
