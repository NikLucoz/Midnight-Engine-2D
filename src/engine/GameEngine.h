#pragma once
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <type_traits>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Keyboard.hpp>
#include "engine/Assets/Assets.h"
#include "engine/camera/Camera.h"
#include "engine/utils/debug_ui/DebugUI.h"
#include "engine/utils/math/Vector2.h"
#include "engine/utils/assets/GameConfigLoader.h"
#include "scenes/Scene.h"

class GameEngine
{
    std::map<std::string, std::shared_ptr<Scene>> scenes_;
    sf::RenderWindow window_;
    sf::Clock clock_;
    std::string currentScene_;
    std::unique_ptr<Camera> camera_;
    std::unique_ptr<Assets> assets_;
    GameConfig gameConfig;
    DebugUI debugUI_;
    bool bIsRunning_;
    Vec2f baseViewSize_;
    bool letterbox_;
    
    Scene* getCurrentScene()
    {
        return scenes_[currentScene_].get();
    }
    
    void handleUserKeyboardInputEvent(sf::Keyboard::Key keyCode, const std::string& actionType);
    void handleUserMouseInputEvent(sf::Mouse::Button button, const std::string& actionType, const Vector2<int> position);
    
    public:
    GameEngine();
    GameEngine(unsigned int width = 800, unsigned int height = 600, const std::string& title = "Game Engine");
    explicit GameEngine(const GameConfig& config);
    
    void init();
    void run();
    void quit();
    void handleEvents();
    void update(float deltaTime);
    void render(float deltaTime);
    void changeScene(const std::string& sceneName);

    template <typename T, typename... Args>
    void registerScene(const std::string& sceneName, Args&&... args)
    {
        static_assert(std::is_base_of_v<Scene, T>, "T must derive from Scene");

        scenes_[sceneName] = std::make_shared<T>(
            this,
            std::forward<Args>(args)...
        );
    }
    
    Assets& getAssets() const;
    Camera& getCamera() const;
    sf::RenderWindow& getWindow();
    const std::string& getCurrentSceneName() const;
    std::vector<std::string> getSceneNames() const;
    DebugOptions& getDebugOptions() { return debugUI_.GetOptions(); }
    const DebugOptions& getDebugOptions() const { return debugUI_.GetOptions(); }
    
    Vec2f getBaseViewSize() const { return baseViewSize_; }
    void setBaseViewSize(Vec2f newSize) { baseViewSize_ = newSize; }
    void handleResize(unsigned int width, unsigned int height);

    void drawTestGrid(sf::RenderWindow& window);

    Vec2f screenToWorldPos(Vec2f screenPosition) const {
        sf::Vector2i screenPos(static_cast<int>(screenPosition.x), static_cast<int>(screenPosition.y));
        sf::Vector2f worldPos = camera_->screenToWorld(screenPos, window_);
        return Vec2f(worldPos.x, worldPos.y);
    }

    Vec2f worldToScreenPos(Vec2f worldPosition) const {
        sf::Vector2i screenCoord = camera_->worldToScreen(worldPosition.toSFVector2(), window_);
        return Vec2f(static_cast<float>(screenCoord.x), static_cast<float>(screenCoord.y));
    }

};
