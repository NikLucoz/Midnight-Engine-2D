#pragma once

#include "engine/utils/math/Vector2.h"
#include <SFML/Graphics/Color.hpp>

void drawVector2(const char* label, Vec2f& value);
void drawVector2(const char* label, Vector2<int>& value);
void drawColor(const char* label, sf::Color& color);