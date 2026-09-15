#include "EntityManager.h"

#include <algorithm>

EntityManager& EntityManager::getInstance()
{
    static EntityManager instance;
    return instance;
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag, const std::string& sceneName, const std::string& entityName)
{
    const size_t id = totalEntities_++;
    auto e = std::shared_ptr<Entity>(new Entity(tag, sceneName, id, entityName));
    toAdd_.push_back(e);
    entitiesIdMap_[id] = e;
    return e;
}

void EntityManager::update()
{
    for (auto& e : toAdd_)
    {
        entities_.push_back(e);
        entitiesMap_[e->getTag()].push_back(e);
        entitiesSceneMap_[e->getSceneName()].push_back(e);
    }

    std::vector<std::shared_ptr<Entity>> toRemove;
    for (auto& e : entities_)
    {
        if (e->isMarkedForDestruction() == false) continue;
        toRemove.push_back(e);
    }
    
    for (auto& e : toRemove)
    {
        if (e->parentId_ != static_cast<size_t>(-1)) {
            const auto parent = getEntityWithId(e->parentId_);
            if (parent != nullptr) {
                parent->removeChild(*e);
            }
        }

        for (const size_t childId : e->childrenIds_) {
            const auto child = getEntityWithId(childId);
            if (child != nullptr) {
                child->parentId_ = static_cast<size_t>(-1);
            }
        }

        auto it = std::find(entities_.begin(), entities_.end(), e);
        if (it != entities_.end()) {
            entities_.erase(it);
        }
    
        auto& taggedEntities = entitiesMap_[e->getTag()];
        auto it2 = std::find(taggedEntities.begin(), taggedEntities.end(), e);
        if (it2 != taggedEntities.end()) {
            taggedEntities.erase(it2);
        }
    
        if (entitiesMap_[e->getTag()].empty()) {
            entitiesMap_.erase(e->getTag());
        }

        auto& sceneEntities = entitiesSceneMap_[e->getSceneName()];
        auto sceneIt = std::find(sceneEntities.begin(), sceneEntities.end(), e);
        if (sceneIt != sceneEntities.end()) {
            sceneEntities.erase(sceneIt);
        }

        if (sceneEntities.empty()) {
            entitiesSceneMap_.erase(e->getSceneName());
        }

        entitiesIdMap_.erase(e->getId());
    }
    
    toAdd_.clear();
}

EntityVec& EntityManager::getEntities()
{
    return entities_;
}

EntityVec& EntityManager::getEntities(const std::string& tag)
{
    return entitiesMap_[tag];
}

EntityVec EntityManager::getEntities(const std::vector<std::string>& tags)
{
    EntityVec entities;
    for (const auto& tag : tags)
    {
        auto it = entitiesMap_.find(tag);
        if (it != entitiesMap_.end())
        {
            entities.insert(entities.end(), it->second.begin(), it->second.end());
        }
    }
    return entities;
}

EntityVec& EntityManager::getEntitiesInScene(const std::string& sceneName)
{
    return entitiesSceneMap_[sceneName];
}

std::shared_ptr<Entity> EntityManager::getEntityWithId(size_t id) {
    const auto it = entitiesIdMap_.find(id);
    if (it == entitiesIdMap_.end()) {
        return nullptr;
    }

    return it->second;
}
