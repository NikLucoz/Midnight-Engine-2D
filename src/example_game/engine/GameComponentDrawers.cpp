#include "engine/editor/components_registry/ComponentDrawerRegistry.h"
#include "example_game/components/CInput.h"
#include "example_game/components/CSpecialBullet.h"
#include <imgui.h>

namespace {

REGISTER_COMPONENT_DRAWER(CInput, "CInput", {
    if (ImGui::TreeNode("CInput")) {
        ImGui::Checkbox("Up", &c.bUp);
        ImGui::Checkbox("Down", &c.bDown);
        ImGui::Checkbox("Left", &c.bLeft);
        ImGui::Checkbox("Right", &c.bRight);
        ImGui::Checkbox("Mouse left", &c.bMouseLeft);
        ImGui::Checkbox("Mouse right", &c.bMouseRight);
        ImGui::TreePop();
    }
});

REGISTER_COMPONENT_DRAWER(CSpecialBullet, "CSpecialBullet", {
    if (ImGui::TreeNode("CSpecialBullet")) {
        ImGui::InputFloat("Oscillation time", &c.oscillationTime);
        ImGui::TreePop();
    }
});

}; // namespace