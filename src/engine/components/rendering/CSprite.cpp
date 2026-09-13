#include "engine/components/rendering/CSprite.h"
#include "SFML/Graphics/Sprite.hpp"
#include <SFML/Graphics/Texture.hpp>

CSprite::CSprite() : m_size(0, 0), m_origin(0, 0), m_color(sf::Color::White)
{
}

CSprite::CSprite(const sf::Texture& texture, const Vec2f& size, const Vec2f& origin,
                 sf::Color color)
    : sprite_(sf::Sprite(texture)),
      m_size(size),
      m_origin(origin),
      m_color(color)
{
}

Vec2f CSprite::getSize()
{
    return m_size;
}

Vec2f CSprite::getOrigin()
{
    return m_origin;
}

sf::Sprite& CSprite::getSprite() {
    return sprite_.value();
}

const sf::Texture& CSprite::getTexture()
{
    return sprite_.value().getTexture();
}