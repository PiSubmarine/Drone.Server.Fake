#include <gtest/gtest.h>

#include "PiSubmarine/Drone/Server/Fake/BatteryProvider.h"

namespace PiSubmarine::Drone::Server::Fake
{
    TEST(BatteryProviderTest, ReturnsChangingTelemetryAcrossReads)
    {
        BatteryProvider provider;

        provider.Tick(std::chrono::seconds(0), std::chrono::seconds(0));
        const auto first = provider.GetState();
        provider.Tick(std::chrono::seconds(6), std::chrono::seconds(6));
        const auto second = provider.GetState();

        ASSERT_TRUE(first.has_value());
        ASSERT_TRUE(second.has_value());
        EXPECT_NE(first->PackVoltage.Value, second->PackVoltage.Value);
        EXPECT_NE(first->StateOfCharge, second->StateOfCharge);
    }
}
