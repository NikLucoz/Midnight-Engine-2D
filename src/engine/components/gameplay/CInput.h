#pragma once
#include "engine/components/Component.h"
#include "engine/utils/math/Vector2.h"

class CInput : public Component
{
public:
    bool bUp = false;
    bool bDown = false;
    bool bLeft = false;
    bool bRight = false;
    bool bMouseLeft = false;
    bool bMouseRight = false;
    
    CInput() = default;
    bool hasMovementInput() const;
    Vector2<int> getMovementDirection() const;
};