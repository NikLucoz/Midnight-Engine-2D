#include "HierarchyInspector.h"
#include "engine/utils/debug_ui/DebugUI.h"
#include "imgui.h"
#include <algorithm>
#include <string>

namespace {
constexpr float panelWidth = 400.0f;
}

void HierarchyInspector::render(const DebugRuntimeInfo &runtimeInfo, const EntityVec &entities, const SelectionCallback &onEntitySelected, float bottomPanelHeight) {
    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    const float panelHeight = std::max(240.0f, viewport->WorkSize.y - bottomPanelHeight);
    ImGui::SetNextWindowPos(viewport->WorkPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(panelWidth, panelHeight), ImGuiCond_Always);
    if (!ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
        ImGui::End();
        return;
    }

    ImGui::Text("Current Scene: %s", runtimeInfo.currentScene.c_str());
    ImGui::SameLine(0, 20);
    if (ImGui::Button("Change scene")) {
        changeSceneOpen_ = true;
    }

    ImGui::TextDisabled("%d entities", static_cast<int>(entities.size()));

    if (entities.empty()) {
        ImGui::TextDisabled("No entities in the active scene.");
        ImGui::End();
        return;
    }

    LiveEntityMap liveEntities;
    for (const auto &entity : entities) {
        if (entity != nullptr) {
            liveEntities[entity->getId()] = entity;
        }
    }

    ChildMap childrenByParent;
    for (const auto &entity : entities) {
        if (entity != nullptr && entity->getParent() != Entity::NoParent &&
            liveEntities.find(entity->getParent()) != liveEntities.end()) {
            childrenByParent[entity->getParent()].push_back(entity);
        }
    }

    std::unordered_set<size_t> visited;
    for (const auto &entity : entities) {
        if (entity == nullptr || entity->getParent() == Entity::NoParent) {
            if (entity != nullptr) {
                renderEntity(entity, childrenByParent, visited, onEntitySelected);
            }
            continue;
        }

        if (liveEntities.find(entity->getParent()) == liveEntities.end()) {
            renderEntity(entity, childrenByParent, visited, onEntitySelected);
        }
    }

    ImGui::End();

    if (changeSceneOpen_)
    {
        const ImVec2 center(
            viewport->WorkPos.x + viewport->WorkSize.x * 0.5f,
            viewport->WorkPos.y + viewport->WorkSize.y * 0.5f);
        ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(320.0f, 220.0f), ImGuiCond_FirstUseEver);

        if (ImGui::Begin("Change Scene", &changeSceneOpen_, ImGuiWindowFlags_NoCollapse))
        {
            for (const auto& scene : runtimeInfo.loadedScenes)
            {
                const bool isCurrent = scene == runtimeInfo.currentScene;
                if (isCurrent) ImGui::BeginDisabled();
                if (ImGui::Button(scene.c_str()) && runtimeInfo.changeScene)
                {
                    runtimeInfo.changeScene(scene);
                    changeSceneOpen_ = false;
                }
                if (isCurrent) ImGui::EndDisabled();
            }
        }
        ImGui::End();
    }
}

void HierarchyInspector::renderEntity(
    const std::shared_ptr<Entity> &entity,
    const ChildMap &childrenByParent,
    std::unordered_set<size_t> &visited,
    const SelectionCallback &onEntitySelected) {
    if (entity == nullptr || !visited.insert(entity->getId()).second) {
        return;
    }

    ImGui::PushID(entity->getId());

    bool hasVisibleChild = false;
    const auto children = childrenByParent.find(entity->getId());
    hasVisibleChild = children != childrenByParent.end() && !children->second.empty();

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth;
    if (!hasVisibleChild) {
        flags |= ImGuiTreeNodeFlags_Leaf;
    }

    const std::string label = entity->getName() + " [" + entity->getTag() + "]  ID " + std::to_string(entity->getId()) + "##" + std::to_string(entity->getId());
    const bool isOpen = ImGui::TreeNodeEx(label.c_str(), flags);
    if (ImGui::IsItemClicked() && onEntitySelected) {
        onEntitySelected(entity);
    }

    if (isOpen) {
        if (children != childrenByParent.end()) {
            for (const auto &child : children->second) {
                renderEntity(child, childrenByParent, visited, onEntitySelected);
            }
        }
        ImGui::TreePop();
    }

    ImGui::PopID();
}
