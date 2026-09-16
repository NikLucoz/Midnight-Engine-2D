#pragma once
#include "engine/camera/Camera.h"
#include "engine/entities/Entity.h"
#include "engine/editor/engine_ui/EntityInspector.h"
#include "engine/editor/engine_ui/HierarchyInspector.h"
#include <SFML/Graphics.hpp>
#include <cstddef>
#include <functional>
#include <memory>
#include <vector>


class Assets;

using EntityVec = std::vector<std::shared_ptr<Entity>>;

struct DebugRuntimeInfo {
    float framesPerSecond = 0.0f;
    sf::Vector2u windowSize;
    std::string currentScene;
    std::vector<std::string> loadedScenes;
    std::vector<std::string> textures;
    std::vector<std::string> animations;
    std::vector<std::string> sounds;
    std::vector<std::string> fonts;
	std::function<void(const std::string &)> changeScene;
    Camera* camera = nullptr;
};

struct DebugOptions {
    bool showDebugUI = false;
    bool showEntityIds = false;
    bool showCollisionGeometry = false;
	bool freeCameraActive = false;

    struct {
        bool render = true;
        bool movement = true;
        bool collision = true;
        bool lifespan = true;
        bool animation = true;
        bool enemySpawner = false;
    } systems;
};

class DebugUI {
  private:
    sf::RenderWindow *Window = nullptr;
    bool initialized_ = false;
    bool frameStarted_ = false;
    bool visible_ = false;
    bool consoleCollapsed_ = false;
    DebugOptions options_;
    EntityInspector entityInspector_;
    HierarchyInspector hierarchyInspector_;
	bool bIsInMouseDragMode_ = false;
	bool bIsDragging_ = false;
	sf::Vector2i lastMousePos_ = sf::Vector2i(0,0);
	sf::Vector2f  pendingPanDelta_{0.f, 0.f};

  public:
    DebugUI();
    void Init(sf::RenderWindow &window, const Assets &assets);
    void Update(sf::Time deltaTime, const DebugRuntimeInfo &runtimeInfo, const std::function<void()> &drawSceneContent);
    void Render();
    void ProcessEvent(sf::Event &event);
    void SetVisible(bool visible);
    bool IsVisible() const;
    bool WantsKeyboardInput() const;
    bool WantsMouseInput() const;
    bool GetAnyItemHovered();
    DebugOptions &GetOptions();
    const DebugOptions &GetOptions() const;
};