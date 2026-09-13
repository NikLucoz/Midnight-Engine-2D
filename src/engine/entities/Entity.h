#pragma once
#include <memory>
#include <string>
#include <tuple>

#include "engine/components/collision/CBoundingBox.h"
#include "engine/components/collision/CCircleCollider.h"
#include "engine/components/gameplay/CInput.h"
#include "engine/components/gameplay/CLifespan.h"
#include "engine/components/rendering/CRenderable.h"
#include "engine/components/rendering/CShape.h"
#include "engine/components/gameplay/CSpecialBullet.h"
#include "engine/components/rendering/CSprite.h"
#include "engine/components/CTransform.h"
#include "engine/components/rendering/CAnimatedSprite.h"

using ComponentTuple = std::tuple<
    CTransform,
    CRenderable,
    CSprite,
    CShape,
    CInput,
    CCircleCollider,
    CLifespan,
    CSpecialBullet,
    CBoundingBox,
    CAnimatedSprite
>;

class Entity
{
private:
    ComponentTuple components_;
    bool bIsMarkedForDestruction_ = false;
    std::string tag_ = "default";
    std::string name_ = "entity";
    std::string sceneName_ = "default";
    size_t id_ = 0;

protected:
    Entity();
    Entity(const std::string& tag, const std::string& sceneName, size_t id,
           const std::string& name = "entity");

public:
    
    template <typename T>
    T& getComponent()
    {
        return std::get<T>(components_);
    }

    template <typename T>
    bool hasComponent()
    {
        return getComponent<T>().exists;
    }

    template <typename T, typename... TArgs>
    T& addComponent(TArgs&&... mArgs)
    {
        auto& component = getComponent<T>();
        component = T(std::forward<TArgs>(mArgs)...);
        component.exists = true;
        return component;
    }

    template <typename T, typename... TArgs>
    T& addRenderable(TArgs&&... mArgs)
    {
        addComponent<CRenderable>();
        return addComponent<T>(std::forward<TArgs>(mArgs)...);
    }

    template <typename T>
    void removeComponent()
    {
        getComponent<T>() = T();
        getComponent<T>().exists = false;
    }

    size_t getId() const;
    void destroy();
    const std::string& getTag() const;
    const std::string& getSceneName() const;
    const std::string& getName() const;
    bool isMarkedForDestruction() const;
    
    // with this only the EntityManager can create Entities since the constructors are private
    friend class EntityManager;
};

