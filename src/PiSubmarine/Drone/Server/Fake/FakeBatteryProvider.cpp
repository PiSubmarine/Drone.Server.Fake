#include "PiSubmarine/Drone/Server/Fake/FakeBatteryProvider.h"

namespace PiSubmarine::Drone::Server::Fake
{
    Error::Api::Result<Battery::Telemetry::Api::State> FakeBatteryProvider::GetState() const
    {
        return Battery::Telemetry::Api::State{
            .PackVoltage = Volts{16.0},
            .ChargerVoltage = Volts{16.8},
            .PackCurrent = Amperes{-1.2},
            .ChargerCurrent = Amperes{0.0},
            .ChargerTemperature = Celsius{28.0},
            .PackTemperature = Celsius{27.0},
            .MonitorTemperature = Celsius{29.0},
            .RemainingCapacity = AmpereHours{4.5},
            .StateOfCharge = NormalizedFraction{0.72}};
    }
}
