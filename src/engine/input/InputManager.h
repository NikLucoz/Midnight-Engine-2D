#pragma once

#include "SFML/Window/Event.hpp"
#include "engine/editor/engine_ui/DebugUI.h"
#include "engine/input/actions/Action.h"
#include "engine/utils/math/Vector2.h"
#include <map>
#include <optional>
#include <string>

enum class InputDevice { Keyboard, MouseButton, Gamepad };

struct InputBinding {
    InputDevice device;
    int code;

    bool operator<(const InputBinding &other) const {
        if (device != other.device)
            return device < other.device;

        return code < other.code;
    }
};

class Scene;

using SceneBindingMap = std::map<InputBinding, std::string>;
using SceneRegistry = std::map<const Scene *, SceneBindingMap>;

class InputManager {
  private:
    InputManager() = default;
    SceneRegistry sceneRegistry_;

    std::optional<Action> resolveInput(const Scene *scene, InputBinding binding, const std::string &actionType, Vector2<int> pos = {0, 0});

  public:
    static InputManager &getInstance() {
        static InputManager instance;
        return instance;
    }

    void registerAction(const Scene *scene, InputDevice device, int code, const std::string &actionName) { sceneRegistry_[scene][{device, code}] = actionName; }

    bool hasActionMappedForScene(const Scene *scene, const std::string &actionName) const {
        auto sceneIt = sceneRegistry_.find(scene);
        if (sceneIt == sceneRegistry_.end())
            return false;
        for (const auto &[binding, name] : sceneIt->second)
            if (name == actionName)
                return true;
        return false;
    }

    void processEvent(std::optional<sf::Event> event, Scene *scene, DebugUI &debugUI);
};