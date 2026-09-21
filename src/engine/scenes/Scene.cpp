#include "Scene.h"
#include "engine/GameEngine.h"
#include "engine/input/InputManager.h"
#include "engine/input/actions/Action.h"
#include <SFML/Window/Keyboard.hpp>
#include <string>

Scene::Scene(GameEngine *gameEngine, std::unique_ptr<IRenderingSystem> renderingSystem)
    : gameEngine_(gameEngine), renderingSystem_(renderingSystem ? std::move(renderingSystem) : std::make_unique<DefaultRenderingSystem>()) {
    InputManager::getInstance().registerAction(this, InputDevice::Keyboard, static_cast<int>(sf::Keyboard::Key::F3), "Toggle_Debug_UI");
}

void Scene::registerAction(InputDevice device, int code, const std::string& actionName) {
    InputManager::getInstance().registerAction(this, device, code, actionName);
}

void Scene::doAction(const Action &action) {
    if (action.name() == "Toggle_Debug_UI" && action.type() == "pressed") {
        gameEngine_->getDebugOptions().showDebugUI = !gameEngine_->getDebugOptions().showDebugUI;
    }

    sDoAction(action);
}