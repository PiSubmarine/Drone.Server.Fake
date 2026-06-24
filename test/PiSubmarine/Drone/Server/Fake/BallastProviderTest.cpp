#include <gtest/gtest.h>

#include "PiSubmarine/Ballast/BallastFillFraction.h"
#include "PiSubmarine/Drone/Server/Fake/BallastProvider.h"
#include "PiSubmarine/Drone/Server/Fake/VerticalController.h"
#include "PiSubmarine/NormalizedFraction.h"

namespace PiSubmarine::Drone::Server::Fake
{
    TEST(BallastProviderTest, UsesInitialPosition)
    {
        BallastProvider provider(Ballast::BallastFillFraction{NormalizedFraction{0.35}});

        const auto state = provider.GetState();

        ASSERT_TRUE(state.has_value());
        ASSERT_TRUE(state->Position.has_value());
        EXPECT_EQ(*state->Position, Ballast::BallastFillFraction{NormalizedFraction{0.35}});
    }

    TEST(BallastProviderTest, VerticalControllerUpdatesBallastPositionCommand)
    {
        BallastProvider provider(Ballast::BallastFillFraction{NormalizedFraction{0.5}});
        VerticalController controller(provider);

        ASSERT_TRUE(controller.SetTarget(Control::Vertical::Api::Command::SetBallastPositionTo(
            Ballast::BallastFillFraction{NormalizedFraction{0.8}})).has_value());

        const auto state = provider.GetState();
        ASSERT_TRUE(state.has_value());
        ASSERT_TRUE(state->Position.has_value());
        EXPECT_EQ(*state->Position, Ballast::BallastFillFraction{NormalizedFraction{0.8}});
    }
}
