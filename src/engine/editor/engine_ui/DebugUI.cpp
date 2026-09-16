#include "DebugUI.h"
#include <imgui-SFML.h>
#include <imgui.h>
#include <iostream>

#include "SFML/Window/Keyboard.hpp"
#include "engine/Assets/Assets.h"
#include "engine/camera/Camera.h"

namespace {
constexpr float consoleHeight = 240.0f;
}

DebugUI::DebugUI() {}

void DebugUI::Init(sf::RenderWindow &window, const Assets &assets) {
    Window = &window;
    if (!ImGui::SFML::Init(*Window)) {
        std::cerr << "Failed to initialize ImGui-SFML!\n";
        return;
    }

    initialized_ = true;
    ImFont *arialFont = ImGui::GetIO().Fonts->AddFontFromFileTTF(assets.getFontPath("fontArial").c_str(), 16.0f);
    if (arialFont != nullptr) {
        ImGui::GetIO().FontDefault = arialFont;
        (void)ImGui::SFML::UpdateFontTexture();
    }

    ImGuiStyle &style = ImGui::GetStyle();
    style.ScaleAllSizes(1.0f);
    style.WindowRounding = 6.0f;
    style.ChildRounding = 4.0f;
    style.FrameRounding = 4.0f;
    style.PopupRounding = 4.0f;
    style.ScrollbarRounding = 4.0f;
    style.GrabRounding = 4.0f;
    style.WindowPadding = ImVec2(12.0f, 12.0f);
    style.FramePadding = ImVec2(8.0f, 5.0f);
}

void DebugUI::Update(sf::Time deltaTime, const DebugRuntimeInfo &runtimeInfo, const std::function<void()> &drawSceneContent) {
    frameStarted_ = false;
    if (!initialized_ || !visible_)
        return;

    ImGui::SFML::Update(*Window, deltaTime);
    frameStarted_ = true;

    const float consoleWindowHeight = consoleCollapsed_ ? ImGui::GetFrameHeight() : consoleHeight;
    const ImGuiViewport *viewport = ImGui::GetMainViewport();

    hierarchyInspector_.render(
        runtimeInfo, EntityManager::getInstance().getEntities(), [this](const std::shared_ptr<Entity> &entity) { entityInspector_.setCurrentEntity(entity); }, consoleWindowHeight);

    ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x, viewport->WorkPos.y + viewport->WorkSize.y - consoleWindowHeight), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, consoleHeight), ImGuiCond_Always);

    entityInspector_.render(*Window, runtimeInfo, consoleWindowHeight);

    /*
    ImGui::Begin("Console", nullptr, ImGuiWindowFlags_NoMove);
    consoleCollapsed_ = ImGui::IsWindowCollapsed();

    ImGui::TextColored(ImVec4(0.35f, 0.78f, 1.0f, 1.0f), "ENGINE CONSOLE");
    ImGui::SameLine();
    ImGui::TextDisabled("F3 to toggle");
    ImGui::Separator();

    if (ImGui::BeginTabBar("DebugTabBar"))
    {
        if (ImGui::BeginTabItem("Overview"))
        {
            const auto& entities = EntityManager::getInstance().getEntities();
            ImGui::Text("Runtime");
            ImGui::Spacing();
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
                        ImGui::TextUnformatted(scene.c_str());
                    }
                    ImGui::EndTable();
                }
                ImGui::EndChild();
            }

            ImGui::SeparatorText("Loaded assets");
            if (ImGui::BeginTable("asset_summary", 4,
                ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                ImGuiTableFlags_SizingStretchSame))
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
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Scene"))
        {
            if (drawSceneContent) drawSceneContent();
            if (ImGui::CollapsingHeader("Rendering", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Checkbox("Render system", &options_.systems.render);
                ImGui::Checkbox("Collision debug geometry", &options_.showCollisionGeometry);
            }
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::End();
    */

    if (runtimeInfo.camera) {
        if (options_.freeCameraActive) {
            runtimeInfo.camera->clearTarget();
        }

        if (pendingPanDelta_.x != 0.f || pendingPanDelta_.y != 0.f) {
            const float zoom = runtimeInfo.camera->getZoom();
            Vector2<float> worldDelta{pendingPanDelta_.x / zoom, pendingPanDelta_.y / zoom};
            runtimeInfo.camera->setPosition(runtimeInfo.camera->getPosition() + worldDelta);
        }
    }
    pendingPanDelta_ = {0.f, 0.f};
}

void DebugUI::Render() {
    if (initialized_ && frameStarted_)
        ImGui::SFML::Render(*Window);
}

void DebugUI::ProcessEvent(sf::Event &event) {
    if(visible_ == false) return;

    if (initialized_)
        ImGui::SFML::ProcessEvent(*Window, event);

    // --- Ctrl key ---
    if (const auto *keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::LControl || keyPressed->code == sf::Keyboard::Key::RControl) {
            bIsInMouseDragMode_ = true;
            options_.freeCameraActive = true;
        }
    }

    if (const auto *keyReleased = event.getIf<sf::Event::KeyReleased>()) {
        if (keyReleased->code == sf::Keyboard::Key::LControl || keyReleased->code == sf::Keyboard::Key::RControl) {
            bIsInMouseDragMode_ = false;
            bIsDragging_ = false;
            options_.freeCameraActive = false;
        }
    }

    // --- Mouse button ---
    if (const auto *mousePressed = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mousePressed->button == sf::Mouse::Button::Middle && bIsInMouseDragMode_) {
            bIsDragging_ = true;
            lastMousePos_ = sf::Mouse::getPosition(*Window);
        }
    }

    if (const auto *mouseReleased = event.getIf<sf::Event::MouseButtonReleased>()) {
        if (mouseReleased->button == sf::Mouse::Button::Middle) {
            bIsDragging_ = false;
        }
    }

    // --- Mouse move (the actual panning) ---
    if (const auto *mouseMoved = event.getIf<sf::Event::MouseMoved>()) {
        if (bIsDragging_) {
            sf::Vector2i currentPos = sf::Mouse::getPosition(*Window);
            sf::Vector2i delta = currentPos - lastMousePos_;
            lastMousePos_ = currentPos;

            pendingPanDelta_.x -= static_cast<float>(delta.x);
            pendingPanDelta_.y -= static_cast<float>(delta.y);
        }
    }
}

void DebugUI::SetVisible(bool visible) { visible_ = visible; }

bool DebugUI::IsVisible() const { return visible_; }

bool DebugUI::WantsKeyboardInput() const { return initialized_ && visible_ && ImGui::GetIO().WantCaptureKeyboard; }

bool DebugUI::WantsMouseInput() const { return initialized_ && visible_ && ImGui::GetIO().WantCaptureMouse; }

bool DebugUI::GetAnyItemHovered() { return initialized_ && visible_ && ImGui::IsAnyItemHovered(); }

DebugOptions &DebugUI::GetOptions() { return options_; }

const DebugOptions &DebugUI::GetOptions() const { return options_; }
