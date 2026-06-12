#include <gtest/gtest.h>

#include "PiSubmarine/Drone/Server/Fake/ErrorCode.h"
#include "PiSubmarine/Drone/Server/Fake/Runtime.h"

namespace PiSubmarine::Drone::Server::Fake
{
    TEST(RuntimeTest, IsNotRunningBeforeRun)
    {
        Runtime runtime(Runtime::Config{});

        EXPECT_FALSE(runtime.IsRunning());
    }

    TEST(RuntimeTest, RunFailsWhenLeaseTlsConfigurationIsMissing)
    {
        Runtime runtime(Runtime::Config{});

        const auto result = runtime.Run();

        ASSERT_FALSE(result.has_value());
        EXPECT_EQ(result.error().Cause, make_error_code(ErrorCode::LeaseServerStartFailed));
        EXPECT_FALSE(runtime.IsRunning());
    }
}
