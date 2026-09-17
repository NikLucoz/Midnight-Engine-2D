#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/Texture.hpp"
#include "engine/assets/Assets.h"
#include "engine/components/CTransform.h"
#include "engine/components/collision/CBoundingBox.h"
#include "engine/components/collision/CCircleCollider.h"
#include "engine/components/rendering/CAnimatedSprite.h"
#include "engine/components/rendering/CShape.h"
#include "engine/components/rendering/CSprite.h"
#include "engine/editor/components_registry/ComponentLoaderRegistry.h"
#include "engine/utils/math/Vector2.h"
#include "engine/utils/parser.h"

REGISTER_COMPONENT_LOADER(CTransform,
    Vec2f position = {0.0f, 0.0f};
    Vec2f velocity = {0.0f, 0.0f};
    Vec2f scale    = {1.0f, 1.0f};
    float rotation = 0.0f;

    if (args.size() >= 1) position = parseVec2f(args[0]);
    if (args.size() >= 2) velocity = parseVec2f(args[1]);
    if (args.size() >= 3) rotation = std::stof(args[2]);
    if (args.size() >= 4) scale = parseVec2f(args[3]);

    e.addComponent<CTransform>(position, velocity, rotation, scale);
);

REGISTER_COMPONENT_LOADER(CBoundingBox,
    Vector2<int> size = {0,0};
    if (args.size() >= 1) size = parseVec2(args[0]);

    e.addComponent<CBoundingBox>(size);
);

REGISTER_COMPONENT_LOADER(CCircleCollider,
    float radius = 1;
    if (args.size() >= 1) radius = parseFloat(args[0]);

    e.addComponent<CCircleCollider>(radius);
);

REGISTER_COMPONENT_LOADER(CShape,
    float     radius           = 1.0f;
    int8_t    pointCount       = 3;
    sf::Color fillColor        = sf::Color::Red;
    sf::Color outlineColor     = sf::Color::White;
    uint16_t  outlineThickness = 10;

    if (args.size() >= 1) radius           = parseFloat(args[0]);
    if (args.size() >= 2) pointCount       = parseInt8(args[1]);
    if (args.size() >= 3) fillColor        = parseHexToColor(args[2]);
    if (args.size() >= 4) outlineColor     = parseHexToColor(args[3]);
    if (args.size() >= 5) outlineThickness = parseUint16(args[4]);

    e.addRenderable<CShape>(radius, pointCount, fillColor, outlineColor, outlineThickness);
)

REGISTER_COMPONENT_LOADER(CSprite,
    Vec2f size = {0.0f, 0.0f};
    Vec2f origin = {0.0f, 0.0f};
    sf::Color color = sf::Color::White;
    
    if (args.size() < 1) return;
    sf::Texture& texture = Assets::getInstance().getTexture(args[0]);
    
    if (args.size() >= 2) size = parseVec2f(args[1]);
    if (args.size() >= 3) origin = parseVec2f(args[2]);
    if (args.size() >= 4) color = parseHexToColor(args[3]);
    e.addRenderable<CSprite>(texture, size, origin, color);
);

REGISTER_COMPONENT_LOADER(CAnimatedSprite,
    if (args.size() >= 1) {
        Animation& animation = Assets::getInstance().getAnimation(args[0]); 
        e.addRenderable<CAnimatedSprite>(animation);
    }
);