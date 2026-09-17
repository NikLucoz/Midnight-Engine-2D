#pragma once
#include "engine/components/CTransform.h"
#include "engine/components/collision/CBoundingBox.h"
#include "engine/components/rendering/CShape.h"
#include "engine/entities/Entity.h"
#include "example_game/components/CInput.h"

class EPlayer : public Entity
{
public:
    Vec2f startPosition;
    EPlayer(const std::string& tag, size_t id, const std::string& name = "player") : Entity(tag, id, name)
    {
        addComponent<CTransform>(Vec2f(300.0f, 300.0f), Vec2f(300.0f, 300.0f), 0, Vec2f(1,1));
        addRenderable<CShape>(30, 12, sf::Color::Black, sf::Color::Red, 5);
        addComponent<CInput>();
        addComponent<CBoundingBox>(Vector2<int>(60,60));
    }
};
