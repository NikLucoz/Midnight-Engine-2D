#include "EntityInspector.h"
#include "SFML/Graphics/RenderTarget.hpp"
#include "engine/components/collision/CBoundingBox.h"
#include "engine/components/collision/CCircleCollider.h"
#include "engine/components/CTransform.h"
#include "engine/components/rendering/CAnimatedSprite.h"
#include "engine/components/rendering/CShape.h"
#include "engine/components/rendering/CSprite.h"
#include "engine/components/rendering/CRenderable.h"
#include "example_game/components/CInput.h"
#include "example_game/components/CLifespan.h"
#include "example_game/components/CSpecialBullet.h"
#include "engine/entities/Entity.h"
#include "engine/entities/EntityManager.h"
#include "imgui.h"
#include <algorithm>
#include <cstdint>
#include <memory>

namespace {
constexpr float panelWidth = 380.0f;
}

void drawVector2(const char *label, Vec2f &value) {
    if (ImGui::BeginTable(label, 3, ImGuiTableFlags_SizingStretchProp)) {
        ImGui::TableSetupColumn("##property", ImGuiTableColumnFlags_WidthFixed, 105.0f);
        ImGui::TableSetupColumn("X");
        ImGui::TableSetupColumn("Y");
        ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
        ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, IM_COL32(0, 0, 0, 0));
        ImGui::TableSetColumnIndex(1);
        ImGui::TextDisabled("X");
        ImGui::TableSetColumnIndex(2);
        ImGui::TextDisabled("Y");
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted(label);
        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputFloat("##x", &value.x);
        ImGui::TableSetColumnIndex(2);
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputFloat("##y", &value.y);
        ImGui::EndTable();
    }
}

void drawVector2(const char *label, Vector2<int> &value) {
    if (ImGui::BeginTable(label, 3, ImGuiTableFlags_SizingStretchProp)) {
        ImGui::TableSetupColumn("##property", ImGuiTableColumnFlags_WidthFixed, 105.0f);
        ImGui::TableSetupColumn("X");
        ImGui::TableSetupColumn("Y");
        ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
        ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, IM_COL32(0, 0, 0, 0));
        ImGui::TableSetColumnIndex(1);
        ImGui::TextDisabled("X");
        ImGui::TableSetColumnIndex(2);
        ImGui::TextDisabled("Y");
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted(label);
        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputInt("##x", &value.x);
        ImGui::TableSetColumnIndex(2);
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputInt("##y", &value.y);
        ImGui::EndTable();
    }
}


void drawTransform(CTransform &transform) {
    if (ImGui::TreeNode("CTransform")) {
        drawVector2("Position", transform.position);
        drawVector2("Velocity", transform.velocity);
        ImGui::InputFloat("Rotation", &transform.rotation);
        drawVector2("Scale", transform.scale);

        ImGui::TreePop();
    }
}

void drawRenderable(CRenderable &renderable) {
    if (ImGui::TreeNode("CRenderable")) {
        bool visible = renderable.isVisible();
        if (ImGui::Checkbox("Visible", &visible)) {
            renderable.setVisible(visible);
        }

        int renderLayer = renderable.getRenderLayer();
        if (ImGui::InputInt("Render layer", &renderLayer)) {
            renderable.setRenderLayer(renderLayer);
        }

        int orderInLayer = renderable.getOrderInLayer();
        if (ImGui::InputInt("Order", &orderInLayer)) {
            renderable.setOrderInLayer(orderInLayer);
        }

        ImGui::TreePop();
    }
}

void drawColor(const char *label, sf::Color &color) {
    float values[4] = {
        color.r / 255.0f,
        color.g / 255.0f,
        color.b / 255.0f,
        color.a / 255.0f
    };

    if (ImGui::ColorEdit4(label, values)) {
        color = sf::Color(
            static_cast<std::uint8_t>(values[0] * 255.0f),
            static_cast<std::uint8_t>(values[1] * 255.0f),
            static_cast<std::uint8_t>(values[2] * 255.0f),
            static_cast<std::uint8_t>(values[3] * 255.0f));
    }
}

void drawBoundingBox(CBoundingBox &boundingBox) {
    if (ImGui::TreeNode("CBoundingBox")) {
        drawVector2("Size", boundingBox.size);
        if (boundingBox.size.x < 0 || boundingBox.size.y < 0) {
            boundingBox.size.x = std::max(boundingBox.size.x, 0);
            boundingBox.size.y = std::max(boundingBox.size.y, 0);
        }
        ImGui::TreePop();
    }
}

void drawCircleCollider(CCircleCollider &collider) {
    if (ImGui::TreeNode("CCircleCollider")) {
        if (ImGui::InputFloat("Radius", &collider.radius_)) {
            collider.radius_ = std::max(collider.radius_, 0.0f);
        }
        ImGui::TreePop();
    }
}

void drawShape(CShape &shape) {
    if (ImGui::TreeNode("CShape")) {
        if (ImGui::InputFloat("Radius", &shape.radius_)) {
            shape.radius_ = std::max(shape.radius_, 0.0f);
        }

        int pointCount = shape.point_count_;
        if (ImGui::InputInt("Point count", &pointCount)) {
            shape.point_count_ = static_cast<int8_t>(std::clamp(pointCount, 3, 255));
        }

        int outlineThickness = shape.outlineThickness_;
        if (ImGui::InputInt("Outline thickness", &outlineThickness)) {
            shape.outlineThickness_ = static_cast<uint16_t>(std::clamp(outlineThickness, 0, 65535));
        }

        drawColor("Fill color", shape.fillColor_);
        drawColor("Outline color", shape.outlineColor_);
        ImGui::TreePop();
    }
}

void drawSprite(CSprite &sprite) {
    if (ImGui::TreeNode("CSprite")) {
        drawVector2("Size", sprite.m_size);
        drawVector2("Origin", sprite.m_origin);
        drawColor("Color", sprite.m_color);
        ImGui::TextDisabled("Texture cannot be changed here");
        ImGui::TreePop();
    }
}

void drawAnimatedSprite(CAnimatedSprite &animatedSprite) {
    if (ImGui::TreeNode("CAnimatedSprite")) {
        if (animatedSprite.animation != nullptr) {
            ImGui::Text("Animation: %s", animatedSprite.animation->getName().c_str());
            ImGui::Text("Frame size: %d x %d",
                animatedSprite.animation->getSize().x,
                animatedSprite.animation->getSize().y);

            bool loopable = animatedSprite.animation->getIsLoopable();
            if (ImGui::Checkbox("Loopable", &loopable)) {
                animatedSprite.animation->setIsLoopable(loopable);
            }
        } else {
            ImGui::TextDisabled("No animation assigned");
        }
        ImGui::TreePop();
    }
}

void drawInput(CInput &input) {
    if (ImGui::TreeNode("CInput")) {
        ImGui::Checkbox("Up", &input.bUp);
        ImGui::Checkbox("Down", &input.bDown);
        ImGui::Checkbox("Left", &input.bLeft);
        ImGui::Checkbox("Right", &input.bRight);
        ImGui::Checkbox("Mouse left", &input.bMouseLeft);
        ImGui::Checkbox("Mouse right", &input.bMouseRight);
        ImGui::TreePop();
    }
}

void drawLifespan(CLifespan &lifespan) {
    if (ImGui::TreeNode("CLifespan")) {
        ImGui::InputFloat("Remaining seconds", &lifespan.remainingSeconds_);
        ImGui::InputFloat("Total seconds", &lifespan.lifespanSeconds_);
        ImGui::TreePop();
    }
}

void drawSpecialBullet(CSpecialBullet &bullet) {
    if (ImGui::TreeNode("CSpecialBullet")) {
        ImGui::InputFloat("Oscillation time", &bullet.oscillationTime);
        ImGui::TreePop();
    }
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

    if (currentEntity_->hasComponent<CTransform>()) {
        drawTransform(currentEntity_->getComponent<CTransform>());
    }

    if (currentEntity_->hasComponent<CRenderable>()) {
        drawRenderable(currentEntity_->getComponent<CRenderable>());
    }

    if (currentEntity_->hasComponent<CBoundingBox>()) {
        drawBoundingBox(currentEntity_->getComponent<CBoundingBox>());
    }

    if (currentEntity_->hasComponent<CCircleCollider>()) {
        drawCircleCollider(currentEntity_->getComponent<CCircleCollider>());
    }

    if (currentEntity_->hasComponent<CShape>()) {
        drawShape(currentEntity_->getComponent<CShape>());
    }

    if (currentEntity_->hasComponent<CSprite>()) {
        drawSprite(currentEntity_->getComponent<CSprite>());
    }

    if (currentEntity_->hasComponent<CAnimatedSprite>()) {
        drawAnimatedSprite(currentEntity_->getComponent<CAnimatedSprite>());
    }

    if (currentEntity_->hasComponent<CInput>()) {
        drawInput(currentEntity_->getComponent<CInput>());
    }

    if (currentEntity_->hasComponent<CLifespan>()) {
        drawLifespan(currentEntity_->getComponent<CLifespan>());
    }

    if (currentEntity_->hasComponent<CSpecialBullet>()) {
        drawSpecialBullet(currentEntity_->getComponent<CSpecialBullet>());
    }

    /*
    ImGui::SeparatorText("Children");

    const auto &children = currentEntity_->getChildrens();
    if (ImGui::TreeNode("Children", "Children (%zu)", children.size())) {
        if (children.empty()) {
            ImGui::TextDisabled("None");
        } else {
            auto &entityManager = EntityManager::getInstance();
            for (const size_t childId : children) {
                auto child = entityManager.getEntityWithId(childId);
                ImGui::PushID(childId);
                ImGui::Text("%zu", childId);
                ImGui::SameLine();
                if (child != nullptr && ImGui::Button("Inspect")) {
                    currentEntity_ = child;
                }
                if (child == nullptr) {
                    ImGui::SameLine();
                    ImGui::TextDisabled("(unavailable)");
                }
                ImGui::PopID();
            }
        }
        ImGui::TreePop();
    }
    */

    ImGui::End();
}