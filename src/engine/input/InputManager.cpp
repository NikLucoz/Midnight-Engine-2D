#include "InputManager.h"
#include "engine/scenes/Scene.h"
#include <SFML/Window/Joystick.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <cmath>
#include <optional>

void InputManager::registerAction(const Scene *scene, InputDevice device, int code, const std::string &actionName, int player) { sceneRegistry_[scene][{device, code, player}] = actionName; }

void InputManager::registerAxis1D(const Scene *scene, int axisId, const std::string &actionName, int player, float deadzone, bool invert, float sensitivity) {
    InputBinding b{InputDevice::GamepadAxis, axisId, player, deadzone, invert, sensitivity};
    sceneRegistry_[scene][b] = actionName;
}

void InputManager::registerAxis2D(const Scene *scene, int xAxisId, int yAxisId, const std::string &actionName, int player, float deadzone, bool invertX, bool invertY, float sensitivity) {
    Axis2DBinding c;
    c.x = {InputDevice::GamepadAxis, xAxisId, player, deadzone, invertX, sensitivity};
    c.y = {InputDevice::GamepadAxis, yAxisId, player, deadzone, invertY, sensitivity};
    c.actionName = actionName;
    axis2DRegistry_[scene].push_back(c);
}

bool InputManager::hasActionMappedForScene(const Scene *scene, const std::string &actionName) const {
    auto it = sceneRegistry_.find(scene);
    if (it != sceneRegistry_.end()) {
        for (const auto &[_, name] : it->second) {
            if (name == actionName)
                return true;
        }
    }
    auto it2 = axis2DRegistry_.find(scene);
    if (it2 != axis2DRegistry_.end()) {
        for (const auto &c : it2->second) {
            if (c.actionName == actionName)
                return true;
        }
    }
    return false;
}

float InputManager::processAxis(float raw, const InputBinding &b) const {
    // SFML returns -100 … +100
    float v = raw / 100.f;
    if (std::abs(v) < b.deadzone)
        return 0.f;
    if (b.invert)
        v = -v;
    return v * b.sensitivity;
}

std::optional<Action> InputManager::resolveDigital(const Scene *scene, InputBinding binding, bool currentlyDown, Vector2<int> pos) {
    auto sceneIt = sceneRegistry_.find(scene);
    if (sceneIt == sceneRegistry_.end())
        return std::nullopt;

    auto actionIt = sceneIt->second.find(binding);
    if (actionIt == sceneIt->second.end())
        return std::nullopt;

    bool wasDown = previousDigitalState_[binding];
    previousDigitalState_[binding] = currentlyDown;

    bool pressed = currentlyDown && !wasDown;
    bool released = !currentlyDown && wasDown;
    bool held = currentlyDown;

    // Only emit an Action when something interesting happened
    if (!pressed && !released && !held)
        return std::nullopt;

    return Action(actionIt->second, pressed, released, held, pos);
}

void InputManager::processEvent(std::optional<sf::Event> event, Scene *scene, DebugUI &debugUI) {
    if (!event)
        return;

    // Keyboard
    if (const auto *kp = event->getIf<sf::Event::KeyPressed>()) {
        if (kp->code == sf::Keyboard::Key::F3 || !debugUI.WantsKeyboardInput()) {
            auto a = resolveDigital(scene, {InputDevice::Keyboard, (int)kp->code}, true);
            if (a)
                scene->doAction(*a);
        }
    }
    if (const auto *kr = event->getIf<sf::Event::KeyReleased>()) {
        if (kr->code != sf::Keyboard::Key::F3 && !debugUI.WantsKeyboardInput()) {
            auto a = resolveDigital(scene, {InputDevice::Keyboard, (int)kr->code}, false);
            if (a)
                scene->doAction(*a);
        }
    }

    // Mouse buttons
    if (const auto *mbp = event->getIf<sf::Event::MouseButtonPressed>()) {
        if (!debugUI.WantsMouseInput()) {
            auto a = resolveDigital(scene, {InputDevice::MouseButton, (int)mbp->button}, true, {mbp->position.x, mbp->position.y});
            if (a)
                scene->doAction(*a);
        }
    }
    if (const auto *mbr = event->getIf<sf::Event::MouseButtonReleased>()) {
        if (!debugUI.WantsMouseInput()) {
            auto a = resolveDigital(scene, {InputDevice::MouseButton, (int)mbr->button}, false, {mbr->position.x, mbr->position.y});
            if (a)
                scene->doAction(*a);
        }
    }

    // gamepad
    if (const auto *jbp = event->getIf<sf::Event::JoystickButtonPressed>()) {
        auto a = resolveDigital(scene, {InputDevice::GamepadButton, (int)jbp->button, (int)jbp->joystickId}, true);
        if (a)
            scene->doAction(*a);
    }
    if (const auto *jbr = event->getIf<sf::Event::JoystickButtonReleased>()) {
        auto a = resolveDigital(scene, {InputDevice::GamepadButton, (int)jbr->button, (int)jbr->joystickId}, false);
        if (a)
            scene->doAction(*a);
    }
}

void InputManager::update(Scene *scene) {
    if (!scene)
        return;

    auto sceneIt = sceneRegistry_.find(scene);
    if (sceneIt != sceneRegistry_.end()) {
        for (const auto &[binding, actionName] : sceneIt->second) {
            if (binding.device != InputDevice::GamepadAxis)
                continue;
            if (!sf::Joystick::isConnected(binding.player))
                continue;

            float raw = sf::Joystick::getAxisPosition(binding.player, static_cast<sf::Joystick::Axis>(binding.code));
            float value = processAxis(raw, binding);

            scene->doAction(Action(actionName, value));
        }
    }

    auto axisIt = axis2DRegistry_.find(scene);
    if (axisIt != axis2DRegistry_.end()) {
        for (const auto &c : axisIt->second) {
            if (!sf::Joystick::isConnected(c.x.player))
                continue;

            float rawX = sf::Joystick::getAxisPosition(c.x.player, static_cast<sf::Joystick::Axis>(c.x.code));
            float rawY = sf::Joystick::getAxisPosition(c.y.player, static_cast<sf::Joystick::Axis>(c.y.code));

            Vec2f v{processAxis(rawX, c.x), processAxis(rawY, c.y)};

            scene->doAction(Action(c.actionName, v.normalized()));
        }
    }
}