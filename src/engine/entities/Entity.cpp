#include "Entity.h"
#include "engine/components/ComponentManager.h"
#include "engine/entities/EntityManager.h"
#include <algorithm>
#include <cstddef>

Entity::Entity()
{
}

Entity::Entity(const std::string& tag, size_t id, const std::string& name)
{
    name_ = name;
    tag_ = tag;
    id_ = id;
}


size_t Entity::getId() const
{
    return id_;
}

void Entity::destroy()
{
    if (bIsMarkedForDestruction_) {
        return;
    }

    bIsMarkedForDestruction_ = true;

    for (const size_t childId : childrenIds_) {
        const auto child = EntityManager::getInstance().getEntityWithId(childId);
        if (child != nullptr) {
            child->destroy();
        }
    }

    ComponentManager::getInstance().removeEntityComponents(id_);
}

const std::string& Entity::getTag() const
{
    return tag_;
}

const std::string &Entity::getSceneName() const
{
    return sceneName_;
}

const std::string &Entity::getName() const
{
    return name_;
}

bool Entity::isMarkedForDestruction() const
{
    return bIsMarkedForDestruction_;
}

void Entity::setSceneName(std::string sceneName) {
    sceneName_ = sceneName;
}


void Entity::addChild(Entity& child) {
    if (&child == this || hasChild(child)) {
        return;
    }

    for (Entity* current = this; current->parentId_ != static_cast<size_t>(-1);) {
        if (current->parentId_ == child.id_) {
            return;
        }

        const auto parent = EntityManager::getInstance().getEntityWithId(current->parentId_);
        if (parent == nullptr) {
            break;
        }

        current = parent.get();
    }

    if (child.parentId_ != static_cast<size_t>(-1)) {
        const auto oldParent = EntityManager::getInstance().getEntityWithId(child.parentId_);
        if (oldParent != nullptr) {
            oldParent->removeChild(child);
        }
    }

    child.parentId_ = id_;
    childrenIds_.push_back(child.id_);
}

void Entity::removeChild(Entity& child) {
    if(hasChild(child)) {
        auto it = std::find(childrenIds_.begin(), childrenIds_.end(),  child.id_);
        childrenIds_.erase(it);
        child.parentId_ = static_cast<size_t>(-1);
    }
}

bool Entity::hasChild(Entity& entity) {
    auto it = std::find(childrenIds_.begin(), childrenIds_.end(),  entity.id_);
    return it != childrenIds_.end();
}

bool Entity::hasChild(size_t id) {
    return hasChild(*EntityManager::getInstance().getEntityWithId(id).get());
}

const std::vector<size_t>& Entity::getChildrens() const {
    return childrenIds_;
}

size_t Entity::getParent() {
    return parentId_;
}

void Entity::setParent(size_t id) {
    if (hasChild(id)) return;
    parentId_ = id;
}

void Entity::changeChildOrder(size_t id, int index) {
    if(!hasChild(id)) return;
    if(index < 0 || index >= childrenIds_.size()) return;

    auto it = find(childrenIds_.begin(), childrenIds_.end(), id);
    int old_index = it - childrenIds_.begin();
    if (old_index == index) return;

    auto val = std::move(childrenIds_[old_index]);
    childrenIds_.erase(it);
    childrenIds_.insert(childrenIds_.begin() + index, std::move(val));
}
