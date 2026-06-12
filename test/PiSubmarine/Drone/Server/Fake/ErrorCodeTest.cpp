#include <gtest/gtest.h>

#include "PiSubmarine/Drone/Server/Fake/ErrorCode.h"

namespace PiSubmarine::Drone::Server::Fake
{
    TEST(ErrorCodeTest, ConvertsToErrorCode)
    {
        const auto errorCode = make_error_code(ErrorCode::LeaseServerStartFailed);

        EXPECT_EQ(errorCode.value(), static_cast<int>(ErrorCode::LeaseServerStartFailed));
        EXPECT_STREQ(errorCode.category().name(), "PiSubmarine.Drone.Server.Fake");
    }
}
