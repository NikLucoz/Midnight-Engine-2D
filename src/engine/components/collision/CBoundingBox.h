#pragma once
#include "engine/components/Component.h"
#include "engine/utils/math/Vector2.h"

struct CBoundingBox : public Component
{
public:
    Vector2<int> size;
    CBoundingBox() = default;
    CBoundingBox(Vector2<int> size)
    {
        this->size = size;
    }
};