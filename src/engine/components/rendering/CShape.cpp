#include "engine/components/rendering/CShape.h"

CShape::CShape(float radius, int8_t point_count, sf::Color fillColor, sf::Color outlineColor,
               uint16_t outlineThickness)
    : radius_(radius),
      point_count_(point_count),
      fillColor_(fillColor),
      outlineColor_(outlineColor),
      outlineThickness_(outlineThickness)
{
    exists = true;
    shape = sf::CircleShape(radius_);
}

sf::CircleShape* CShape::getShape()
{
    return &shape;
}