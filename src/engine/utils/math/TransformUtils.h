#pragma once

#include "Transform2D.h"

namespace TransformUtils {

Transform2D combine(const Transform2D& parent, const Transform2D& local);
Vec2f rotate(const Vec2f& vector, float degrees);

}