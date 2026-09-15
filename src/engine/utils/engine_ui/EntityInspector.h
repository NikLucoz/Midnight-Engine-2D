#pragma once

#include "SFML/Graphics/RenderTarget.hpp"
#include "engine/entities/Entity.h"
#include <memory>

struct DebugRuntimeInfo;

class EntityInspector {
    std::shared_ptr<Entity> currentEntity_;
    public:
    void setCurrentEntity(std::shared_ptr<Entity> entity);
    void clearCurrentEntity();
    void render(sf::RenderTarget& renderTarget,
                const DebugRuntimeInfo& runtimeInfo,
                float bottomPanelHeight);
};