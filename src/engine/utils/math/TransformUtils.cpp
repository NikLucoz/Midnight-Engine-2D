#include "TransformUtils.h"

#include <cmath>

namespace TransformUtils {

Transform2D combine(const Transform2D& parent, const Transform2D& local)
{
    const Vec2f scaledLocalPosition = local.position * parent.scale;
    const Vec2f rotatedLocalPosition = rotate(scaledLocalPosition, parent.rotation);

    return {
        parent.position + rotatedLocalPosition,
        parent.scale * local.scale,
        parent.rotation + local.rotation
    };
}

Vec2f rotate(const Vec2f& vector, float degrees)
{
    constexpr float pi = 3.14159265358979323846f;
    const float radians = degrees * pi / 180.0f;
    const float cosine = std::cos(radians);
    const float sine = std::sin(radians);

    return {
        vector.x * cosine - vector.y * sine,
        vector.x * sine + vector.y * cosine
    };
}

}