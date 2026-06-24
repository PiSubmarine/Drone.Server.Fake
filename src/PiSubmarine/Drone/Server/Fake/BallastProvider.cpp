#include "PiSubmarine/Drone/Server/Fake/BallastProvider.h"

namespace PiSubmarine::Drone::Server::Fake
{
    BallastProvider::BallastProvider(const NormalizedFraction initialPosition) noexcept
        : m_State(Ballast::Telemetry::Api::State{.Position = initialPosition})
    {
    }

    Error::Api::Result<Ballast::Telemetry::Api::State> BallastProvider::GetState() const
    {
        return m_State;
    }

    void BallastProvider::SetPosition(const NormalizedFraction position) noexcept
    {
        m_State = Ballast::Telemetry::Api::State{.Position = position};
    }
}
