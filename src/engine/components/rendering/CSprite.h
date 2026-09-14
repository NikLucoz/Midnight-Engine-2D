#pragma once
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Texture.hpp> 
#include "SFML/Graphics/Sprite.hpp"
#include "engine/components/Component.h"
#include "engine/utils/math/Vector2.h"
#include <optional>

struct CSprite : public Component
{
    std::optional<sf::Sprite> sprite_;
public:
    Vec2f m_size;
    Vec2f m_origin;
    sf::Color m_color;
    
    CSprite();
    CSprite(const sf::Texture& texture, const Vec2f& size, const Vec2f& origin,
            sf::Color color = sf::Color::White);
    Vec2f getSize();
    Vec2f getOrigin();
  
    const sf::Texture& getTexture();
    sf::Sprite& getSprite();
};