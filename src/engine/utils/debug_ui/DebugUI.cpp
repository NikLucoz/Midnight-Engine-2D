#include "DebugUI.h"
#include <map>
#include <imgui.h>
#include <imgui-SFML.h>
#include <iostream>

#include "engine/entities/EntityManager.h"
#include "engine/components/CTransform.h"
#include "engine/Assets/Assets.h"

DebugUI::DebugUI() {}

void DebugUI::Init(sf::RenderWindow& window, const Assets& assets)
{

    Window = &window;

    if (!ImGui::SFML::Init(*Window)) {
        std::cerr << "Failed to initialize ImGui-SFML!\n";
        return;
    }

	initialized_ = true;
    ImFont* arialFont = ImGui::GetIO().Fonts->AddFontFromFileTTF(assets.getFontPath("fontArial").c_str(), 16.0f);
    if (arialFont != nullptr)
    {
        ImGui::GetIO().FontDefault = arialFont;
        (void)ImGui::SFML::UpdateFontTexture();
    }

    ImGui::GetStyle().ScaleAllSizes(1.0f);
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 6.0f;
    style.ChildRounding = 4.0f;
    style.FrameRounding = 4.0f;
    style.PopupRounding = 4.0f;
    style.ScrollbarRounding = 4.0f;
    style.GrabRounding = 4.0f;
    style.WindowPadding = ImVec2(12.0f, 12.0f);
    style.FramePadding = ImVec2(8.0f, 5.0f);
}

void DebugUI::Update(sf::Time deltaTime, const DebugRuntimeInfo& runtimeInfo,
    const std::function<void()>& drawSceneContent,
    const std::function<void(const std::string&)>& changeScene)
{
    frameStarted_ = false;
    if (!initialized_ || !visible_) return;

    ImGui::SFML::Update(*Window, deltaTime);
	frameStarted_ = true;

    ImGui::SetNextWindowSize(ImVec2(560.0f, 620.0f), ImGuiCond_FirstUseEver);
    ImGui::Begin("Debug Panel");

    ImGui::TextColored(ImVec4(0.35f, 0.78f, 1.0f, 1.0f), "ENGINE DEBUGGER");
    ImGui::SameLine();
    ImGui::TextDisabled("F3 to toggle");
    ImGui::Separator();

    if (ImGui::BeginTabBar("DebugTabBar"))
    {
        if (ImGui::BeginTabItem("Overview"))
        {
            ImGui::Text("Runtime");
            ImGui::Spacing();

            auto& entities = EntityManager::getInstance().getEntities();
            if (ImGui::BeginTable("overview_metrics", 3, ImGuiTableFlags_SizingStretchSame))
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextDisabled("Frame rate");
                ImGui::Text("%.1f FPS", runtimeInfo.framesPerSecond);
                ImGui::TableSetColumnIndex(1);
                ImGui::TextDisabled("Resolution");
                ImGui::Text("%u x %u", runtimeInfo.windowSize.x, runtimeInfo.windowSize.y);
                ImGui::TableSetColumnIndex(2);
                ImGui::TextDisabled("Entities");
                ImGui::Text("%d", static_cast<int>(entities.size()));
                ImGui::EndTable();
            }

            ImGui::Spacing();
            ImGui::SeparatorText("Scene");
            ImGui::Text("Current: %s", runtimeInfo.currentScene.c_str());
            if (ImGui::BeginChild("loaded_scenes", ImVec2(0.0f, 66.0f), true))
            {
                if (ImGui::BeginTable("loaded_scene_rows", 2, ImGuiTableFlags_SizingStretchProp))
                {
                    ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 58.0f);
                    ImGui::TableSetupColumn("Scene", ImGuiTableColumnFlags_WidthStretch);

                    for (const auto& scene : runtimeInfo.loadedScenes)
                    {
                        const bool isCurrent = scene == runtimeInfo.currentScene;
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        if (!isCurrent && ImGui::Button(("Load##" + scene).c_str()) && changeScene)
                        {
                            changeScene(scene);
                        }
                        else if (isCurrent)
                        {
                            ImGui::TextDisabled("Active");
                        }

                        ImGui::TableSetColumnIndex(1);
                        if (isCurrent) ImGui::TextColored(ImVec4(0.35f, 0.78f, 1.0f, 1.0f), "%s", scene.c_str());
                        else ImGui::TextUnformatted(scene.c_str());
                    }

                    ImGui::EndTable();
                }
                ImGui::EndChild();
            }

            ImGui::Spacing();
            ImGui::SeparatorText("Loaded assets");
            if (ImGui::BeginTable("asset_summary", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchSame))
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Textures\n%d", static_cast<int>(runtimeInfo.textures.size()));
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("Animations\n%d", static_cast<int>(runtimeInfo.animations.size()));
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("Sounds\n%d", static_cast<int>(runtimeInfo.sounds.size()));
                ImGui::TableSetColumnIndex(3);
                ImGui::Text("Fonts\n%d", static_cast<int>(runtimeInfo.fonts.size()));
                ImGui::EndTable();
            }

            if (ImGui::CollapsingHeader("Asset names"))
            {
                const auto drawAssetGroup = [](const char* label, const std::vector<std::string>& names)
                {
                    if (ImGui::TreeNode(label))
                    {
                        if (names.empty()) ImGui::TextDisabled("None loaded");
                        for (const auto& name : names) ImGui::BulletText("%s", name.c_str());
                        ImGui::TreePop();
                    }
                };

                drawAssetGroup("Textures", runtimeInfo.textures);
                drawAssetGroup("Animations", runtimeInfo.animations);
                drawAssetGroup("Sounds", runtimeInfo.sounds);
                drawAssetGroup("Fonts", runtimeInfo.fonts);
            }

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Entities"))
        {
            auto& entities = EntityManager::getInstance().getEntities();
            std::map<std::string, EntityVec> entitiesByTag;
            std::map<std::string, EntityVec> entitiesByScene;
            for (const auto& e : entities) {
                entitiesByTag[e->getTag()].push_back(e);
                entitiesByScene[e->getSceneName()].push_back(e);
            }

            static int entityViewMode = 0;
            const char* entityViewModes[] = { "All", "By scene", "By tag" };
            ImGui::TextDisabled("%d entities", static_cast<int>(entities.size()));
            ImGui::SetNextItemWidth(180.0f);
            ImGui::Combo("View", &entityViewMode, entityViewModes, IM_ARRAYSIZE(entityViewModes));
            ImGui::Checkbox("Show entity IDs", &options_.showEntityIds);

            const auto drawEntityTable = [](const char* tableId, const EntityVec& tableEntities)
            {
                if (!ImGui::BeginTable(tableId, 8, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp))
                {
                    return;
                }

                ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 50.0f);
                ImGui::TableSetupColumn("ID");
                ImGui::TableSetupColumn("Name");
                ImGui::TableSetupColumn("Tag");
                ImGui::TableSetupColumn("Scene");
                ImGui::TableSetupColumn("Position");
                ImGui::TableSetupColumn("Velocity");
                ImGui::TableSetupColumn("Rotation");
                ImGui::TableHeadersRow();

                for (const auto& entity : tableEntities)
                {
                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);
                    std::string buttonName = "D##" + std::to_string(entity->getId());
                    if (ImGui::Button(buttonName.c_str(), ImVec2(40.0f, 0.0f)))
                    {
                        entity->destroy();
                    }

                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%d", static_cast<int>(entity->getId()));
                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("%s", entity->getName().c_str());
                    ImGui::TableSetColumnIndex(3);
                    ImGui::Text("%s", entity->getTag().c_str());
                    ImGui::TableSetColumnIndex(4);
                    ImGui::Text("%s", entity->getSceneName().c_str());

                    auto& transform = entity->getComponent<CTransform>();
                    ImGui::TableSetColumnIndex(5);
                    ImGui::Text("(%.1f, %.1f)", transform.position.x, transform.position.y);
                    ImGui::TableSetColumnIndex(6);
                    ImGui::Text("(%.1f, %.1f)", transform.velocity.x, transform.velocity.y);
                    ImGui::TableSetColumnIndex(7);
                    ImGui::Text("%.1f", transform.rotation);
                }

                ImGui::EndTable();
            };

            if (entityViewMode == 0 || entityViewMode == 1)
            {
                const auto& entitiesBy = entityViewMode == 0 ? entitiesByTag : entitiesByScene;
                const char* groupLabel = entityViewMode == 0 ? "tag" : "scene";
                for (const auto& pair : entitiesBy)
                {
                    std::string name = pair.first + " Entities (" + std::to_string(pair.second.size()) + ")";
                    if (ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen) && !pair.second.empty())
                    {
                        std::string tableId = "entities_by_" + std::string(groupLabel) + "##" + pair.first;
                        drawEntityTable(tableId.c_str(), pair.second);
                    }
                }
            }
            else
            {
                if (entities.empty())
                {
                    ImGui::TextDisabled("No entities in the active scene.");
                }
                else
                {
                    drawEntityTable("all_entities_table", entities);
                }
            }

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Scene"))
        {
            if (drawSceneContent) drawSceneContent();

            if (ImGui::CollapsingHeader("Rendering", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Checkbox("Render system", &options_.systems.render);
                ImGui::Checkbox("Collision debug geometry",  &options_.showCollisionGeometry);
            }
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
}

void DebugUI::Render()
{
    if (initialized_ && frameStarted_) ImGui::SFML::Render(*Window);
}

void DebugUI::ProcessEvent(sf::Event& event)
{
    if (initialized_) ImGui::SFML::ProcessEvent(*Window, event);
}

void DebugUI::SetVisible(bool visible)
{
    visible_ = visible;
}

bool DebugUI::IsVisible() const
{
    return visible_;
}

bool DebugUI::WantsKeyboardInput() const
{
    return initialized_ && visible_ && ImGui::GetIO().WantCaptureKeyboard;
}

bool DebugUI::WantsMouseInput() const
{
    return initialized_ && visible_ && ImGui::GetIO().WantCaptureMouse;
}

bool DebugUI::GetAnyItemHovered()
{
    return initialized_ && visible_ && ImGui::IsAnyItemHovered();
}

DebugOptions& DebugUI::GetOptions()
{
    return options_;
}

const DebugOptions& DebugUI::GetOptions() const
{
    return options_;
}