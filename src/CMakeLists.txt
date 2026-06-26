include("${PISUBMARINE_BUILD_SYSTEM}")

message("Adding library: ${PISUBMARINE_MODULE_NAME}")
add_library(${PISUBMARINE_MODULE_NAME}
        PiSubmarine/Drone/Server/Fake/BallastActuator.cpp
        PiSubmarine/Drone/Server/Fake/BallastActuator.h
        PiSubmarine/Drone/Server/Fake/BallastProvider.cpp
        PiSubmarine/Drone/Server/Fake/BatteryProvider.cpp
        PiSubmarine/Drone/Server/Fake/BidirectionalMotor.cpp
        PiSubmarine/Drone/Server/Fake/BidirectionalMotor.h
        PiSubmarine/Drone/Server/Fake/DepthProvider.cpp
        PiSubmarine/Drone/Server/Fake/LampController.cpp
        PiSubmarine/Drone/Server/Fake/LoggerFactory.cpp
        PiSubmarine/Drone/Server/Fake/ProximityProvider.cpp
        PiSubmarine/Drone/Server/Fake/VerticalSimulationEngine.cpp
        PiSubmarine/Drone/Server/Fake/ServoController.cpp
        PiSubmarine/Drone/Server/Fake/ServoController.h
        PiSubmarine/Drone/Server/Fake/UnidirectionalMotor.cpp
        PiSubmarine/Drone/Server/Fake/UnidirectionalMotor.h)
target_include_directories(${PISUBMARINE_MODULE_NAME}
        PUBLIC "../public"
        PRIVATE "./")
PiSubmarineInitTarget(${PISUBMARINE_MODULE_NAME})

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC spdlog::spdlog)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Drone.Server.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Drone.Server)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Error.Api.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Error.Api)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Control.Lamp.Api.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Control.Lamp.Api)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Logging.Api.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Logging.Api)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Battery.Telemetry.Api.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Battery.Telemetry.Api)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Ballast.Telemetry.Api.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Ballast.Telemetry.Api)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Ballast.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Ballast)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Depth.Telemetry.Api.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Depth.Telemetry.Api)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Lamp.Telemetry.Api.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Lamp.Telemetry.Api)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Motor.Telemetry.Api.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Motor.Telemetry.Api)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Motor.Bidirectional.Api.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Motor.Bidirectional.Api)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Proximity.Telemetry.Api.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Proximity.Telemetry.Api)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Time.Tickable.Api.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Time.Tickable.Api)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Servo.Api.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Servo.Api)

PiSubmarineAddDependency("https://github.com/PiSubmarine/Motor.Unidirectional.Api.git" "")
target_link_libraries(${PISUBMARINE_MODULE_NAME} PUBLIC PiSubmarine.Motor.Unidirectional.Api)
