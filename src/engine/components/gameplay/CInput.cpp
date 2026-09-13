#include "engine/components/gameplay/CInput.h"

bool CInput::hasMovementInput() const
{
    return bDown || bLeft || bRight || bUp;
}

Vector2<int> CInput::getMovementDirection() const
{
    Vector2<int> moveDirection = Vector2<int>(0, 0);
    
    if (bUp)
    {
        moveDirection.y = -1;
    }
    
    if (bLeft)
    {
        moveDirection.x = -1;
    }
    
    if (bRight)
    {
        moveDirection.x = 1;
    }
    
    if (bDown)
    {
        moveDirection.y = 1;
    }
    
    return moveDirection;
}