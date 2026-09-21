#include "InputManager.h"
#include "engine/scenes/Scene.h"
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <optional>

std::optional<Action> InputManager::resolveInput(const Scene *scene, InputBinding binding, const std::string &actionType, Vector2<int> pos) {
    auto sceneIt = sceneRegistry_.find(scene);
    if (sceneIt == sceneRegistry_.end())
        return std::nullopt;

    auto actionIt = sceneIt->second.find(binding);
    if (actionIt == sceneIt->second.end())
        return std::nullopt;

    return Action(actionIt->second, actionType, pos);
}

void InputManager::processEvent(std::optional<sf::Event> event, Scene *scene, DebugUI &debugUI) {
    if (const auto *kp = event->getIf<sf::Event::KeyPressed>()) {
        if (kp->code == sf::Keyboard::Key::F3 || !debugUI.WantsKeyboardInput()) {
            auto action = resolveInput(scene, {InputDevice::Keyboard, (int)kp->code}, "pressed");
            if (action)
                scene->doAction(*action);
        }
    }

    if (const auto *kr = event->getIf<sf::Event::KeyReleased>()) {
        if (kr->code != sf::Keyboard::Key::F3 && !debugUI.WantsKeyboardInput()) {
            auto action = resolveInput(scene, {InputDevice::Keyboard, (int)kr->code}, "released");
            if (action)
                scene->doAction(*action);
        }
    }

    if (const auto *mbp = event->getIf<sf::Event::MouseButtonPressed>()) {
        if (!debugUI.WantsMouseInput()) {
            auto action = resolveInput(scene, {InputDevice::MouseButton, (int)mbp->button}, "pressed", Vector2<int>(mbp->position.x, mbp->position.y));
            if (action)
                scene->doAction(*action);
        }
    }

    if (const auto *mbr = event->getIf<sf::Event::MouseButtonReleased>()) {
        if (!debugUI.WantsMouseInput()) {
            auto action = resolveInput(scene, {InputDevice::MouseButton, (int)mbr->button}, "released", Vector2<int>(mbr->position.x, mbr->position.y));
            if (action)
                scene->doAction(*action);
        }
    }
}
