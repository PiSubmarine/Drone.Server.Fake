#pragma once

#include "PiSubmarine/Battery/Telemetry/Api/IProvider.h"
#include "PiSubmarine/Time/ITickable.h"

namespace PiSubmarine::Drone::Server::Fake
{
    class BatteryProvider final
        : public Battery::Telemetry::Api::IProvider
        , public Time::ITickable
    {
    public:
        [[nodiscard]] Error::Api::Result<Battery::Telemetry::Api::State> GetState() const override;
        void Tick(const std::chrono::nanoseconds& uptime, const std::chrono::nanoseconds& deltaTime) override;

    private:
        Battery::Telemetry::Api::State m_State{
            .PackVoltage = Volts{15.6},
            .ChargerVoltage = Volts{16.8},
            .PackCurrent = Amperes{-1.8},
            .ChargerCurrent = Amperes{0.0},
            .ChargerTemperature = Celsius{28.0},
            .PackTemperature = Celsius{27.0},
            .MonitorTemperature = Celsius{29.0},
            .RemainingCapacity = AmpereHours{4.8},
            .StateOfCharge = NormalizedFraction{0.8}};
    };
}
