#pragma once
#include "engine/components/Component.h"

class CLifespan : public Component
{
public:
    float remainingSeconds_ = 0.0f;
    float lifespanSeconds_ = 0.0f;
    
    CLifespan() = default;
    CLifespan(float lifeSpanSeconds)
    {
        this->lifespanSeconds_ = lifeSpanSeconds;
        remainingSeconds_ = lifespanSeconds_;
    }
};