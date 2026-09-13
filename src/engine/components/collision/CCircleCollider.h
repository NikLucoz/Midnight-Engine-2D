#pragma once
#include "engine/components/Component.h"

class CCircleCollider : public Component
{
public:
    float radius_;
    CCircleCollider() = default;
    CCircleCollider(const float radius)
    {
        this->radius_ = radius;
    }
};