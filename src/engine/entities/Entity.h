#pragma once
#include "engine/components/ComponentManager.h"
#include "engine/components/rendering/CRenderable.h"
#include <string>
#include <typeindex>
#include <vector>

class Entity {
  public:
    static constexpr std::size_t NoParent = std::numeric_limits<std::size_t>::max();

    // Component access
    template <typename T> T &getComponent() { return ComponentManager::getInstance().get<T>(id_); }

    template <typename T> const T &getComponent() const { return ComponentManager::getInstance().get<T>(id_); }

    template <typename T> bool hasComponent() const { return ComponentManager::getInstance().has<T>(id_); }

    bool hasComponent(std::type_index type) const { return ComponentManager::getInstance().has(id_, type); }

    template <typename T, typename... Args> T &addComponent(Args &&...args) { return ComponentManager::getInstance().add<T>(id_, std::forward<Args>(args)...); }

    template <typename T, typename... TArgs> T &addRenderable(TArgs &&...mArgs) {
        addComponent<CRenderable>();
        return addComponent<T>(std::forward<TArgs>(mArgs)...);
    }

    template <typename T> void removeComponent() { ComponentManager::getInstance().remove<T>(id_); }

    void *getComponentPtr(std::type_index type) { return ComponentManager::getInstance().getPtr(id_, type); }

    // Identity / state
    size_t getId() const;
    void destroy();
    const std::string &getTag() const;
    const std::string &getName() const;
    const std::string &getSceneName() const;
    void setSceneName(std::string sceneName);
    bool isMarkedForDestruction() const;

    // Hierarchy
    size_t getParent();
    void setParent(size_t id);
    void addChild(Entity &child);
    void removeChild(Entity &child);
    bool hasChild(Entity &entity);
    bool hasChild(size_t id);
    const std::vector<size_t> &getChildrens() const;
    void changeChildOrder(size_t id, int index);
    
  private:
    Entity();
    Entity(const std::string &tag, size_t id, const std::string &name = "entity");
    size_t id_ = 0;
    bool bIsMarkedForDestruction_ = false;
    std::string tag_ = "default";
    std::string name_ = "entity";
    std::string sceneName_ = "default";
    size_t parentId_ = NoParent;
    std::vector<size_t> childrenIds_;

    // with this only the EntityManager can create Entities since the constructors are private
    friend class EntityManager;
};