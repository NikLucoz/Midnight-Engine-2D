#pragma once

#include "engine/entities/Entity.h"
#include "engine/entities/EntityManager.h"
#include <functional>
#include <memory>
#include <unordered_map>
#include <unordered_set>

struct DebugRuntimeInfo;

class HierarchyInspector {
  public:
    using SelectionCallback = std::function<void(const std::shared_ptr<Entity> &)>;

    void render(const DebugRuntimeInfo &runtimeInfo, const EntityVec &entities, const SelectionCallback &onEntitySelected, float bottomPanelHeight);

  private:
    using LiveEntityMap = std::unordered_map<size_t, std::shared_ptr<Entity>>;
    using ChildMap = std::unordered_map<size_t, std::vector<std::shared_ptr<Entity>>>;
    bool changeSceneOpen_ = false;

    void renderEntity(const std::shared_ptr<Entity> &entity,
              const ChildMap &childrenByParent,
              std::unordered_set<size_t> &visited,
              const SelectionCallback &onEntitySelected);
};
