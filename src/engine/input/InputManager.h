#pragma once

#include "SFML/Window/Event.hpp"
#include "engine/editor/engine_ui/DebugUI.h"
#include "engine/input/actions/Action.h"
#include "engine/utils/math/Vector2.h"
#include <map>
#include <optional>
#include <string>
#include <vector>

enum class InputDevice { Keyboard, MouseButton, GamepadButton, GamepadAxis };

struct InputBinding {
    InputDevice device;
    int code;       // key / button / axis_id
    int player = 0; // joystick id (0, 1, …)

    // Axis options (ignored for buttons/keys)
    float deadzone = 0.15f;
    bool invert = false;
    float sensitivity = 1.0f;

    bool operator<(const InputBinding &o) const {
        if (device != o.device)
            return device < o.device;
        if (code != o.code)
            return code < o.code;
        return player < o.player;
    }
};

struct Axis2DBinding {
    InputBinding x;
    InputBinding y;
    std::string actionName;
};

class Scene;

using SceneBindingMap = std::map<InputBinding, std::string>;
using SceneRegistry = std::map<const Scene *, SceneBindingMap>;
using Axis2DRegistry = std::map<const Scene *, std::vector<Axis2DBinding>>;

class InputManager {
  private:
    InputManager() = default;

    SceneRegistry sceneRegistry_;
    Axis2DRegistry axis2DRegistry_;
    std::map<InputBinding, bool> previousDigitalState_;

    std::optional<Action> resolveDigital(const Scene *scene, InputBinding binding, bool currentlyDown, Vector2<int> pos = {0, 0});

    float processAxis(float raw, const InputBinding &b) const;

  public:
    static InputManager &getInstance() {
        static InputManager instance;
        return instance;
    }

    void registerAction(const Scene *scene, InputDevice device, int code, const std::string &actionName, int player = 0);

    void registerAxis1D(const Scene *scene, int axisId, const std::string &actionName, int player = 0, float deadzone = 0.15f, bool invert = false, float sensitivity = 1.0f);

    void registerAxis2D(const Scene *scene, int xAxisId, int yAxisId, const std::string &actionName, int player = 0, float deadzone = 0.15f, bool invertX = false, bool invertY = false, float sensitivity = 1.0f);

    bool hasActionMappedForScene(const Scene *scene, const std::string &actionName) const;

    void processEvent(std::optional<sf::Event> event, Scene *scene, DebugUI &debugUI);
    void update(Scene *scene);
};