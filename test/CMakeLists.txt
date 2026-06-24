cmake_minimum_required(VERSION 3.20)

include(FetchContent)

project(${PISUBMARINE_MODULE_NAME}.Test LANGUAGES CXX)

set(${PISUBMARINE_MODULE_NAME}.Test.Sources
        "PiSubmarine/Drone/Server/Fake/BallastProviderTest.cpp"
        "PiSubmarine/Drone/Server/Fake/BatteryProviderTest.cpp"
        "PiSubmarine/Drone/Server/Fake/LampControllerTest.cpp"
        "PiSubmarine/Drone/Server/Fake/MotorProviderTest.cpp"
        "PiSubmarine/Drone/Server/Fake/ErrorCodeTest.cpp"
        "PiSubmarine/Drone/Server/Fake/RuntimeTest.cpp"
        "PiSubmarine/Drone/Server/Fake/VerticalSimulationEngineTest.cpp")

enable_testing()

find_package(GTest)

add_executable(${PISUBMARINE_MODULE_NAME}.Test ${${PISUBMARINE_MODULE_NAME}.Test.Sources})
PiSubmarineInitTarget(${PISUBMARINE_MODULE_NAME}.Test)
target_include_directories(${PISUBMARINE_MODULE_NAME}.Test PRIVATE "../src")

target_link_libraries(${PISUBMARINE_MODULE_NAME}.Test PRIVATE ${PISUBMARINE_MODULE_NAME})
target_link_libraries(${PISUBMARINE_MODULE_NAME}.Test PRIVATE GTest::gtest_main)
include(GoogleTest)
if(CMAKE_CROSSCOMPILING)
    gtest_add_tests(TARGET ${PISUBMARINE_MODULE_NAME}.Test)
else()
    gtest_discover_tests(${PISUBMARINE_MODULE_NAME}.Test)
endif()
