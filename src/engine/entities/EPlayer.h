#pragma once
#include "Entity.h"

class EPlayer : public Entity
{
public:
    Vec2f startPosition;
    EPlayer(const std::string& tag, const std::string& sceneName, size_t id, const std::string& name = "player") : Entity(tag, sceneName, id, name)
    {
        addComponent<CTransform>(Vec2f(300.0f, 300.0f), Vec2f(300.0f, 300.0f), 0, Vec2f(1,1));
        addRenderable<CShape>(30, 12, sf::Color::Black, sf::Color::Red, 5);
        addComponent<CInput>();
        addComponent<CBoundingBox>(Vector2<int>(60,60));
    }
};
