#pragma once
#include "engine/components/rendering/CRenderable.h"
#include <any>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>

/*
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
*/

using ComponentMap = std::unordered_map<std::type_index, std::any>;

class Entity {
  private:
    size_t id_ = 0;
    ComponentMap components_;
    bool bIsMarkedForDestruction_ = false;
    std::string tag_ = "default";
    std::string name_ = "entity";
    std::string sceneName_ = "default";
    size_t parentId_ = NoParent;
    std::vector<size_t> childrenIds_;

  protected:
    Entity();
    Entity(const std::string &tag, const std::string &sceneName, size_t id, const std::string &name = "entity");

  public:
    static constexpr std::size_t NoParent = std::numeric_limits<std::size_t>::max();

    template <typename T> T &getComponent() { return std::any_cast<T &>(components_.at(typeid(T))); }

    template <typename T> const T &getComponent() const { return std::any_cast<const T &>(components_.at(typeid(T))); }

    template <typename T> bool hasComponent() const { return components_.find(typeid(T)) != components_.end(); }

    template <typename T, typename... Args> T &addComponent(Args &&...args) {
        auto [iterator, inserted] = components_.insert_or_assign(typeid(T), T(std::forward<Args>(args)...));

        return std::any_cast<T &>(iterator->second);
    }

    template <typename T, typename... TArgs> T &addRenderable(TArgs &&...mArgs) {
        addComponent<CRenderable>();
        return addComponent<T>(std::forward<TArgs>(mArgs)...);
    }

    template <typename T> void removeComponent() { components_.erase(typeid(T)); }

    bool hasComponent(std::type_index type) const { return components_.find(type) != components_.end(); }

    void *getComponentPtr(std::type_index type) {
        auto it = components_.find(type);
        if (it == components_.end()) {
            return nullptr;
        }
        
        return &it->second;
    }

    size_t getId() const;
    void destroy();
    const std::string &getTag() const;
    const std::string &getSceneName() const;
    const std::string &getName() const;
    bool isMarkedForDestruction() const;

    void addChild(Entity &child);
    void removeChild(Entity &child);
    bool hasChild(Entity &entity);
    bool hasChild(size_t id);

    size_t getParent();
    void setParent(size_t id);

    void changeChildOrder(size_t id, int index);
    const std::vector<size_t> &getChildrens() const;

    // with this only the EntityManager can create Entities since the constructors are private
    friend class EntityManager;
};
