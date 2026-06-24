#pragma once

#include "PiSubmarine/Motor/Bidirectional/Api/IController.h"

namespace PiSubmarine::Drone::Server::Fake
{
    class BidirectionalMotor final : public Motor::Bidirectional::Api::IController
    {
    public:
        [[nodiscard]] Error::Api::Result<void> SetPowered(bool enabled) override;
        [[nodiscard]] Error::Api::Result<bool> IsPowered() const override;
        [[nodiscard]] Error::Api::Result<SignedNormalizedFraction> GetDutyCycle() const override;
        [[nodiscard]] Error::Api::Result<void> SetDutyCycle(SignedNormalizedFraction dutyCycle) override;
        [[nodiscard]] Error::Api::Result<NormalizedFraction> GetForwardMinimalEffectiveDutyCycle() const override;
        [[nodiscard]] Error::Api::Result<NormalizedFraction> GetReverseMinimalEffectiveDutyCycle() const override;

    private:
        bool m_IsPowered = false;
        SignedNormalizedFraction m_DutyCycle{0.0};
        NormalizedFraction m_ForwardMinimalEffectiveDutyCycle{0.15};
        NormalizedFraction m_ReverseMinimalEffectiveDutyCycle{0.17};
    };
}
