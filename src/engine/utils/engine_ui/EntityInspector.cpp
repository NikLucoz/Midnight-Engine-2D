#include "EntityInspector.h"
#include "SFML/Graphics/RenderTarget.hpp"
#include "engine/editor/ComponentDrawerRegistry.h"
#include "engine/entities/Entity.h"
#include "engine/entities/EntityManager.h"
#include "imgui.h"
#include <algorithm>
#include <memory>
namespace {
constexpr float panelWidth = 380.0f;
}

void EntityInspector::clearCurrentEntity() { currentEntity_ = nullptr; }

void EntityInspector::setCurrentEntity(std::shared_ptr<Entity> entity) { currentEntity_ = entity; }

void EntityInspector::render(sf::RenderTarget &renderTarget, const DebugRuntimeInfo &runtimeInfo, float bottomPanelHeight) {
    (void)renderTarget;
    (void)runtimeInfo;

    if (currentEntity_ == nullptr) {
        return;
    }

    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    
    const float inspectorHeight = std::max(240.0f, viewport->WorkSize.y - bottomPanelHeight);
    
        const ImVec2 topRight(viewport->WorkPos.x + viewport->WorkSize.x, viewport->WorkPos.y);
    
    ImGui::SetNextWindowPos(topRight, ImGuiCond_Always, ImVec2(1.0f, 0.0f));
    
    ImGui::SetNextWindowSize(ImVec2(panelWidth, inspectorHeight), ImGuiCond_Always);
   
    if (!ImGui::Begin("Inspector", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
        ImGui::End();
        return;
    }

    ImGui::TextColored(ImVec4(0.35f, 0.78f, 1.0f, 1.0f), "%s", currentEntity_->getName().c_str());
    
    ImGui::SameLine(0, 20);
    if (ImGui::Button("Destroy")) {
        if (currentEntity_->getParent() != Entity::NoParent) {
            setCurrentEntity(EntityManager::getInstance().getEntityWithId(currentEntity_->getParent()));
        } else {
            clearCurrentEntity();
        }
        currentEntity_->destroy();
        ImGui::End();
        return;
    }

    ImGui::Separator();

    if (ImGui::BeginTable("entity_properties", 2, ImGuiTableFlags_SizingStretchProp)) {
        const auto drawProperty = [](const char *label, const char *value) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextDisabled("%s", label);
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%s", value);
        };

        drawProperty("ID", std::to_string(currentEntity_->getId()).c_str());
        drawProperty("Tag", currentEntity_->getTag().c_str());
        drawProperty("Scene", currentEntity_->getSceneName().c_str());
        ImGui::EndTable();
    }

    ImGui::Spacing();
    ImGui::SeparatorText("Hierarchy");

    const size_t parentId = currentEntity_->getParent();
    if (parentId == Entity::NoParent) {
        ImGui::TextDisabled("Parent: None");
    } else {
        ImGui::Text("Parent ID: %zu", parentId);
        ImGui::SameLine();

        auto parent = EntityManager::getInstance().getEntityWithId(parentId);
        if (parent != nullptr && ImGui::Button("Inspect##parent")) {
            currentEntity_ = parent;
        } else if (parent == nullptr) {
            ImGui::TextDisabled("(unavailable)");
        }
    }

    ImGui::SeparatorText("Components");
    ComponentDrawerRegistry::getInstance().drawEntity(*currentEntity_);

    ImGui::End();
}