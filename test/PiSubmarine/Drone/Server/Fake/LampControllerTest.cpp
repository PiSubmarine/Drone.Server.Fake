#include <gtest/gtest.h>

#include "PiSubmarine/Control/Lamp/Api/Command.h"
#include "PiSubmarine/Drone/Server/Fake/LampController.h"

namespace PiSubmarine::Drone::Server::Fake
{
    TEST(LampControllerTest, ReflectsActiveTargetAndCyclesStatusFlags)
    {
        LampController controller;

        ASSERT_TRUE(controller.SetTarget(Control::Lamp::Api::Command::Create(NormalizedFraction{0.7})).has_value());

        controller.Tick(std::chrono::seconds(0), std::chrono::seconds(0));
        const auto initial = controller.GetStatus();
        ASSERT_TRUE(initial.has_value());
        EXPECT_EQ(initial->Intensity, NormalizedFraction{0.7});

        controller.Tick(std::chrono::seconds(2), std::chrono::seconds(2));
        const auto warning = controller.GetStatus();
        controller.Tick(std::chrono::seconds(4), std::chrono::seconds(2));
        const auto fault = controller.GetStatus();

        ASSERT_TRUE(warning.has_value());
        ASSERT_TRUE(fault.has_value());
        EXPECT_NE(*initial, *warning);
        EXPECT_NE(*warning, *fault);
        EXPECT_TRUE(warning->HasOvertemperatureWarning);
        EXPECT_TRUE(fault->HasOvercurrentFault);
    }
}
