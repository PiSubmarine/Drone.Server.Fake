#include "PiSubmarine/Drone/Server/Fake/BidirectionalMotor.h"

namespace PiSubmarine::Drone::Server::Fake
{
    Error::Api::Result<void> BidirectionalMotor::SetPowered(const bool enabled)
    {
        m_IsPowered = enabled;
        if (!m_IsPowered)
        {
            m_DutyCycle = SignedNormalizedFraction{0.0};
        }

        return {};
    }

    Error::Api::Result<bool> BidirectionalMotor::IsPowered() const
    {
        return m_IsPowered;
    }

    Error::Api::Result<SignedNormalizedFraction> BidirectionalMotor::GetDutyCycle() const
    {
        return m_DutyCycle;
    }

    Error::Api::Result<void> BidirectionalMotor::SetDutyCycle(const SignedNormalizedFraction dutyCycle)
    {
        m_DutyCycle = dutyCycle;
        m_IsPowered = static_cast<double>(m_DutyCycle) != 0.0;
        return {};
    }

    Error::Api::Result<NormalizedFraction> BidirectionalMotor::GetForwardMinimalEffectiveDutyCycle() const
    {
        return m_ForwardMinimalEffectiveDutyCycle;
    }

    Error::Api::Result<NormalizedFraction> BidirectionalMotor::GetReverseMinimalEffectiveDutyCycle() const
    {
        return m_ReverseMinimalEffectiveDutyCycle;
    }
}
