#include <gtest/gtest.h>

#include "PiSubmarine/Drone/Server/Fake/ServoController.h"
#include "PiSubmarine/Error/Api/ErrorCondition.h"

namespace PiSubmarine::Drone::Server::Fake
{
    TEST(ServoControllerTest, SetTargetAngleStoresAcceptedAngle)
    {
        ServoController controller;
        const auto targetAngle = Degrees{45.0}.ToRadians();

        ASSERT_TRUE(controller.SetTargetAngle(targetAngle).has_value());

        EXPECT_EQ(controller.GetTargetAngle(), targetAngle);
    }

    TEST(ServoControllerTest, SetTargetAngleRejectsAngleOutsideServoSector)
    {
        ServoController controller;

        const auto result = controller.SetTargetAngle(Degrees{181.0}.ToRadians());

        ASSERT_FALSE(result.has_value());
        EXPECT_EQ(result.error().Condition, Error::Api::ErrorCondition::ContractError);
    }

    TEST(ServoControllerTest, SetEnabledUpdatesEnabledState)
    {
        ServoController controller;

        ASSERT_TRUE(controller.SetEnabled(false).has_value());

        const auto isEnabled = controller.IsEnabled();
        ASSERT_TRUE(isEnabled.has_value());
        EXPECT_FALSE(isEnabled.value());
    }
}
