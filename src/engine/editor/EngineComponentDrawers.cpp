#include "ComponentDrawerRegistry.h"
#include "engine/components/CTransform.h"
#include "engine/components/collision/CBoundingBox.h"
#include "engine/components/collision/CCircleCollider.h"
#include "engine/components/rendering/CAnimatedSprite.h"
#include "engine/components/rendering/CRenderable.h"
#include "engine/components/rendering/CShape.h"
#include "engine/components/rendering/CSprite.h"
#include "engine/editor/EditorWidgets.h"
#include "example_game/components/CLifespan.h"
#include <algorithm>
#include <imgui.h>

// ---------- Static auto-registration ----------
namespace {

REGISTER_COMPONENT_DRAWER(CTransform, "CTransform", {
    if (ImGui::TreeNode("CTransform")) {
        drawVector2("Position", c.position);
        drawVector2("Velocity", c.velocity);
        ImGui::InputFloat("Rotation", &c.rotation);
        drawVector2("Scale", c.scale);
        ImGui::TreePop();
    }
});

REGISTER_COMPONENT_DRAWER(CRenderable, "CRenderable", {
    if (ImGui::TreeNode("CRenderable")) {
        bool visible = c.isVisible();
        if (ImGui::Checkbox("Visible", &visible)) {
            c.setVisible(visible);
        }

        int renderLayer = c.getRenderLayer();
        if (ImGui::InputInt("Render layer", &renderLayer)) {
            c.setRenderLayer(renderLayer);
        }

        int orderInLayer = c.getOrderInLayer();
        if (ImGui::InputInt("Order", &orderInLayer)) {
            c.setOrderInLayer(orderInLayer);
        }

        ImGui::TreePop();
    }
});

REGISTER_COMPONENT_DRAWER(CBoundingBox, "CBoundingBox", {
    if (ImGui::TreeNode("CBoundingBox")) {
        drawVector2("Size", c.size);
        if (c.size.x < 0 || c.size.y < 0) {
            c.size.x = std::max(c.size.x, 0);
            c.size.y = std::max(c.size.y, 0);
        }
        ImGui::TreePop();
    }
});

REGISTER_COMPONENT_DRAWER(CCircleCollider, "CCircleCollider", {
    if (ImGui::TreeNode("CCircleCollider")) {
        if (ImGui::InputFloat("Radius", &c.radius_)) {
            c.radius_ = std::max(c.radius_, 0.0f);
        }
        ImGui::TreePop();
    }
});

REGISTER_COMPONENT_DRAWER(CShape, "CShape", {
    if (ImGui::TreeNode("CShape")) {
        if (ImGui::InputFloat("Radius", &c.radius_)) {
            c.radius_ = std::max(c.radius_, 0.0f);
        }

        int pointCount = c.point_count_;
        if (ImGui::InputInt("Point count", &pointCount)) {
            c.point_count_ = static_cast<int8_t>(std::clamp(pointCount, 3, 255));
        }

        int outlineThickness = c.outlineThickness_;
        if (ImGui::InputInt("Outline thickness", &outlineThickness)) {
            c.outlineThickness_ = static_cast<uint16_t>(std::clamp(outlineThickness, 0, 65535));
        }

        drawColor("Fill color", c.fillColor_);
        drawColor("Outline color", c.outlineColor_);
        ImGui::TreePop();
    }
});

REGISTER_COMPONENT_DRAWER(CSprite, "CSprite", {
    if (ImGui::TreeNode("CSprite")) {
        drawVector2("Size", c.m_size);
        drawVector2("Origin", c.m_origin);
        drawColor("Color", c.m_color);
        ImGui::TextDisabled("Texture cannot be changed here");
        ImGui::TreePop();
    }
});

REGISTER_COMPONENT_DRAWER(CAnimatedSprite, "CAnimatedSprite", {
    if (ImGui::TreeNode("CAnimatedSprite")) {
        if (c.animation != nullptr) {
            ImGui::Text("Animation: %s", c.animation->getName().c_str());
            ImGui::Text("Frame size: %d x %d", c.animation->getSize().x, c.animation->getSize().y);

            bool loopable = c.animation->getIsLoopable();
            if (ImGui::Checkbox("Loopable", &loopable)) {
                c.animation->setIsLoopable(loopable);
            }
        } else {
            ImGui::TextDisabled("No animation assigned");
        }
        ImGui::TreePop();
    }
});

REGISTER_COMPONENT_DRAWER(CLifespan, "CLifespan", {
    if (ImGui::TreeNode("CLifespan")) {
        ImGui::InputFloat("Remaining seconds", &c.remainingSeconds_);
        ImGui::InputFloat("Total seconds", &c.lifespanSeconds_);
        ImGui::TreePop();
    }
});

} // namespace