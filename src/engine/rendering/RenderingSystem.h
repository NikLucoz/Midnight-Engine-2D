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
#include "engine/editor/engine_ui/DebugUI.h"
#include "engine/utils/math/Transform2D.h"
#include "engine/utils/math/TransformUtils.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

struct RenderContext {
    sf::RenderTarget &renderTarget;
    Assets &assets;
    std::vector<std::shared_ptr<Entity>> &entitiesToRender;
    const DebugOptions &debugOptions;
};

class IRenderingSystem {
  public:
    virtual ~IRenderingSystem() = default;

    virtual void renderEntities(float deltaTime, RenderContext renderContext) = 0;
    virtual void renderTilemapLayer(RenderContext renderContext, TileMap &tilemap,
                                    std::string tilemapLayerName = "default") = 0;
};

class DefaultRenderingSystem : public IRenderingSystem {

  public:
    void renderEntities(float deltaTime, RenderContext renderContext) {
        std::vector<Entity *> roots;
        roots.reserve(renderContext.entitiesToRender.size());

        for (const auto &entityPtr : renderContext.entitiesToRender) {
            Entity &entity = *entityPtr;

            if (entity.getParent() == Entity::NoParent && entity.hasComponent<CRenderable>() &&
                entity.getComponent<CRenderable>().isVisible()) {
                roots.push_back(&entity);
            }
        }

        std::sort(roots.begin(), roots.end(), [](Entity *lhs, Entity *rhs) {
            const auto &left = lhs->getComponent<CRenderable>();
            const auto &right = rhs->getComponent<CRenderable>();

            if (left.getRenderLayer() != right.getRenderLayer())
                return left.getRenderLayer() < right.getRenderLayer();

            return left.getOrderInLayer() < right.getOrderInLayer();
        });

        for (Entity *root : roots)
            renderEntity(*root, renderContext, nullptr);
    }

    void renderEntity(Entity &entity, RenderContext &context, const Transform2D *parentTransform) {
        if (!entity.hasComponent<CRenderable>())
            return;

        auto &renderable = entity.getComponent<CRenderable>();

        if (!renderable.isVisible())
            return;

        Transform2D worldTransform;

        if (entity.hasComponent<CTransform>()) {
            const auto &local = entity.getComponent<CTransform>();

            Transform2D localTransform{local.getPosition(), local.getScale(), local.getRotation()};

            worldTransform =
                parentTransform == nullptr ? localTransform : TransformUtils::combine(*parentTransform, localTransform);

            renderComponents(entity, context, worldTransform);
        }

        for (const std::size_t childId : entity.getChildrens()) {
            auto child = EntityManager::getInstance().getEntityWithId(childId);

            if (child != nullptr)
                renderEntity(*child, context, &worldTransform);
        }
    }

    void renderComponents(Entity &e, RenderContext &renderContext, const Transform2D &worldTransform) {
        CRenderable &cRenderable = e.getComponent<CRenderable>();
        const Vec2f &pos = worldTransform.position;

        if (e.hasComponent<CShape>()) {
            CShape &cShape = e.getComponent<CShape>();
            cShape.getShape()->setPointCount(cShape.point_count_);
            cShape.getShape()->setPosition(sf::Vector2f(pos.x, pos.y));
            cShape.getShape()->setRotation(sf::degrees(worldTransform.rotation));
            cShape.getShape()->setOrigin({cShape.radius_, cShape.radius_});
            cShape.getShape()->setFillColor(cShape.fillColor_);
            cShape.getShape()->setOutlineColor(cShape.outlineColor_);
            cShape.getShape()->setOutlineThickness(cShape.outlineThickness_);
            cShape.getShape()->setScale((cRenderable.getLocalScale() * worldTransform.scale).toSFVector2());

            renderContext.renderTarget.draw(*cShape.getShape());
        }

        if (e.hasComponent<CAnimatedSprite>()) {
            CAnimatedSprite &cAnimatedSprite = e.getComponent<CAnimatedSprite>();
            if (cAnimatedSprite.animation != nullptr) {
                sf::Sprite &sprite = cAnimatedSprite.animation->getSprite();
                sprite.setPosition(sf::Vector2f(pos.x, pos.y));
                sprite.setRotation(sf::degrees(worldTransform.rotation));
                sprite.setScale((cRenderable.getLocalScale() * worldTransform.scale).toSFVector2());
                renderContext.renderTarget.draw(sprite);
            }
        }

        if (e.hasComponent<CSprite>()) {
            CSprite &sprite = e.getComponent<CSprite>();
            auto &sfSprite = sprite.getSprite();
            sf::Vector2u textureSize = sprite.getTexture().getSize();
            Vec2f desiredSize = sprite.getSize();

            Vec2f calculatedScale(desiredSize.x / textureSize.x, desiredSize.y / textureSize.y);

            sf::Vector2f finalScale =
                (calculatedScale * (cRenderable.getLocalScale() * worldTransform.scale)).toSFVector2();

            sf::Vector2f spriteOrigin(textureSize.x / 2.0f, textureSize.y / 2.0f);

            sfSprite.setOrigin(spriteOrigin);
            sfSprite.setPosition(sf::Vector2f(pos.x, pos.y));
            sfSprite.setRotation(sf::degrees(worldTransform.rotation));
            sfSprite.setScale(finalScale);
            sfSprite.setColor(sprite.m_color);
            renderContext.renderTarget.draw(sfSprite);
        }

        if (renderContext.debugOptions.showEntityIds) {
            sf::Text entityIdText(renderContext.assets.getFont("fontArial"), std::to_string(e.getId()), 14);

            entityIdText.setOrigin({7.0f, 14.0f});
            entityIdText.setPosition({pos.x, pos.y - 40.0f});
            entityIdText.setFillColor(sf::Color::White);
            entityIdText.setOutlineColor(sf::Color::Black);
            entityIdText.setOutlineThickness(1.0f);

            renderContext.renderTarget.draw(entityIdText);
        }
    }

    void renderTilemapLayer(RenderContext renderContext, TileMap &tilemap, std::string tilemapLayerName = "default") {
        TileLayer &layer = tilemap.getLayer(tilemapLayerName);
        TileDefinitions tileDefinitions = tilemap.getTileDefinitions();
        for (int row = 0; row < tilemap.getHeight(); ++row) {
            for (int column = 0; column < tilemap.getWidth(); ++column) {
                const int tileId = layer.tiles[static_cast<std::size_t>(row * tilemap.getWidth() + column)];
                if (tileId < 0)
                    continue;

                auto definitionIt = tileDefinitions.find(static_cast<std::size_t>(tileId));
                if (definitionIt == tileDefinitions.end())
                    throw std::runtime_error("Tile definition not found: " + std::to_string(tileId));

                const Tile &definition = definitionIt->second;
                sf::Sprite sprite(renderContext.assets.getTexture(definition.getTilesetName()));
                sprite.setTextureRect(definition.getTextureRect());
                sprite.setPosition(
                    sf::Vector2f(
                        static_cast<float>(tilemap.getOrigin().x + column * tilemap.getTileWidth()),
                        static_cast<float>(renderContext.renderTarget.getSize().y - tilemap.getOrigin().y - (row + 1) * tilemap.getTileHeight())
                    )
                );
                renderContext.renderTarget.draw(sprite);
            }
        }
    }
};