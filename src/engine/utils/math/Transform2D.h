#pragma once

#include "Vector2.h"

struct Transform2D {
    Vec2f position = {0.0f, 0.0f};
    Vec2f scale = {1.0f, 1.0f};
    float rotation = 0.0f;
};