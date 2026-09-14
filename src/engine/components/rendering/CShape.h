#pragma once
#include <SFML/Graphics/Color.hpp>
#include "engine/components/Component.h"
#include <SFML/Graphics/CircleShape.hpp>

struct CShape : public Component
{
    sf::CircleShape shape;

public:
    float radius_;
    int8_t point_count_;
    sf::Color fillColor_;
    sf::Color outlineColor_;
    uint16_t outlineThickness_;
    CShape() = default;
    CShape(float radius, int8_t point_count, sf::Color fillColor, sf::Color OutlineColor,
           uint16_t OutlineThickness);
    sf::CircleShape* getShape();
};