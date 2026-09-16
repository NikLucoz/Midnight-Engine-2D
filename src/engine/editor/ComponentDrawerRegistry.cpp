// ComponentDrawerRegistry.cpp
#include "ComponentDrawerRegistry.h"
#include <imgui.h>
#include "engine/entities/Entity.h"

void ComponentDrawerRegistry::drawEntity(Entity& entity) {
    for (auto& [type, entry] : drawers_) {
        if (entity.hasComponent(type)) {
            entry.draw(entity.getComponentPtr(type));
        }
    }
}