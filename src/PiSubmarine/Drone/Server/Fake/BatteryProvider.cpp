#include "PiSubmarine/Drone/Server/Fake/BatteryProvider.h"

#include <cmath>

namespace PiSubmarine::Drone::Server::Fake
{
    namespace
    {
        [[nodiscard]] double FractionalCycle(
            const std::chrono::nanoseconds& uptime,
            const std::chrono::duration<double>& period)
        {
            return std::fmod(std::chrono::duration<double>(uptime).count(), period.count()) / period.count();
        }
    }

    Error::Api::Result<Battery::Telemetry::Api::State> BatteryProvider::GetState() const
    {
        return m_State;
    }

    void BatteryProvider::Tick(const std::chrono::nanoseconds& uptime, const std::chrono::nanoseconds&)
    {
        const auto phase = FractionalCycle(uptime, std::chrono::seconds(12));
        m_State = Battery::Telemetry::Api::State{
            .PackVoltage = Volts{15.6 + (phase * 0.8)},
            .ChargerVoltage = Volts{16.8},
            .PackCurrent = Amperes{-1.8 + phase},
            .ChargerCurrent = Amperes{0.0},
            .ChargerTemperature = Celsius{28.0 + phase},
            .PackTemperature = Celsius{27.0 + (phase * 1.5)},
            .MonitorTemperature = Celsius{29.0 + (phase * 0.5)},
            .RemainingCapacity = AmpereHours{4.8 - (phase * 0.6)},
            .StateOfCharge = NormalizedFraction{0.8 - (phase * 0.2)}};
    }
}
