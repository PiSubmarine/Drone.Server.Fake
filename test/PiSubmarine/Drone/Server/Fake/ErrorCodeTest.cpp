#include <gtest/gtest.h>

#include "PiSubmarine/Drone/Server/Fake/ErrorCode.h"

namespace PiSubmarine::Drone::Server::Fake
{
    TEST(ErrorCodeTest, ConvertsToErrorCode)
    {
        const auto errorCode = make_error_code(ErrorCode::GrpcServerStartFailed);

        EXPECT_EQ(errorCode.value(), static_cast<int>(ErrorCode::GrpcServerStartFailed));
        EXPECT_STREQ(errorCode.category().name(), "PiSubmarine.Drone.Server.Fake");
    }
}
