include("${PISUBMARINE_BUILD_SYSTEM}")

message("Adding library: ${PISUBMARINE_MODULE_NAME}")
add_library(${PISUBMARINE_MODULE_NAME}
        PiSubmarine/Drone/Server/Fake/ErrorCode.cpp
        PiSubmarine/Drone/Server/Fake/BallastProvider.cpp
        PiSubmarine/Drone/Server/Fake/BatteryProvider.cpp
        PiSubmarine/Drone/Server/Fake/DepthProvider.cpp
        PiSubmarine/Drone/Server/Fake/GimbalController.cpp
        PiSubmarine/Drone/Server/Fake/HorizontalController.cpp
        PiSubmarine/Drone/Server/Fake/LampController.cpp
        PiSubmarine/Drone/Server/Fake/LoggerFactory.cpp
        PiSubmarine/Drone/Server/Fake/MotorProvider.cpp
        PiSubmarine/Drone/Server/Fake/ProximityProvider.cpp
        PiSubmarine/Drone/Server/Fake/VerticalController.cpp
        PiSubmarine/Drone/Server/Fake/Runtime.cpp)
target_include_directories(${PISUBMARINE_MODULE_NAME}
        PUBLIC "../public"
        PRIVATE "./")
PiSubmarineInitTarget(${PISUBMARINE_MODULE_NAME})

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC spdlog::spdlog)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Error.Api.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Error.Api)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Control.Engine.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Control.Engine)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Control.Server.Udp.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Control.Server.Udp)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Control.Serialization.Protobuf.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Control.Serialization.Protobuf)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Control.Pilot.Manual.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Control.Pilot.Manual)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Control.Pilot.Dummy.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Control.Pilot.Dummy)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Control.Horizontal.Api.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Control.Horizontal.Api)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Control.Vertical.Api.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Control.Vertical.Api)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Control.Gimbal.Api.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Control.Gimbal.Api)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Control.Lamp.Api.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Control.Lamp.Api)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Control.Video.Api.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Control.Video.Api)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Lease.InMemory.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Lease.InMemory)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Grpc.Server.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Grpc.Server)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Lease.Server.Grpc.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Lease.Server.Grpc)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Logging.Api.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Logging.Api)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Telemetry.Server.Udp.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Telemetry.Server.Udp)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Telemetry.Channels.Api.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Telemetry.Channels.Api)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Security.Aead.Openssl.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Security.Aead.Openssl)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Security.Nonce.Openssl.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Security.Nonce.Openssl)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Battery.Telemetry.Protobuf.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Battery.Telemetry.Protobuf)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Battery.Telemetry.Api.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Battery.Telemetry.Api)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Ballast.Telemetry.Api.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Ballast.Telemetry.Api)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Ballast.Telemetry.Protobuf.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Ballast.Telemetry.Protobuf)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Depth.Telemetry.Api.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Depth.Telemetry.Api)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Depth.Telemetry.Protobuf.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Depth.Telemetry.Protobuf)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Lamp.Telemetry.Api.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Lamp.Telemetry.Api)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Lamp.Telemetry.Protobuf.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Lamp.Telemetry.Protobuf)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Motor.Telemetry.Api.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Motor.Telemetry.Api)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Motor.Telemetry.Protobuf.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Motor.Telemetry.Protobuf)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Proximity.Telemetry.Api.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Proximity.Telemetry.Api)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Proximity.Telemetry.Protobuf.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Proximity.Telemetry.Protobuf)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Time.Tickable.Api.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Time.Tickable.Api)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Time.Manager.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Time.Manager)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Time.Telemetry.Protobuf.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Time.Telemetry.Protobuf)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Udp.Asio.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Udp.Asio)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Video.Server.GStreamer.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Video.Server.GStreamer)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Video.Telemetry.Api.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Video.Telemetry.Api)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Video.Telemetry.Protobuf.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Video.Telemetry.Protobuf)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Video.Subscription.Grpc.Server.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Video.Subscription.Grpc.Server)
