#include "CollisionUtils.h"
#include "engine/entities/Entity.h"
#include "engine/components/CTransform.h"
#include "engine/components/collision/CBoundingBox.h"

static Vector2<int> GetTopLeft(Entity* entity)
{
    auto& t  = entity->getComponent<CTransform>();
    auto& bb = entity->getComponent<CBoundingBox>();

    return Vector2<int>(
        t.getPosition().x - bb.size.x / 2,
        t.getPosition().y - bb.size.y / 2
    );
}

Vector2<int> getOverlap(Entity* entity1, Entity* entity2)
{
    Vector2<int> tl1 = GetTopLeft(entity1);
    Vector2<int> tl2 = GetTopLeft(entity2);

    auto& bb1 = entity1->getComponent<CBoundingBox>();
    auto& bb2 = entity2->getComponent<CBoundingBox>();

    int overlapX = std::min(tl1.x + bb1.size.x, tl2.x + bb2.size.x) - std::max(tl1.x, tl2.x);
    int overlapY = std::min(tl1.y + bb1.size.y, tl2.y + bb2.size.y) - std::max(tl1.y, tl2.y);

    return Vector2<int>(overlapX, overlapY);
}

bool IsColliding(Entity* a, Entity* b)
{
    auto overlap = getOverlap(a, b);
    return overlap.x > 0 && overlap.y > 0;
}

bool IsIntersect(Entity* a, Entity* b)
{
    Vector2<int> tl1 = GetTopLeft(a);
    Vector2<int> tl2 = GetTopLeft(b);

    auto& bb1 = a->getComponent<CBoundingBox>();
    auto& bb2 = b->getComponent<CBoundingBox>();

    return (tl1.x < tl2.x + bb2.size.x) &&
           (tl2.x < tl1.x + bb1.size.x) &&
           (tl1.y < tl2.y + bb2.size.y) &&
           (tl2.y < tl1.y + bb1.size.y);
}

bool IsInside(Entity* entity, const Vec2f& point)
{
    Vector2<int> tl = GetTopLeft(entity);
    auto& bb = entity->getComponent<CBoundingBox>();

    return point.x > tl.x &&
           point.x < tl.x + bb.size.x &&
           point.y > tl.y &&
           point.y < tl.y + bb.size.y;
}