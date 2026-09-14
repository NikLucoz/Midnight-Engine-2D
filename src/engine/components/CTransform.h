#pragma once

#include "engine/components/Component.h"
#include "engine/utils/math/Vector2.h"

struct CTransform : public Component
{
public:
    Vec2f position = {0.0f, 0.0f};
    Vec2f velocity = {0.0f, 0.0f};
    Vec2f scale = {1.0f, 1.0f};
    float rotation = 0.0f;

    CTransform() = default;
    CTransform(const Vec2f& position, const Vec2f& velocity, float rotation, const Vec2f& scale)
        : position(position), velocity(velocity), rotation(rotation), scale(scale) {}
    Vec2f getPosition() const { return position; }
    Vec2f getVelocity() const { return velocity; }
    float getRotation() const { return rotation; }
    Vec2f getScale() const { return scale; }
};