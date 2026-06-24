#pragma once

#include "PiSubmarine/Ballast/BallastFillFraction.h"
#include "PiSubmarine/Ballast/Telemetry/Api/IProvider.h"

namespace PiSubmarine::Drone::Server::Fake
{
    class BallastProvider final
        : public Ballast::Telemetry::Api::IProvider
    {
    public:
        explicit BallastProvider(
            Ballast::BallastFillFraction initialPosition = Ballast::BallastFillFraction{NormalizedFraction{0.5}})
            noexcept;

        [[nodiscard]] Error::Api::Result<Ballast::Telemetry::Api::State> GetState() const override;
        void SetPosition(Ballast::BallastFillFraction position) noexcept;

    private:
        Ballast::Telemetry::Api::State m_State;
    };
}
