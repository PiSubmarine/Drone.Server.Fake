#include <gtest/gtest.h>

#include "PiSubmarine/Drone/Server/Fake/BallastProvider.h"
#include "PiSubmarine/Drone/Server/Fake/VerticalSimulationEngine.h"

namespace PiSubmarine::Drone::Server::Fake
{
    TEST(VerticalSimulationEngineTest, ReportsInitialDepthAndSeaFloorDistance)
    {
        BallastProvider ballastProvider(NormalizedFraction(0.5));
        VerticalSimulationEngine simulation(
            ballastProvider,
            VerticalSimulationConfig{
                .InitialDepth = 3.0_m,
                .SeaFloorDepth = 25.0_m});

        const auto depth = simulation.GetDepthState();
        const auto proximity = simulation.GetProximityState();

        ASSERT_TRUE(depth.Depth.has_value());
        ASSERT_TRUE(proximity.Distance.has_value());
        EXPECT_DOUBLE_EQ(depth.Depth->Value, 3.0);
        EXPECT_DOUBLE_EQ(proximity.Distance->Value, 22.0);
    }

    TEST(VerticalSimulationEngineTest, SinksWhenBallastIsAboveEquilibrium)
    {
        BallastProvider ballastProvider(NormalizedFraction(0.8));
        VerticalSimulationEngine simulation(
            ballastProvider,
            VerticalSimulationConfig{
                .InitialDepth = 5.0_m,
                .SeaFloorDepth = 25.0_m});

        for (int index = 0; index < 40; ++index)
        {
            simulation.Tick(std::chrono::milliseconds(index * 100), std::chrono::milliseconds(100));
        }

        const auto depth = simulation.GetDepthState();
        const auto proximity = simulation.GetProximityState();

        ASSERT_TRUE(depth.Depth.has_value());
        ASSERT_TRUE(proximity.Distance.has_value());
        EXPECT_GT(depth.Depth->Value, 5.0);
        EXPECT_LT(proximity.Distance->Value, 20.0);
    }

    TEST(VerticalSimulationEngineTest, RisesWhenBallastIsBelowEquilibrium)
    {
        BallastProvider ballastProvider(NormalizedFraction(0.2));
        VerticalSimulationEngine simulation(
            ballastProvider,
            VerticalSimulationConfig{
                .InitialDepth = 5.0_m,
                .SeaFloorDepth = 25.0_m});

        for (int index = 0; index < 40; ++index)
        {
            simulation.Tick(std::chrono::milliseconds(index * 100), std::chrono::milliseconds(100));
        }

        const auto depth = simulation.GetDepthState();
        const auto proximity = simulation.GetProximityState();

        ASSERT_TRUE(depth.Depth.has_value());
        ASSERT_TRUE(proximity.Distance.has_value());
        EXPECT_LT(depth.Depth->Value, 5.0);
        EXPECT_GT(proximity.Distance->Value, 20.0);
    }
}
