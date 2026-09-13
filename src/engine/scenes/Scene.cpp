#include "Scene.h"
#include "engine/GameEngine.h"
#include "engine/actions/Action.h"
#include <SFML/Window/Keyboard.hpp>

Scene::Scene(GameEngine* gameEngine) : gameEngine_(gameEngine), renderingSystem_(std::make_unique<DefaultRenderingSystem>())
{
	registerAction(InputDevice::Keyboard, static_cast<int>(sf::Keyboard::Key::F3), "Toggle_Debug_UI");
}

void Scene::doAction(const Action &action)
{
    if (action.name() == "Toggle_Debug_UI" && action.type() == "pressed")
    {
        gameEngine_->getDebugOptions().showDebugUI = !gameEngine_->getDebugOptions().showDebugUI;
    }

    sDoAction(action);
}

void Scene::registerAction(InputDevice device, int code, const std::string& actionName)
{
    actionMap_[{ device, code }] = actionName;
}

ActionMap& Scene::getActionMap()
{
    return actionMap_;
}
