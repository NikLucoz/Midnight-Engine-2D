#pragma once
#include "engine/components/Component.h"
#include <engine/assets/Animation.h>

struct CAnimatedSprite : public Component
{
public:
    Animation* animation = nullptr;

    CAnimatedSprite() = default;

    explicit CAnimatedSprite(Animation& animation)
        : animation(&animation)
    {
    }
};