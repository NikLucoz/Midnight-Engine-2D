#pragma once

#include "SFML/Graphics/RenderWindow.hpp"
#include "engine/assets/Assets.h"
#include "engine/assets/tilemap/Tile.h"
#include "engine/assets/tilemap/Tilemap.h"
#include "engine/components/CTransform.h"
#include "engine/components/rendering/CAnimatedSprite.h"
#include "engine/components/rendering/CRenderable.h"
#include "engine/components/rendering/CShape.h"
#include "engine/components/rendering/CSprite.h"
#include "engine/entities/Entity.h"
#include "engine/entities/EntityManager.h"
#include "engine/utils/debug_ui/DebugUI.h"
#include <SFML/Graphics.hpp>
#include <string>

struct RenderContext
{
    sf::RenderTarget& target;
    Assets& assets;
    const DebugOptions& debugOptions;
};

class IRenderingSystem
{
public:
    virtual ~IRenderingSystem() = default;

    virtual void renderEntities(float deltaTime, RenderContext renderContext) = 0;
    virtual void renderTilemapLayer(RenderContext renderContext, TileMap& tilemap, std::string tilemapLayerName = "default") = 0;
};

class DefaultRenderingSystem : public IRenderingSystem{

public:
    void renderEntities(float deltaTime, RenderContext renderContext) {
        std::vector<Entity*> entitiesToDraw;

        for (const std::shared_ptr<Entity>& entityPtr :
            EntityManager::getInstance().getEntities())
        {
            Entity& entity = *entityPtr;

            if (!entity.hasComponent<CRenderable>())
                continue;

            if (!entity.getComponent<CRenderable>().isVisible())
                continue;

            entitiesToDraw.push_back(&entity);
        }

        std::stable_sort(
            entitiesToDraw.begin(),
            entitiesToDraw.end(),
            [](Entity* lhs, Entity* rhs)
            {
                return lhs->getComponent<CRenderable>().getRenderLayer() < rhs->getComponent<CRenderable>().getRenderLayer();
            }
        );
        
        for (Entity* entity : entitiesToDraw)
        {
            Entity& e = *entity;
            if (!e.hasComponent<CTransform>()) continue;
            CTransform& transform = e.getComponent<CTransform>();
            CRenderable& cRenderable = e.getComponent<CRenderable>();
            Vec2f pos = transform.getPosition();
            
            if (e.hasComponent<CShape>())
            {
                CShape& cShape = e.getComponent<CShape>();
                cShape.getShape()->setPointCount(cShape.point_count_);
                cShape.getShape()->setPosition(sf::Vector2f(pos.x, pos.y));
                cShape.getShape()->setRotation(sf::degrees(transform.getRotation()));
                cShape.getShape()->setOrigin({cShape.radius_, cShape.radius_});
                cShape.getShape()->setFillColor(cShape.fillColor_);
                cShape.getShape()->setOutlineColor(cShape.outlineColor_);
                cShape.getShape()->setOutlineThickness(cShape.outlineThickness_);
                cShape.getShape()->setScale((cRenderable.getLocalScale() * transform.getScale()).toSFVector2());
                renderContext.target.draw(*cShape.getShape());
            }

            if (e.hasComponent<CAnimatedSprite>()) {
                CAnimatedSprite& cAnimatedSprite = e.getComponent<CAnimatedSprite>();
                sf::Sprite& sprite = cAnimatedSprite.animation->getSprite();
                sprite.setPosition(sf::Vector2f(transform.getPosition().x, transform.getPosition().y));
                sprite.setRotation(sf::degrees(transform.getRotation()));
                sprite.setScale((cRenderable.getLocalScale() * transform.getScale()).toSFVector2());
                renderContext.target.draw(sprite);
            }
            
            if (e.hasComponent<CSprite>())
            {
                CSprite& sprite = e.getComponent<CSprite>();
                auto& sfSprite = sprite.getSprite();
                sf::Vector2u textureSize = sprite.getTexture().getSize();
                Vec2f desiredSize = sprite.getSize();
                
                Vec2f calculatedScale(
                    desiredSize.x / textureSize.x,
                    desiredSize.y / textureSize.y
                );

                sf::Vector2f finalScale = (calculatedScale * (cRenderable.getLocalScale() * transform.getScale())).toSFVector2();
                
                sf::Vector2f spriteOrigin(
                    textureSize.x / 2.0f,
                    textureSize.y / 2.0f
                );
                
                sfSprite.setOrigin(spriteOrigin);
                sfSprite.setPosition(sf::Vector2f(transform.getPosition().x, transform.getPosition().y));
                sfSprite.setRotation(sf::degrees(transform.getRotation()));
                sfSprite.setScale(finalScale);
                renderContext.target.draw(sfSprite);
            }
            
            if (renderContext.debugOptions.showEntityIds)
            {
                sf::Text entityIdText(
                    renderContext.assets.getFont("fontArial"),
                    std::to_string(e.getId()),
                    14
                );

                entityIdText.setOrigin({7.0f, 14.0f});
                entityIdText.setPosition({pos.x, pos.y - 40.0f});
                entityIdText.setFillColor(sf::Color::White);
                entityIdText.setOutlineColor(sf::Color::Black);
                entityIdText.setOutlineThickness(1.0f);

                renderContext.target.draw(entityIdText);
            }
        }
    }

    void renderTilemapLayer(RenderContext renderContext, TileMap& tilemap, std::string tilemapLayerName = "default") {
        TileLayer& layer = tilemap.getLayer(tilemapLayerName);
        TileDefinitions tileDefinitions = tilemap.getTileDefinitions();
        for (int row = 0; row < tilemap.getHeight(); ++row)
        {
            for (int column = 0; column < tilemap.getWidth(); ++column)
            {
                const int tileId = layer.tiles[static_cast<std::size_t>(row * tilemap.getWidth() + column)];
                if (tileId < 0)
                    continue;

                auto definitionIt = tileDefinitions.find(static_cast<std::size_t>(tileId));
                if (definitionIt == tileDefinitions.end())
                    throw std::runtime_error("Tile definition not found: " + std::to_string(tileId));

                const Tile& definition = definitionIt->second;
                sf::Sprite sprite(renderContext.assets.getTexture(definition.getTilesetName()));
                sprite.setTextureRect(definition.getTextureRect());
                sprite.setPosition(sf::Vector2f(
                    static_cast<float>(tilemap.getOrigin().x + column * tilemap.getTileWidth()),
                    static_cast<float>(renderContext.target.getSize().y - tilemap.getOrigin().y - (row + 1) * tilemap.getTileHeight())));
                renderContext.target.draw(sprite);
            }
        }
    }
};